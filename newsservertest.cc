#include "newsserver.h"
#include "databaseinterface.h"
#include "diskdatabase.h"
#include "database.h"
#include "util.h"
#include <utility>
#include <vector>
#include <string>

using namespace std;
using namespace client_server;


int main(int argc, char* argv[])
{
    NewsServer skynet(7348);
    skynet.mainLoop();

    return 0;
}
