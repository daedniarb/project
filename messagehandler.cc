#include "protocol.h"
#include "messagehandler.h"
/*
typedef std::vector<int> Logger;
private Connection conn;   // the connection
private Logger logWindow;     // the log window
*/
 
using namespace client_server;
 
MessageHandler::MessageHandler(Connection &c){
    conn = c;
}
 
void MessageHandler::setLogWindow(const Logger &log){
    logWindow = log;
}
 
void MessageHandler::sendCode(int code) const throw(client_server::ConnectionClosedException){
    sendByte(code);
    logWindow.push_back(code);
}
 
void MessageHandler::sendInt(int value) const throw(client_server::ConnectionClosedException){
    sendByte((value >> 24) & 0xFF);
    logWindow.push_back((value >> 24) & 0xFF);
    sendByte((value >> 16) & 0xFF);
    logWindow.push_back((value >> 16) & 0xFF);
    sendByte((value >> 8) & 0xFF);
    logWindow.push_back((value >> 8) & 0xFF);
    sendByte(value & 0xFF);
    logWindow.push_back(value & 0xFF);
}
 
void MessageHandler::sendIntParameter(int param) const throw(client_server::ConnectionClosedException){
    sendCode(Protocol::PAR_NUM);
    sendInt(param);
}
 
void MessageHandler::sendStringParameter(std::string param) const throw(client_server::ConnectionClosedException){
    sendCode(Protocol::PAR_STRING);
    sendInt(param.size());
    for(int i=0;i<param.size();++i){
 	 sendByte(param[i]);
  	logWindow.push_back(static_cast<int>(param[i]));
    }
}
int MessageHandler::recvCode() const throw(client_server::ConnectionClosedException){
    int code = recvByte();
    logWindow.push_back(code);
    return code;
}
 
int MessageHandler::recvInt() const throw(client_server::ConnectionClosedException){
    int b1 = recvByte();
    logWindow.push_back(b1);
    int b2 = recvByte();
    logWindow.push_back(b2);
    int b3 = recvByte();
    logWindow.push_back(b3);
    int b4 = recvByte();
    logWindow.push_back(b4);
 
    return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}
 
int MessageHandler::recvIntParameter() const throw(client_server::ConnectionClosedException){
    int code = recvCode();
    if (code != Protocol::PAR_NUM)
  	throw ProtocolViolationException();
    return recvInt();
}
 
std::string MessageHandler::recvStringParameter() const throw(client_server::ConnectionClosedException)
    int &code = recvCode();
    if (code!=protocol::PAR_STRING)
  	throw ProtocolViolationException();
    int n = recvInt();
    if (n<0)
 	 throw new ProtocolViolationException();
    std::string result = n;
    for (int i = 1; i<=n; i++) {
  	char ch = static_cast<char> conn.read();
  	result+=ch;
  	logWindow.push_back(static_cast<int>(ch));
    }
    return result;
}
 
void MessageHandler::sendByte(int code) const throw(client_server::ConnectionClosedException){
    try{
  	conn.write(static_cast<unsigned char> (code));
    }catch(ConnectionClosedException){
  	throw ConnectionClosedException()
    }
}
int MessageHandler::recvByte() const throw(client_server::ConnectionClosedException){
    code = conn.read();
    if (!conn.isConnected())
  	throw new ConnectionClosedException();
    return code;
}