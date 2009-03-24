
#include "command.h"
#include "client.h"
#include "protocol.h"

#include <iostream>

using namespace std;

using namespace client_server;

/*
void ListNewsGroup()
{
    int amount = handler.readNumber();
    int id;
    string group;

    for(int i=0 ; i<amount ; ++i) {
        id = handler.readNumber();
        group = handler.readString();
        cout << id << " : " << group << endl;
    }

	//ANS_END om inte error
}*/




int main () {
    cout << "news> ";
    
    string hostname = "localhost";
    int	port = 7348;
    
    Client *client = new Client(hostname, port);
    Command *cmd = new Command(client);
    
    while(cin.good() && cmd->interact(cin));

    return 0;
}



