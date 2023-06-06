
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <functional>
#include <HttpRequest.hpp>
#include <string>
#include "ClientSession.hpp"
using namespace boost::asio;

// std::string buffered = "GET / HTTP/1.1\r\n\r\n";
//             // "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
//             // "Accept-Encoding: gzip, deflate\r\n"
//             // "Accept-Language: zh-CN,zh;q=0.9\r\n"
//             // "Cache-Control: max-age=0\r\n"
//             // "Host: 81.70.1.235:6869\r\n"
//             // "Upgrade-Insecure-Requests: 1\r\n"
//             // "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36\r\n\r\n";



int main(){
    std::shared_ptr<io_context> service(new io_context());
    ClientHttpSession session(service);
    session.ConnectServer(("127.0.0.1"),1110);

    auto res =  session.call<int>("add3",1,2,3);
    std::cout<<res<<std::endl;

    auto res2 =  session.call<int>("add2",1,2);
    std::cout<<res2<<std::endl;

    auto res3 =  session.call<int>("add1",1);
    std::cout<<res3<<std::endl;
    
    service->run();
    
}

