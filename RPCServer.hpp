#include "HttpServer.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class RPCServer : public HttpServer{
    public:
        RPCServer(std::shared_ptr<boost::asio::io_context> ioc, std::uint16_t port,std::shared_ptr<Route> route):
            HttpServer(ioc,port, route){

            }
        
        
        template<class R,class A1,class A2,class A3>
        void RPCBind(std::string F,std::function<R(A1,A2,A3)> bindFunction){
            route_->AddRoute(F,std::bind(&RPCServer::RPCrouteFunction<R,A1,A2,A3>,this, bindFunction,
                std::placeholders::_1,std::placeholders::_2));
        }
        template<class R,class A1,class A2>
        void RPCBind(std::string F,std::function<R(A1,A2)> bindFunction){
            route_->AddRoute(F,std::bind(&RPCServer::RPCrouteFunction<R,A1,A2>,this, bindFunction,
                std::placeholders::_1,std::placeholders::_2));
        }
        template<class R,class A1>
        void RPCBind(std::string F,std::function<R(A1)> bindFunction){
            route_->AddRoute(F,std::bind(&RPCServer::RPCrouteFunction<R,A1>,this, bindFunction,
                std::placeholders::_1,std::placeholders::_2));
        }


        
        bool RPCJsonFunction(const Net::HttpRequest& req,Net::HttpResponse& res,json& reqjson){

            if(req.method() == HttpRequest::Method::kPost){
                res.setStatusCode(HttpResponse::K200Ok);
                res.setStatusMessage("OK");
                res.setContentType("application/json");
                std::string parsebody =  req.postBody();
                reqjson = json::parse(parsebody);   
                return true;
            }
            return false;
        }

        template<class R,class A1,class A2,class A3>
        void RPCrouteFunction(std::function<R(A1,A2,A3)> bindFunction,const Net::HttpRequest& req,Net::HttpResponse& res){
            json reqjson;
            if(RPCJsonFunction(req,res,reqjson)){
                json response;
                A1 a = reqjson["args1"].get<A1>();
                A2 b = reqjson["args2"].get<A2>();
                A3 c = reqjson["args3"].get<A3>();
                response["res"] = bindFunction(a,b,c);
                res.setBody(response.dump());
            }
        }

        template<class R,class A1,class A2>
        void RPCrouteFunction(std::function<R(A1,A2)> bindFunction,const Net::HttpRequest& req,Net::HttpResponse& res){
            json reqjson;
            if(RPCJsonFunction(req,res,reqjson)){
                json response;
                A1 a = reqjson["args1"].get<A1>();
                A2 b = reqjson["args2"].get<A2>();
                response["res"] = bindFunction(a,b);
                res.setBody(response.dump());
            }
        }
        template<class R,class A1>
        void RPCrouteFunction(std::function<R(A1)> bindFunction,const Net::HttpRequest& req,Net::HttpResponse& res){
            json reqjson;
            if(RPCJsonFunction(req,res,reqjson)){
                json response;
                A1 a = reqjson["args1"].get<A1>();
                response["res"] = bindFunction(a);
                res.setBody(response.dump());
            }
        }
};



