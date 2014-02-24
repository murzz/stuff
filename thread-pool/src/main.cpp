#include <cstdlib>

//#include <set>
//#include <vector>
#include <deque>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/random.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

#include <glog/logging.h>

void log_exception(const char* caller)
{
   try
   {
      throw;
   }
   catch (const std::exception& e)
   {
      LOG(ERROR)<< "exception in "<<caller<<": "<<e.what();
   }
   catch(...)
   {
      LOG(ERROR) << "unexpected exception in "<<caller;
   }
}

#define LOG_EXCEPTION() {log_exception(__func__);}

class thread_pool
{
private:
   boost::asio::io_service io_service_;
   boost::asio::io_service::work work_;
   boost::thread_group threads_;
   std::size_t threads_available_;
   boost::asio::signal_set signals_;

   typedef boost::unique_lock<boost::mutex> lock_t;
   typedef lock_t::mutex_type mutex_;

public:

   boost::asio::io_service& get_io_service()
   {
      return io_service_;
   }

   thread_pool(std::size_t pool_size) :
         work_(io_service_), threads_available_(pool_size),
               signals_(io_service_, SIGINT, SIGTERM)
   {
      DLOG(INFO)<< __func__;
      for (std::size_t idx = 0; idx < pool_size; ++idx)
      {
         threads_.create_thread( boost::bind(&boost::asio::io_service::run, boost::ref(io_service_)));
      }

      signals_.async_wait( boost::bind(&boost::asio::io_service::stop, &io_service_));
   }

   ~thread_pool()
   {
      DLOG(INFO) << __func__;
      // Force all threads to return from io_service::run().
      io_service_.stop();

      // Suppress all exceptions.
      try
      {
         threads_.join_all();
      }
      catch (...)
      {
         LOG_EXCEPTION();
      }
   }

   /// @brief Adds a task to the thread pool if a thread is currently available.
   template<typename Task>
   void run_task(Task task)
   {
      lock_t lock(mutex_);
      --threads_available_;

      // Post a wrapped task into the queue.
      io_service_.post(
            boost::bind(&thread_pool::wrap_task<Task>, this,task));
   }

   const size_t& available()
   {
      lock_t lock(mutex_);
      return threads_available_;
   }

private:
   /// @brief Wrap a task so that the available count can be increased once
   ///        the user provided task has completed.
   /// @note If we wont do anything smart here then this method is not needed, right?
   template<typename Task>
   void wrap_task(Task task)
   {
      // Run the user supplied task.
      try
      {
         DLOG(INFO) << "task started";
         task();
         DLOG(INFO) << "task finished";
      }
      // Suppress all exceptions.
      catch (...)
      {
         LOG_EXCEPTION();
      }

      // Task has finished, so increment count of available threads.
      lock_t lock(mutex_);
      ++threads_available_;
   }
};

void work()
{
   LOG(INFO)<< __func__;
}

struct worker    //:boost::noncopyable
{
   worker() :
         done_(false)
   {
   }
   ~worker()
   {
      // LOG_IF(WARNING, !done_) << "work was not done";
      // LOG_IF(INFO, done_) << "work was done";
   }
   void operator()()
   {
      LOG(INFO)<<"--> starting work";

      boost::random::mt19937 rng;
      boost::random::uniform_int_distribution<> rnd(1,3);

      // simulating hard work
      boost::this_thread::sleep(boost::posix_time::seconds(rnd(rng)));
      done_=true;
      LOG(INFO)<<"<-- work done";
   }
   bool done_;
};

void more_work(int)
{
   LOG(INFO)<< __func__;
}

template<class container>
class queue
{
public:
   typedef typename container::value_type worker_t;

   void push(const worker_t& value)
   {
      lock_t lock(mutex_);
      container_.push_back(value);
      LOG(INFO)<<"work pushed, queue size is " << container_.size();
   }

   typename container::size_type size()
   {
      lock_t lock(mutex_);
      return container_.size();
   }

