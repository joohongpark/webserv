#include "ConnectionSocket.hpp"

#include <iostream>


ConnectionSocket::ConnectionSocket(int listeningSocketFd, const NginxConfig::ServerBlock& serverConf, const NginxConfig& nginxConf) : Socket(-1, serverConf), _nginxConf(nginxConf) {
    struct sockaddr_in myAddr;
    this->_socket = accept(listeningSocketFd, (struct sockaddr *) &this->_socketAddr, &this->_socketLen);
    if (this->_socket == -1) {
        throw ErrorHandler("Error: connection socket error.", ErrorHandler::ALERT, "ConnectionSocket::ConnectionSocket");
    }
    if (getsockname(this->_socket, (struct sockaddr *) &myAddr, &this->_socketLen) == -1) {
        throw ErrorHandler("Error: getsockname error.", ErrorHandler::ALERT, "ConnectionSocket::ConnectionSocket");
    }
    setConnectionData(_socketAddr, myAddr);
    _req = new HTTPRequestHandler(_socket, _serverConf, _nginxConf);
    _res = NULL;
    _dynamicBufferSize = 0;
}

ConnectionSocket::~ConnectionSocket(){
    delete _req;
    delete _res;
}

HTTPRequestHandler::Phase ConnectionSocket::HTTPRequestProcess(void) {
    HTTPRequestHandler::Phase phase;
    try {
        phase = _req->process(_data);
        if (phase == HTTPRequestHandler::FINISH) {
            _res = new HTTPResponseHandler(_socket, _serverConf, _nginxConf);
        }
    } catch (const std::exception &error) {
        std::cout << error.what() << std::endl;
        _data._statusCode = 400; // Bad Request
        // TODO: response에서 StatusCode를 인식해서 동작하게 해야 함.
        // TODO: 07.06 오전: 얘는 어디서 사용 되나요?====================================
        phase = HTTPRequestHandler::FINISH;
    }
    return (phase);
}

void ConnectionSocket::setConnectionData(struct sockaddr_in _serverSocketAddr, struct sockaddr_in _clientSocketAddr) {
    std::stringstream portString;
    _data._clientIP = std::string(inet_ntoa(_serverSocketAddr.sin_addr));
    portString << ntohs(_serverSocketAddr.sin_port);
    _data._clientPort = std::string(portString.str());
    _data._hostIP = std::string(inet_ntoa(_clientSocketAddr.sin_addr));
    portString.clear();
    portString << ntohs(_clientSocketAddr.sin_port);
    _data._hostPort = std::string(portString.str());
}

HTTPResponseHandler::Phase ConnectionSocket::HTTPResponseProcess(void) {
    HTTPResponseHandler::Phase phase;
    phase = _res->process(_data);
    return (phase);
}

int ConnectionSocket::runSocket() {
    return (0);
}

int ConnectionSocket::getCGIfd(void) {
    return (_res->getCGIfd());
}

void ConnectionSocket::ConnectionSocketKiller(void* connectionsocket) {
    delete reinterpret_cast<ConnectionSocket*>(connectionsocket);
}

long ConnectionSocket::getDynamicBufferSize(void) {
    return (_dynamicBufferSize);
}
void ConnectionSocket::setDynamicBufferSize(long dynamicBufferSize) {
    _dynamicBufferSize = dynamicBufferSize;
}
