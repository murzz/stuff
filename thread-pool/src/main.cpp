#include <cstdlib>

//#include <set>
//#include <vector>
#include <deque>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/random.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options.hpp>

#include <glog/logging.h>

class thread_pool
{
private:
    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
    boost::thread_group threads_;
    std::size_t available_;
    boost::mutex mutex_;
public:

    /// @brief Constructor.
    thread_pool(std::size_t pool_size) :
            work_(io_service_), available_(pool_size)
    {
        DLOG(INFO)<< __func__;
        for (std::size_t idx = 0; idx < pool_size; ++idx)
        {

            threads_.create_thread(
                    boost::bind(&boost::asio::io_service::run, &io_service_));
            DLOG(INFO) << "thread #" << idx << " created";
        }
    }

    /// @brief Destructor.
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
            LOG(ERROR) << "thrown in join_all";
        }
    }

    /// @brief Adds a task to the thread pool if a thread is currently available.
    template<typename Task>
    void run_task(Task task)
    {
        boost::unique_lock<boost::mutex> lock(mutex_);

        // If no threads are available, then return.
        if (0 == available_)
        {
            LOG(WARNING) << "no room inda pool";
            return;
        }

        // Decrement count, indicating thread is no longer available.
        --available_;

        // Post a wrapped task into the queue.
        io_service_.post(
                boost::bind(&thread_pool::wrap_task, this,
                        boost::function<void()>(task)));
    }

    const size_t& available()
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        return available_;
    }

private:
    /// @brief Wrap a task so that the available count can be increased once
    ///        the user provided task has completed.
    void wrap_task(boost::function<void()> task)
    {
        // Run the user supplied task.
        try
        {
           // DLOG(INFO) << "task started";
            task();
            //DLOG(INFO) << "task finished";
        }
        // Suppress all exceptions.
        catch (...)
        {
            LOG(ERROR) << "thrown in task";
        }

        // Task has finished, so increment count of available threads.
        boost::unique_lock<boost::mutex> lock(mutex_);
        ++available_;
    }
};

void work()
{
    LOG(INFO)<< __func__;
}

struct worker//:boost::noncopyable
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
        LOG(INFO)<< boost::this_thread::get_id() <<" work in progress";

        boost::random::mt19937 rng;
        boost::random::uniform_int_distribution<> rnd(1,10);

        boost::this_thread::sleep(boost::posix_time::seconds(rnd(rng)));
        done_=true;
        LOG(INFO)<< boost::this_thread::get_id() <<" work done";
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
    typedef typename container::value_type value_t;
    void push(const value_t& value)
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        container_.push_back(value);
    }
    
    typename container::size_type size()
    {
    boost::unique_lock<boost::mutex> lock(mutex_);
    //std::cout << container_.size() << std::endl;
    return container_.size();
   }

    value_t pop()
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        value_t item = container_.front();
        container_.pop_front();
        return item;
    }
private:
    container container_;
    boost::mutex mutex_;
};

template<class queue, class worker>
struct producer
{
    queue& queue_;
    producer(queue& value) :
            queue_(value)
    {

    }
    void operator()()
    {
        boost::random::mt19937 rng;
        boost::random::uniform_int_distribution<> rnd(1, 2);

        for (int idx = 10;idx; --idx/*, boost::this_thread::sleep(boost::posix_time::seconds(rnd(rng)))*/)
        {
        worker wrk;
        //LOG(INFO) << "+";
            queue_.push(wrk);
        }
        DLOG(INFO)<<"producer done";
    }
};

template<class queue, class worker>
struct consumer
{
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
    wrk.operator()();
    }
    //LOG(INFO) << "-";
    //queue_.pop().operator()();
//        boost::random::mt19937 rng;
//        boost::random::uniform_int_distribution<> rnd(1, 16);
//
//        for (;; boost::this_thread::sleep(boost::posix_time::seconds(rnd(rng))))
//        {
//            queue_.push_back(worker());
//        }
    }
};

int main(int argc, char* argv[])
{

//prinimaj kak parametr razmer poola (GOptions ili boost::program_options)
//sdelaj tak, 4tobi producer 4erez kakoe
//ostanovisj kogda o4eredj pusta
//proverj 4to 4em bolshe pool tem bistree obrabativaetsja o4ered

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    
    namespace po = boost::program_options;
    // Declare the supported options.
po::options_description desc("Allowed options");
desc.add_options()
    ("help", "produce help message")
    ("pool-size", po::value<size_t>(), "set pool size")
;

po::variables_map vm;
po::store(po::parse_command_line(argc, argv, desc), vm);
po::notify(vm);    

if (vm.count("help")) {
    std::cout << desc << std::endl;
    return EXIT_FAILURE;
}

if (vm.count("pool-size")) {
    LOG(INFO) << "pool size set to " 
 << vm["pool-size"].as<size_t>();
} else {
    std::cout << desc << std::endl;
    return EXIT_FAILURE;

}

    typedef queue<std::deque<worker> > queue_t;
    //typedef queue<std::queue<worker> > queue_t;
    queue_t the_queue;

    typedef producer<queue_t, worker> producer_t;
    //producer_t the_producer(the_queue);

    typedef consumer<queue_t, worker> consumer_t;
    //producer_t the_consumer(the_queue);

    //enum
    //{
    //    MAX_POOL_SIZE = 8
    //};
    //thread_pool pool(MAX_POOL_SIZE);
        thread_pool pool(vm["pool-size"].as<size_t>());
    

pool.run_task(producer_t(the_queue));
// sleep until 1st queue item generated

//for(;!the_queue.size();)
//{
boost::this_thread::sleep(boost::posix_time::seconds(5));
//}
        for (; the_queue.size();
                boost::this_thread::sleep(boost::posix_time::seconds(1)))
        {
        LOG(INFO) << "queue size: " << the_queue.size() << ", pool available: " << pool.available();
        if(pool.available())
        {
        pool.run_task(consumer_t(the_queue));
        }
        
        

}

//    typedef boost::shared_ptr<worker> worker_ptr_t;
//    std::set<worker_ptr_t> workers;
//    for (size_t idx = 10; idx; --idx)
//    {
//        worker_ptr_t worker(new ::worker(idx));
//        workers.insert(worker);
//    }
    //producer prod(pool);
    //boost::thread producer(boost::bind(produce, boost::ref(pool)));

    //  std::for_each(workers.begin(), workers.end(),
    //      boost::bind(&thread_pool::run_task, &pool));
    //pool.run_task(work);                        // Function pointer.
    //pool.run_task(worker(1));                    // Callable object.
    //pool.run_task(boost::bind(more_work, 5)); // Callable object.

//    for (; workers.size();
//            boost::this_thread::sleep(boost::posix_time::seconds(1)))
//    {
//        BOOST_FOREACH(worker_ptr_t worker, workers)
//        {
//            if (worker->done_)
//            {
//                //
//            }
//            else
//            {
//                pool.run_task(*worker);
//            }
//        }
//        LOG(INFO)<< "free pool: " << pool.available();
//    }

    return EXIT_SUCCESS;
}
