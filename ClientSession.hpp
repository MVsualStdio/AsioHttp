
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <functional>
#include <HttpContext.hpp>
#include <string>
#include <nlohmann/json.hpp>
using namespace boost::asio;
using json = nlohmann::json;


class ClientHttpSession : public std::enable_shared_from_this<ClientHttpSession>{
    public:
        ClientHttpSession(std::shared_ptr<io_context> io):io_(io){

        }

        void ConnectServer(std::string ip,int port){
            ep = std::make_shared<ip::tcp::endpoint>(ip::address::from_string(ip),port);
            sock_ = std::make_shared<ip::tcp::socket>(*io_);
            sock_->connect(*ep);
        }
    private:
        template<class R>
        R callJson(std::string Fun,json args){
            // sock_->connect(*ep);
            Net::HttpRequest req;
            std::string buff;
            std::array<char,1000> readbuf;

            req.setMethod("POST");
            req.setPath(Fun);
            req.setPostBody(args.dump());
            req.ToString(buff);
            
            sock_->write_some(buffer(buff));

            int length = sock_->read_some(buffer(readbuf));
            Net::HttpContext context;
            context.parseResponse(string(readbuf.begin(),length));
            Net::HttpRequest response = context.request();
            std::string body = response.postBody();

            //std::cout<<body<<std::endl;

            json reqjson = json::parse(body);
            // std::cout<<readbuf.begin()<<std::endl;
            return reqjson["res"].get<R>();
        }
    public:
        template<class R,class A1,class A2,class A3>
        R call(std::string Fun,A1 args1,A2 args2,A3 args3){
            json args;
            args["args1"] = args1;
            args["args2"] = args2;
            args["args3"] = args3;
            return callJson<R>(Fun,args);
        }

        template<class R,class A1,class A2>
        R call(std::string Fun,A1 args1,A2 args2){
            json args;
            args["args1"] = args1;
            args["args2"] = args2;
            return callJson<R>(Fun,args);
        }

        template<class R,class A1>
        R call(std::string Fun,A1 args1){
            json args;
            args["args1"] = args1;
            return callJson<R>(Fun,args);
        }

           

    private:
        std::shared_ptr<io_context> io_;
        std::shared_ptr<ip::tcp::socket> sock_;
        std::shared_ptr<ip::tcp::endpoint> ep;
};

