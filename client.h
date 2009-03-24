#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "connection.h"
#include "messagehandler.h"

namespace client_server {

    class Client
    {
    public:
	Client();
	Client(std::string& hostname, int port);
	~Client();

	bool open(const std::string& host, const int port);
	void close();
	bool isConnected() const;
	
	void listGroups() const
	    throw(ConnectionClosedException);
	void createGroup(const std::string& name) const
	    throw(ConnectionClosedException);
	void deleteGroup(const int groupId) const
	    throw(ConnectionClosedException);
	
	void listArticles(const int groupId) const
	    throw(ConnectionClosedException);
	void createArticle(const int groupId, const std::string& title, const std::string& author, const std::string& text) const
	    throw(ConnectionClosedException);
	void deleteArticle(const int groupId, const int articleId) const
	    throw(ConnectionClosedException);
	void displayArticle(const int groupId, const int articleId) const
	    throw(ConnectionClosedException);
	
    private:
	void response() const;
	Connection* connection;
	MessageHandler* handler;
	
    };
    
}
#endif
