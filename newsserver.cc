#include "newsserver.h"
#include "messagehandler.h"
#include "database.h"
#include "diskdatabase.h"
#include <cstdlib>
#include <string>
#include <iostream>

using namespace std;

namespace client_server {

    const std::string NewsServer::protocolError = "Client did not follow the protocol.";

    // Create a newsserver that listens to the port 'port'.
    // If 'disk' is false an in-memory version of the database will be used, and any
    // data will be lost when the server stops. Otherwise the data will be written to
    // disk in the current directory or at the specified location.
    NewsServer::NewsServer(int port, bool disk, const std::string& directory)
    	: server(Server(port))
    {
	if(disk)
	{
	    if(directory.size() > 0)
		database = new DiskDatabase(directory.c_str());
	    else
		database = new DiskDatabase();
	}
	else
	    database = new Database();
    }
        
    // Create a newsserver that listens to the port 'port' and uses a database stored
    // on disk in 'directory'.
    NewsServer::NewsServer(int port, const std::string& directory)
    	: server(Server(port))
    {
	if(directory.size() > 0)
	    database = new DiskDatabase(directory.c_str());
	else
	    database = new DiskDatabase();
    }

    // Destructor
    NewsServer::~NewsServer()
    {
	delete database;
    }     
        
    // The main interaction loop
    void NewsServer::mainLoop()
    {
	if (! server.isReady()) {
	    cerr << "Server initialization error" << endl;
	    exit(1);
	}
    
	while (true)
	{
	    Connection* conn = server.waitForActivity();
	    if (conn != 0) {
		try {
		    parseMessage(conn);

		}
		catch (ConnectionClosedException&) {
		    server.deregisterConnection(conn);
		    delete conn;
		    cout << "Client closed connection" << endl;
		}
	    }
	    else {
		server.registerConnection(new Connection);
		cout << "New client connects" << endl;
	    }
	}	
    }

    void NewsServer::disconnect(Connection* conn, const std::string& error_message)
    {
	server.deregisterConnection(conn);
	delete conn;
	cout << error_message << endl; //"Client closed connection" << endl;
    }


    void NewsServer::parseMessage(Connection* conn)
    {
	MessageHandler comm(conn);
	try{
	    int command = comm.recvCode();
	    
	    switch(command){

	    case Protocol::COM_LIST_NG    :{           // list newsgroups
		if( comm.recvCode() != Protocol::COM_END)
		    disconnect(conn, protocolError);
		
		comm.sendCode(Protocol::ANS_LIST_NG);
		
		vector<Newsgroup> groups = database->listNewsgroups();
		comm.sendIntParameter(groups.size());
		for(vector<Newsgroup>::iterator it = groups.begin(); it != groups.end(); ++it)
		{
		    comm.sendIntParameter(it->getID());
		    comm.sendStringParameter(it->getName());
		}
		comm.sendCode(Protocol::ANS_END);
		break;
	    }
		
	    case Protocol::COM_CREATE_NG  :{           // create newsgroup
		string name = comm.recvStringParameter();
		if( comm.recvCode() != Protocol::COM_END)
		    disconnect(conn, protocolError);

		comm.sendCode(Protocol::ANS_CREATE_NG);
		database->createNewsgroup(name);
		comm.sendCode(Protocol::ANS_ACK);
		comm.sendCode(Protocol::ANS_END);
		break;
	    }	
		
	    case Protocol::COM_DELETE_NG  :{           // delete newsgroup
		int ID = comm.recvIntParameter();
		if( comm.recvCode() != Protocol::COM_END)
		    disconnect(conn, protocolError);
		
		comm.sendCode(Protocol::ANS_DELETE_NG);
		database->deleteNewsgroup(ID);
		comm.sendCode(Protocol::ANS_ACK);
		comm.sendCode(Protocol::ANS_END);
		break;
	    }

	    case Protocol::COM_LIST_ART   :{           // list articles
		int newsgroupID = comm.recvIntParameter();
		if( comm.recvCode() != Protocol::COM_END)
		    disconnect(conn, protocolError);
		comm.sendCode(Protocol::ANS_LIST_ART);
		
		vector<Article> articles = database->listArticles(newsgroupID);
		comm.sendCode(Protocol::ANS_ACK);
		comm.sendIntParameter(articles.size());
		for(vector<Article>::iterator it = articles.begin(); it != articles.end(); ++it)
		{
		    comm.sendIntParameter(it->getID());
		    comm.sendStringParameter(it->getTitle());
		}
		comm.sendCode(Protocol::ANS_END);
		break;
	    }

	    case Protocol::COM_CREATE_ART :{           // create article
		int newsgroupID = comm.recvIntParameter(); 
		string title = comm.recvStringParameter();
		string author = comm.recvStringParameter();
		string text = comm.recvStringParameter();
		if( comm.recvCode() != Protocol::COM_END)
		    disconnect(conn, protocolError);
		comm.sendCode(Protocol::ANS_CREATE_ART);

		database->addArticle(newsgroupID, title, author, text);
		comm.sendCode(Protocol::ANS_ACK);
		comm.sendCode(Protocol::ANS_END);
		break;
	    }

	    case Protocol::COM_DELETE_ART :{           // delete article
		int newsgroupID = comm.recvIntParameter(); 
		int articleID = comm.recvIntParameter(); 
		if( comm.recvCode() != Protocol::COM_END)
		    disconnect(conn, protocolError);
	
		comm.sendCode(Protocol::ANS_DELETE_ART);
		database->deleteArticle(newsgroupID, articleID);
		comm.sendCode(Protocol::ANS_ACK);
		comm.sendCode(Protocol::ANS_END);
		break;
	    }

	    case Protocol::COM_GET_ART    :{           // get article
		int newsgroupID = comm.recvIntParameter(); 
		int articleID = comm.recvIntParameter(); 
		if( comm.recvCode() != Protocol::COM_END)
		    disconnect(conn, protocolError);

		comm.sendCode(Protocol::ANS_GET_ART);		
		Article article = database->getArticle(newsgroupID, articleID);
		comm.sendCode(Protocol::ANS_ACK);
		comm.sendStringParameter(article.getTitle());
		comm.sendStringParameter(article.getAuthor());
		comm.sendStringParameter(article.getText());
		comm.sendCode(Protocol::ANS_END);
		break;
	    }
	    default:
		disconnect(conn, protocolError);;
	    }
	}
	catch(NoNewsgroupException){
	    comm.sendCode(Protocol::ANS_NAK);
	    comm.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	    comm.sendCode(Protocol::ANS_END);
	}
	catch(NoArticleException){
	    comm.sendCode(Protocol::ANS_NAK);
	    comm.sendCode(Protocol::ERR_ART_DOES_NOT_EXIST);
	    comm.sendCode(Protocol::ANS_END);
	}
	catch(ProtocolViolationException){
	    disconnect(conn, protocolError);
	}
    }




        
} // End namespace
