#ifndef HTTPRESPONSEHANDLER_HPP
#define HTTPRESPONSEHANDLER_HPP

#include <ctime>
#include <iostream>
#include <fcntl.h>

#include "FileController.hpp"
#include "HTTPHandler.hpp"
#include "CGISession.hpp"

#include "NginxConfig.hpp"
#include "HTMLBody.hpp"

class HTTPResponseHandler : public HTTPHandler {
    private:
        HTTPResponseHandler();
        std::string _serverIndex;
    public:
        HTTPResponseHandler(int coneectionFd);
        virtual ~HTTPResponseHandler();

        typedef enum e_Phase {FIND_RESOURCE, AUTOINDEX, CGI_RUN, CGI_REQ, GET_FILE, NOT_FOUND, DATA_SEND_LOOP, CGI_SEND_LOOP, CGI_RECV_LOOP, FINISH} Phase;
        virtual HTTPResponseHandler::Phase process(HTTPData& data);

        void responseNotFound(const HTTPData& data);
        void responseAutoIndex(const HTTPData& data);

        int getCGIfd(void);
    private:
        std::string getMIME(const std::string& extension) const;
        bool isCGI(std::string& URI);
        std::string getServerIndex(NginxConfig::ServerBlock server);
        void setHTMLHeader(const std::string& extension, const long contentLength);

    private:
        Phase _phase;
        std::string _root;
        FileController::Type _type;

        std::string _absolutePath;
        std::string _staticHtml;
        FileController* _file;
        CGISession* _cgi;
        NginxConfig _nginxConfig;
};

#endif
