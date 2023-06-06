#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <map>
#include <string>
#include <memory>
#include <list>

namespace Net{
    class HttpResponse{
        public:
            enum HttpStatusCode{
                KUnknow,
                K200Ok = 200,
                K404NotFound = 404,
                K302Found = 302
            };
        private:
            bool closeConnection_;
            std::map<std::string,std::string> header;
            std::string body;
            std::string statusMessage;
            HttpStatusCode statusCode;
        public:
            explicit HttpResponse(bool close):closeConnection_(close),statusCode(HttpStatusCode::KUnknow){};
            void addHeader(const std::string& key,const std::string& value)
                {header[key]=value;}
            void setBody(const std::string& _body)
                {body=_body;}
            void ToString(std::string& bufferHead);
            void setContentType(const std::string& type)
                {addHeader("Content-Type", type);}
            void setStatusCode(HttpStatusCode code){statusCode = code;}
            void setStatusMessage(const std::string& message){statusMessage = message;};
            void setCloseConnect(bool on){closeConnection_ = on;}
            bool closeConnection(){return closeConnection_;}
    };
}

void Net::HttpResponse::ToString(std::string& bufferHead){
    char buf[32];
    
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode);

    bufferHead.append(buf);
    bufferHead.append(statusMessage);
    bufferHead.append("\r\n");
    if (closeConnection_){
        bufferHead.append("Connection: close\r\n");
    }
    else {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body.size());
        bufferHead.append(buf);
        bufferHead.append("Connection: Keep-Alive\r\n");
    }
    for (auto he : header)
    {
        bufferHead.append(he.first);
        bufferHead.append(": ");
        bufferHead.append(he.second);
        bufferHead.append("\r\n");
    }
    bufferHead.append("\r\n");
    bufferHead.append(body);
}

#endif