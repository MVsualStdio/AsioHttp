
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <functional>

using namespace boost::asio;

class Pool{
    public:
    Pool(int size);
    ~Pool();

    void run();
    void stop();
    std::shared_ptr<boost::asio::io_context> get_io_service();

private:
    void stop_io_services();
    void stop_threads();
    void run_thread(std::shared_ptr<boost::asio::io_service> io);
private:
    std::vector<std::shared_ptr<boost::asio::io_service>> ios_pool_;
    std::vector<std::shared_ptr<boost::asio::io_service::work>> work_pool_;
    std::vector<std::shared_ptr<std::thread>> threads_; 
    std::size_t next_io_service_ = 0;
};

Pool::Pool(int size){
    for(int i=0;i<size;++i){
        auto ios = std::make_shared<boost::asio::io_service>();
        auto work = std::make_shared<boost::asio::io_service::work>(*ios);
        ios_pool_.emplace_back(ios);
        work_pool_.emplace_back(work);
    }
}

Pool::~Pool()
{
    stop();
}

void Pool::run()
{
    for (std::size_t i = 0; i < ios_pool_.size(); ++i)
    {
        auto t = std::make_shared<std::thread>(std::bind(&Pool::run_thread,this,ios_pool_[i]));
        threads_.emplace_back(t);
    }
}

void Pool::stop()
{
    stop_io_services();
    stop_threads();
}

std::shared_ptr<boost::asio::io_context> Pool::get_io_service()
{
    auto ios = ios_pool_[next_io_service_];
    ++next_io_service_;
    if (next_io_service_ == ios_pool_.size())
    {
        next_io_service_ = 0;
    }

    return ios;
}

void Pool::stop_io_services()
{
    for (auto& ios : ios_pool_)
    {
        if (ios)
        {
            ios->stop();
        }
    }
}

void Pool::stop_threads()
{
    for (auto& t : threads_)
    {
        if (t && t->joinable())
        {
            t->join();
        }
    }       
}

void Pool::run_thread(std::shared_ptr<boost::asio::io_service> io){
    io->run();
}
