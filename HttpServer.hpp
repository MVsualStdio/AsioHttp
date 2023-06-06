#include "HttpSession.hpp"
#include "IOPool.hpp"
#include <vector>
#include <list>
#include <iostream>

class HttpServer {
public:
	HttpServer(std::shared_ptr<boost::asio::io_context> ioc, std::uint16_t port,std::shared_ptr<Route> route)
		: acceptor_(*ioc, ip::tcp::endpoint(ip::tcp::v4(), port)),
		io(ioc),
		route_(route),
        pool_(std::make_shared<Pool>(3)){
            pool_->run();
		    DoAccept();
	}
protected:
  void DoAccept() {
	std::shared_ptr<HttpSession> session = std::make_shared<HttpSession>(pool_->get_io_service(),route_);
    acceptor_.async_accept(
		session->GetSocket(),
        [this,session](boost::system::error_code ec) {
          if (!ec) {
            session->Start();
          }
          DoAccept();
        });
    }

protected:
    std::shared_ptr<Pool> pool_;
    ip::tcp::acceptor acceptor_;
    std::shared_ptr<boost::asio::io_context> io;
    std::shared_ptr<Route> route_;
};
