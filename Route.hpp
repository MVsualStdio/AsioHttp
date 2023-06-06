#include <unordered_map>
#include <string>
#include <functional>

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Route : public std::enable_shared_from_this<Route>{
    private:
        using RouteFun = std::function<void(const Net::HttpRequest& req,Net::HttpResponse& res)>;
        std::unordered_map<std::string,RouteFun> route_;
    public:
       
        Route(){

        }

        void AddRoute(std::string route,RouteFun F){
            route_[route] = F;
        }


        RouteFun GetFunction(std::string route){
            if(route_.find(route) == route_.end()){
                return nullptr;
            }
            
            return route_[route];
        }


        virtual void NotFound(const Net::HttpRequest& req,Net::HttpResponse& res){
            res.setStatusCode(Net::HttpResponse::K404NotFound);
            res.setStatusMessage("Not Found");
            res.setContentType("text/html");
            res.setBody("404");
        }
};