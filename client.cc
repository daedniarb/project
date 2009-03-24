#include "client.h"
#include <iostream> 

namespace client_server {

    using namespace protocol;
    using namespace std;

    Client::Client()
    {		
    }

    Client::Client(std::string& hostname, int port)
    {
	if(!open(hostname, port))
	{
	    cout<< "Could not connect to this port." << endl;
	    exit(1);
	}
    }

    bool Client::open(const string& hostname, const int port)
    {
        if(connection == 0 && handler == 0) {
            connection = new Connection(hostname.c_str(), port);
            handler = new MessageHandler(connection);
        }
        if(!connection->isConnected()) {
            close();
            return false;
        }
        return true;
    }

    Client::~Client()
    {
        close();
    }

    void Client::close()
    {
        delete connection;
        connection = 0;
        delete handler;
        handler = 0;
    }

    bool Client::isConnected() const
    {
        return (connection == 0) ? false : connection->isConnected();
    }

    void Client::response() const
    {
	// Receive answer code - i.e. which command the result 
	// is about
	int command = handler->recvCode();
	//... do something here perhaps, I don't know


	// Depending on what command is, parse the resulting message

	if(command == Protocol::ANS_LIST_NG)
	{
	    // Do something
	    cout<< "List of newsgroups should be printed now." << endl;
	}

	// All the others are on the form ANS_ACK/ANS_NAK parameters

	// So something like the code below, or switch the if and switch statements around perhaps, not sure which is best.
	
	// Receive  ANS_ACK or ANS_NAK
	int ans = handler->recvCode();


	if(ans == Protocol::ANS_NAK)
	{
	    // Receive and print error message
	    // "Newsgroups could not be listed because ..." or something
	    switch(command){
	    case Protocol::ANS_LIST_ART :
		cout<< "Articles could not be listed because a horde of rhinoceros overran the database." << endl;
		
		break;
	    default:
		break;
		
	    }
	}
	else if(ans == Protocol::ANS_ACK)
	{
	    switch(command){
	    case Protocol::ANS_LIST_ART :
		cout<< "List of articles should be printed now." << endl;
		break;

	    default:
		break;
	    
	    }
	}
	else
	{
	    // Panic? Shouldn't happen.
	}
	
	// Check that the last one is indeed Protocol::ANS_END:
	ans = handler->recvCode();
	


//         do {
//             b = handler->recvInt();
//         } while(b != Protocol::ANS_END);

//         cout << endl;

    }


    void Client::listGroups() const throw(ConnectionClosedException)
    {
        if(!isConnected()) { throw ConnectionClosedException(); }

        handler->sendCode(Protocol::COM_LIST_NG);
        handler->sendCode(Protocol::COM_END);

        response();
    }

    void Client::createGroup(const std::string& name) const throw(ConnectionClosedException)
    {
        if(!isConnected()) { throw ConnectionClosedException(); }

        handler->sendCode(Protocol::COM_CREATE_NG);
        handler->sendStringParameter(name);
        handler->sendCode(Protocol::COM_END);

        response();
    }

    void Client::deleteGroup(const int groupId) const throw(ConnectionClosedException)
    {
        if(!isConnected()) { throw ConnectionClosedException(); }

        handler->sendCode(Protocol::COM_DELETE_NG);
        handler->sendInt(groupId);
        handler->sendCode(Protocol::COM_END);

        response();
    }

    void Client::listArticles(const int groupId) const throw(ConnectionClosedException)
    {
        if(!isConnected()) { throw ConnectionClosedException(); }

        handler->sendCode(Protocol::COM_LIST_ART);
        handler->sendInt(groupId);
        handler->sendCode(Protocol::COM_END);

        response();
    }

    void Client::createArticle(const int groupId, const std::string& title, const std::string& author, const std::string& text) const throw(ConnectionClosedException)
    {
        if(!isConnected()) { throw ConnectionClosedException(); }

        handler->sendCode(Protocol::COM_CREATE_ART);
        handler->sendInt(groupId);
        handler->sendStringParameter(title);
        handler->sendStringParameter(author);
        handler->sendStringParameter(text);
        handler->sendCode(Protocol::COM_END);

        response();
    }

    void Client::deleteArticle(const int groupId, const int articleId) const throw(ConnectionClosedException)
    {
        if(!isConnected()) { throw ConnectionClosedException(); }

        handler->sendCode(Protocol::COM_DELETE_ART);
        handler->sendInt(groupId);
        handler->sendInt(articleId);
        handler->sendCode(Protocol::COM_END);

        response();
    }

    void Client::displayArticle(const int groupId, const int articleId) const throw(ConnectionClosedException)
    {
        if(!isConnected()) { throw ConnectionClosedException(); }

        handler->sendCode(Protocol::COM_GET_ART);
        handler->sendInt(groupId);
        handler->sendInt(articleId);
        handler->sendCode(Protocol::COM_END);

        response();
    }


}

