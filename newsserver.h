#ifndef NEWSSERVER_H
#define NEWSSERVER_H

#include <vector>
#include "connection.h"
#include "server.h"
#include "databaseinterface.h"

namespace client_server {

    /* A server listens to a port and handles multiple connections */
    class NewsServer{

    public:
        // Create a newsserver that listens to the port 'port'.
	// If 'disk' is false an in-memory version of the database will be used, and any
	// data will be lost when the server stops. Otherwise the data will be written to
	// disk in the current directory or at the specified location.
	NewsServer(int port, bool disk = false, const std::string& directory = "");

        // Create a newsserver that listens to the port 'port' and uses a database stored
	// on disk in 'directory'.
        NewsServer(int port, const std::string& directory);
        
        // Destructor
         ~NewsServer();
        
	// The main interaction loop
	void mainLoop();

	void parseMessage(Connection* conn);
        
    private:
	static const std::string protocolError;

	void disconnect(Connection* conn, const std::string& error_message);

	Server server;

	DatabaseInterface* database;

    };
}

#endif
