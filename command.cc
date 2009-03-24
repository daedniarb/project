#include "connectionclosedexception.h"
#include "command.h"
#include "client.h"

#include <iostream>
#include <string>
#include <sstream>

namespace client_server {

	using namespace std;
  
    Command::Command(Client* client) : client(client)
    {
    }

    Command::~Command()
    {
    }


	bool Command::interact(istream &in) const 
	{
	
		string line;
		getline(in, line);

		if(in.eof()) {
			cout << "Exiting." << endl;
			return false;
		}

		if(line == "list")
		{
			cout << "listgroups" << endl;
			client->listGroups();
			return true;
		}
		return false;
	}

    
} // Namespace
