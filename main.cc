#include <nlohmann/json.hpp>
#include "RPCServer.hpp"
using json = nlohmann::json;

void Route_1(const Net::HttpRequest& req,Net::HttpResponse& res){
    res.setStatusCode(HttpResponse::K200Ok);
    res.setStatusMessage("OK");
    res.setContentType("text/html");
    res.setBody("Hello World");
}


int main(){
    std::shared_ptr<boost::asio::io_context> io(new boost::asio::io_context());
    std::shared_ptr<Route> route(new Route());
    route->AddRoute("/",Route_1);
    RPCServer http(io,1110,route);
    http.RPCBind<int,int,int,int>("add3",[](int a,int b,int c){return a+b+c;});
    http.RPCBind<int,int,int>("add2",[](int a,int b){return a+b;});
    http.RPCBind<int,int>("add1",[](int a){return a;});
    io->run();
}