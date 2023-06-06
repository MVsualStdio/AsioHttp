#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <algorithm>

namespace Net{
    class HttpContext{
        public:
            enum HttpRequestParseState{
                kExpectRequestLine,
                kExpectHeaders,
                kExpectBody,
                kGotAll,
            };
        HttpContext(): state_(kExpectRequestLine){}


        bool parseRequest(const std::string& buf);
        bool parseResponse(const std::string& buf);
        bool gotAll() const{ 
            return state_ == kGotAll;
        }

        void reset(){
            state_ = kExpectRequestLine;
            HttpRequest dummy;
            request_.swap(dummy);
        }

        const HttpRequest& request() const{ 
            return request_; 
        }

        const HttpRequestParseState state() const{ 
            return state_; 
        }

        HttpRequest& request(){ 
            return request_; 
        }
        
        private:
            bool processRequestLine(const char* begin, const char* end);
            HttpRequestParseState state_;
            HttpRequest request_;
            int body_lenth;
    };
}

using namespace Net;

bool HttpContext::processRequestLine(const char* begin, const char* end){
    bool succed = false;
    const char* start = begin;
    const char* space = std::find(start,end,' ');
   
    if(space!=end && request_.setMethod(start,space)){
        start = space+1;
        space = std::find(start,end,' ');
        if(space!=end){
            const char* question = std::find(start, space, '?');
            if(question!=space){
                request_.setPath(start,question);
                request_.setQuery(question,space);
            }
            else{
                request_.setPath(start,space);
            }
            start = space+1;
            
            succed = (end-start == 8) && std::equal(start, end-1, "HTTP/1.");
            if(succed){
                if(*(end-1) == '1'){
                    request_.setVersion(HttpRequest::Version::kHttp11);
                }
                else if((*end-1) == '0'){
                    request_.setVersion(HttpRequest::Version::kHttp10);
                }
                else{
                    succed = false;
                }
            }
        }
    }
    return succed;
}


bool HttpContext::parseRequest(const std::string& buf){
    bool ok = true;
    bool hasMore = true;
    std::size_t pos = 0;
    // std::cout<<"peeeek:"<<std::string(buf->peek(),buf->end())<<std::endl;
    // std::cout<<"peeeekend"<<std::endl;
    while(hasMore){
        if(pos >= buf.length()){
            break;
        }
        if(state_ == kExpectRequestLine){
            std::size_t posend =  buf.find("\r\n",pos);
            if(posend < buf.length()){
                //std::cout<<std::string(buf.begin()+pos,buf.begin()+posend)<<std::endl;
                ok = processRequestLine(buf.c_str()+pos,buf.c_str()+posend);
                
                if(ok){
                    pos = posend + 2;
                    state_ = kExpectHeaders;
                }
                else{
                    hasMore = false;
                }
            }
            else{
                hasMore = false;
            }
        }
        else if(state_ == kExpectHeaders){
            std::size_t posend =  buf.find("\r\n",pos);

            if(posend < buf.length()){
                //std::cout<<std::string(buf->peek(),crlf)<<std::endl;
                auto colon = buf.find(':',pos);

                if(colon < posend){
                    request_.addHeader(buf.c_str()+pos,buf.c_str()+colon,buf.c_str()+posend);
                }
                else{
                    if(request_.getHeader("Content-Length").size() == 0){
                        state_ = kGotAll;
                        hasMore = false;
                    } 
                    else{
                        body_lenth = stoi(request_.getHeader("Content-Length"));
                        state_ = kExpectBody;
                    }
                    
                }
                pos = posend + 2;
            }
            else{
                hasMore = false;
            }
        }

        else if(state_ == kExpectBody){
            //std::cout<<buf->getUnreadSize()<<std::endl;
            if(buf.length() - pos >= static_cast<size_t>(body_lenth)){
                //buf->retrieve(2);// /r/n
                
                const char* begin = buf.c_str() + pos; 
                
                const char* end = begin + static_cast<size_t>(body_lenth);
                
                request_.setPostBody(begin,end);
                state_ = kGotAll;
                //hasMore = false;
            }
            hasMore = false;
        }
    }
    return ok;
}

bool HttpContext::parseResponse(const std::string& buf){
    bool ok = true;
    bool hasMore = true;
    int pos = 0;
    // std::cout<<"peeeek:"<<std::string(buf->peek(),buf->end())<<std::endl;
    // std::cout<<"peeeekend"<<std::endl;
    while(hasMore){
        if(pos >= buf.length()){
            break;
        }
        if(state_ == kExpectRequestLine){
            std::size_t posend =  buf.find("\r\n",pos);
            if(posend < buf.length()){
                //std::cout<<std::string(buf.begin()+pos,buf.begin()+posend)<<std::endl;
                ok = (std::string(buf.begin()+pos,buf.begin()+posend) == "HTTP/1.1 200 OK"); 
                if(ok){
                    pos = posend + 2;
                    state_ = kExpectHeaders;
                }
                else{
                    hasMore = false;
                }
            }
            else{
                hasMore = false;
            }
        }
        else if(state_ == kExpectHeaders){
            std::size_t posend =  buf.find("\r\n",pos);
    
            if(posend < buf.length()){
                //std::cout<<std::string(buf->peek(),crlf)<<std::endl;
                auto colon = buf.find(':',pos);

                if(colon < posend){
                    request_.addHeader(buf.c_str()+pos,buf.c_str()+colon,buf.c_str()+posend);
                }
                else{
                    if(request_.getHeader("Content-Length").size() == 0){
                        state_ = kGotAll;
                        hasMore = false;
                    } 
                    else{
                        body_lenth = stoi(request_.getHeader("Content-Length"));
                        state_ = kExpectBody;
                    }
                    
                }
                pos = posend + 2;
            }
            else{
                hasMore = false;
            }
        }

        else if(state_ == kExpectBody){
            //std::cout<<buf->getUnreadSize()<<std::endl;
            if(buf.length() - pos >= static_cast<size_t>(body_lenth)){
                //buf->retrieve(2);// /r/n
                const char* begin = buf.c_str() + pos; 
                const char* end = begin + static_cast<size_t>(body_lenth);
                request_.setPostBody(begin,end);
                state_ = kGotAll;
                //hasMore = false;
            }
            hasMore = false;
        }
    }
    return ok;
}

#endif