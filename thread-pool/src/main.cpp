#include <cstdlib>

//#include <set>
//#include <vector>
#include <deque>
#include <map>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/random.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>

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
   boost::asio::signal_set signals_;

public:

   boost::asio::io_service& get_io_service()
   {
      return io_service_;
   }

   std::size_t run()
   {
      DLOG(INFO)<< __func__;

      // this thread will also work for us!
      return io_service_.run();
   }

   thread_pool(std::size_t pool_size) :
   work_(io_service_), signals_(io_service_, SIGINT, SIGTERM)
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
      DLOG(INFO) << "stopping service...";
      // Force all threads to return from io_service::run().
      io_service_.stop();

      DLOG(INFO) << "joining threads...";
      try
      {
         threads_.join_all();
      }
      catch (...)
      {
         LOG_EXCEPTION();
      }
      DLOG(INFO) << __func__;
   }

   template<typename Task>
   void post(Task task)
   {
      io_service_.post( boost::bind(task));
   }
};

template<class container>
class queue
{
public:
   typedef typename container::value_type worker_t;

   queue()
   {
      LOG(INFO)<<__func__;
   }

   ~queue()
   {
      LOG(INFO)<<__func__;
   }

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

struct worker
{
   void operator()()
   {
      LOG(INFO)<<"--> starting work";

      boost::random::mt19937 rng;
      boost::random::uniform_int_distribution<> rnd(1,3);

      // simulating hard work
      boost::this_thread::sleep(boost::posix_time::seconds(rnd(rng)));
      LOG(INFO)<<"<-- work done";
   }
};

//void work()
//{
//   LOG(INFO)<< __func__;
//}

//
//void more_work(int)
//{
//   LOG(INFO)<< __func__;
//}

template<class queue>
struct producer
{
   typedef queue queue_t;
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
      typename queue::worker_t work;
      queue_.push(work);
   }
};

template<class queue>
struct consumer
{
   typedef queue queue_t;
   queue& queue_;
   consumer(queue& value) :
      queue_(value)
   {
   }

   void operator()()
   {
      if (queue_.size())
      {
         typename queue::worker_t wrk = queue_.pop();
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

template<typename consumer_or_producer>
struct async_wrapper
{
   //TODO get rid of this
   typedef void result_type;
   typename consumer_or_producer::queue_t& queue_;
   boost::asio::io_service& io_service_;
   async_wrapper(typename consumer_or_producer::queue_t& value,
      boost::asio::io_service& io_service) :
      queue_(value), io_service_(io_service)
   {
   }

   void operator()()
   {
      consumer_or_producer consume_or_produce(queue_);
      consume_or_produce();

      io_service_.post(
         boost::bind(
            async_wrapper<consumer_or_producer>(queue_, io_service_)));
   }
};

struct env
{
   size_t pool_size_;
   static const size_t defalt_pool_size_ = 2;
   env() :
      pool_size_(defalt_pool_size_)
   {
      pool_size_ =
         boost::thread::hardware_concurrency() ?
                                                 boost::thread::hardware_concurrency() :
                                                 defalt_pool_size_;
   }

   int program_options(int argc, char* argv[])
   {
      boost::program_options::options_description desc("Allowed options");
      desc.add_options()
      ("help,h", "produce help message")
      ("pool-size,s", boost::program_options::value<std::size_t>(&pool_size_),
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

      LOG(INFO)<< "pool size is " << pool_size_;
      return EXIT_SUCCESS;
   }
};

// f(n)=f(n-1)+f(n-2)
struct fibo
{
   std::size_t idx_;
   std::size_t& value_;

   fibo(const std::size_t& idx, std::size_t& value) :
      idx_(idx), value_(value)
   {

   }
   
   void calculate()
   {
	  switch (idx_)
      {
         case 0:
         case 1:
            value_ = 0;
            break;
         case 2:
            value_ = 1;
            break;
         default:
            std::size_t value1 = 0, value2 = 0;
			
            fibo f1(idx_ - 2, value1);
            f1.calculate();

            fibo f2(idx_ - 1, value2);
            f2.calculate();

            value_ = value1 + value2;
            break;
      }
   }
   void operator()()
   {
		calculate();
   }
};

//typedef std::map<std::size_t, std::size_t> fibos_t;

//void fibo_single_thread(const fibos_t::value_type& pair)
template<typename container>
void fibo_single_thread(const typename container::value_type& pair)
{
   boost::timer::auto_cpu_timer t;

   std::size_t idx = pair.first;
   std::size_t value = 0;
   fibo fibo(idx, value);
   fibo();

   std::cout << "fibo(" << idx << ") = " << value << std::endl;
   if (pair.second != value)
   {
      std::cout << "!!!!!!!!!!!!!!11 fibo is wrong" << std::endl;
   }
}

template<typename container>
struct fibo_multi_thread
{
   const std::size_t& thread_count_;
   fibo_multi_thread(const std::size_t& thread_count) :
      thread_count_(thread_count)
   {
   }

   void operator()(const typename container::value_type& pair)
   {
      boost::timer::auto_cpu_timer t;

      std::size_t idx = pair.first;
      std::size_t value = 0;
      fibo fibo(idx, value);
      thread_pool pool(thread_count_);
      //pool.post();
      pool.run();

      std::cout << "fibo(" << idx << ") = " << value << std::endl;
      if (pair.second != value)
      {
         std::cout << "!!!!!!!!!!!!!!11 fibo is wrong" << std::endl;
      }
   }
};

int main(int argc, char* argv[])
{
   // glog
   google::InitGoogleLogging(argv[0]);
   google::InstallFailureSignalHandler();

   env env;
   const int result = env.program_options(argc, argv);
   if (EXIT_SUCCESS != result)
   {
      return result;
   }

   // calculate fibonacci numbers using thread pool
   typedef std::map<std::size_t, std::size_t> fibos_t;
   fibos_t fibos =
      {
         { 1, 0 },
         { 2, 1 },
         { 26, 75025 },
         { 31, 832040 },
         { 40, 63245986 },
         { 42, 165580141 },
      //{ 45, 701408733 },
      //{50, 7778742049},
      };

   std::cout << "-----------------------> single thread fibo <-----------------------" << std::endl;
   std::for_each(fibos.begin(), fibos.end(), fibo_single_thread<fibos_t>);

   std::cout << "-----------------------> multi thread fibo <-----------------------" << std::endl;
   std::for_each(fibos.begin(), fibos.end(), fibo_multi_thread<fibos_t>(env.pool_size_));

#if 0
   typedef queue<std::deque<worker> > queue_t;
   typedef async_wrapper<producer<queue_t> > async_producer_t;
   typedef async_wrapper<consumer<queue_t> > async_consumer_t;

// pool needs to be destroyed before queue, otherwise some tasks could still use queue when it is already dead
   queue_t queue;
   {
      thread_pool pool(env.pool_size_);

      // since consumer and producer would repost work after current work item is finished
      // only 2 threads will work. now we create multiple guys for many threads to be busy.
      for (size_t idx = env.pool_size_; idx; --idx)
      {
         // producing work
         pool.post(async_producer_t(queue, pool.get_io_service()));
         // consuming work
         pool.post(async_consumer_t(queue, pool.get_io_service()));
      }

      // add current thread to pool. will block.
      pool.run();
   }
#endif
   return EXIT_SUCCESS;
}
