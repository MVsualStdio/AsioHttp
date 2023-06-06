
#include <memory>
#include <boost/asio.hpp>
#include "Route.hpp"
#include "HttpContext.hpp"

const std::size_t BUF_SIZE = 100000;

using namespace boost::asio;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
	HttpSession(std::shared_ptr<boost::asio::io_context> io,std::shared_ptr<Route> route) 
		:socket_(*io),
		route_(route){
	}

	void Start() {
		DoRead();
	}

	ip::tcp::socket& GetSocket(){
        return socket_;
    }

    void DoRead() {
        auto self(shared_from_this());
        socket_.async_read_some(
            boost::asio::buffer(buffer_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
					Net::HttpContext contest;
                    
                    std::string parse_string(buffer_.begin(),0,length);
                    contest.parseRequest(parse_string);
                    std::cout<<parse_string<<std::endl;
                    // if(!contest.gotAll()){
                    //     DoRead();
                    //     return ;
                    // }
                    auto req = contest.request();
                    std::cout<<socket_.remote_endpoint().address()<<": "<<req.path()<<std::endl;
                    
                    auto F = route_->GetFunction(req.path());
                    
                    if(F == nullptr){
                        F = std::bind(&Route::NotFound,route_,std::placeholders::_1,std::placeholders::_2);
                        std::cout<<"NULL"<<std::endl;
                    }
                    Net::HttpResponse res(false);

                    F(req,res);
                    write_buff_.clear();
                    res.ToString(write_buff_);
                    
					// std::cout<<write_buff_<<std::endl;
                    DoWrite(length);
                }
            });
    }

  void DoWrite(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(write_buff_),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
                DoRead();
            	// boost::system::error_code ig;
				// socket_.close();
                // socket_.shutdown(ip::tcp::socket::shutdown_both,ig);
          }
        });
  }

private:
  ip::tcp::socket socket_;
  std::array<char, BUF_SIZE> buffer_;
  std::shared_ptr<Route> route_;
  std::string write_buff_;
};

