#ifndef COMMAND_H
#define COMMAND_H

#include "client.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>

namespace client_server {


    class Command
    {
	public:
        Command(Client* client);
        ~Command();


	bool interact(std::istream&) const;

	private:
        Client* client;
    };

} // Namespace

#endif