   worker_t pop()
   {
      lock_t lock(mutex_);
      worker_t item = container_.front();
      container_.pop_front();
      LOG(INFO)<<"work popped, queue size is " << container_.size();
      return item;
   }
private:
   container container_;
   typedef boost::unique_lock<boost::mutex> lock_t;
   lock_t::mutex_type mutex_;
};

template<class queue, class worker>
struct producer
{
   //TODO how to get rid of it?
   //typedef void result_type;
   typedef queue queue_t;
   typedef worker worker_t;
   queue& queue_;
   producer(queue& value) :
         queue_(value)
   {
   }

   void operator()()
   {
      // simulate some random delay in generating more work
      boost::random::mt19937 rng;
      boost::random::uniform_int_distribution<> rnd(1, 2);

      // wait for randomness
      boost::this_thread::sleep(boost::posix_time::seconds(rnd(rng)));

      // push some work
      worker work;
      queue_.push(work);
   }
};

template<class queue, class worker>
struct consumer
{
   typedef queue queue_t;
   typedef worker worker_t;
   queue& queue_;
   consumer(queue& value) :
         queue_(value)
   {
   }

   void operator()()
   {
      if (queue_.size())
      {
         worker wrk = queue_.pop();
         wrk();
      }
   }
};

//template <typename queue, typename worker>
//void producer_task(boost::asio::io_service& _io_service, queue& _queue)
//{
//   boost::random::mt19937 rng;
//   boost::random::uniform_int_distribution<> rnd(1, 2);
//
//   // wait for randomness
//   boost::this_thread::sleep(boost::posix_time::seconds(rnd(rng)));
//
//   // push some work
//   worker wrk;
//   _queue.push(wrk);
//}

template<typename queue, typename worker, typename helper>
struct async_wrapper
{
   typedef void result_type;
   queue& queue_;
   boost::asio::io_service& io_service_;
   async_wrapper(queue& value, boost::asio::io_service& io_service) :
         queue_(value), io_service_(io_service)
   {
   }

   void operator()()
   {
      helper do_help(queue_);
      do_help();

      io_service_.post(
            boost::bind(
                  async_wrapper<queue, worker, helper>(queue_, io_service_)));
   }
};

int main(int argc, char* argv[])
{
   // google
   google::InitGoogleLogging(argv[0]);
   google::InstallFailureSignalHandler();

   // defaults for program options
   const size_t defalt_pool_size = 2;
   size_t pool_size =
         boost::thread::hardware_concurrency() ?
               boost::thread::hardware_concurrency() : defalt_pool_size;

   // program options
   boost::program_options::options_description desc("Allowed options");
   desc.add_options()
   ("help,h", "produce help message")
   ("pool-size,s", boost::program_options::value<size_t>(&pool_size),
         "set pool size");

   boost::program_options::variables_map vm;
   boost::program_options::store(
         boost::program_options::parse_command_line(argc, argv, desc), vm);
   boost::program_options::notify(vm);

   if (vm.count("help"))
   {
      std::cout << desc << std::endl;
      return EXIT_FAILURE;
   }

   LOG(INFO)<< "pool size is " << pool_size;

   // defines
   typedef queue<std::deque<worker> > queue_t;
   //typedef queue<std::queue<worker> > queue_t;

   typedef producer<queue_t, queue_t::worker_t> producer_t;
   typedef async_wrapper<producer_t::queue_t, producer_t::worker_t, producer_t> async_producer_t;

   typedef consumer<queue_t, queue_t::worker_t> consumer_t;
   typedef async_wrapper<consumer_t::queue_t, consumer_t::worker_t, consumer_t> async_consumer_t;

   // declarations
   thread_pool pool(pool_size);
   queue_t the_queue;

   // producing work
   pool.run_task(async_producer_t(the_queue, pool.get_io_service()));
   // consuming work
   pool.run_task(async_consumer_t(the_queue, pool.get_io_service()));

   // this thread will also work for us!
   pool.get_io_service().run();

   return EXIT_SUCCESS;
}
