#include <cstdlib>

#include "base/log.h"
#include "server.h"


using namespace limd;


int
main(int argc, char **argv)
{
    Log::init("limd.log");
    Server server;
    if (!server.start("localhost:13001")) {
        logFatal("can't start the server");
        exit(1);
    }
    exit(0);
}
