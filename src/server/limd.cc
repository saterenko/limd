#include <cstdlib>

#include "base/log.h"
#include "server.h"


using namespace limd;


int
main(int argc, char **argv)
{
    Log::init("limd.log");
    try {
        Server server("localhost:13001");
        server.start();
    } catch (const std::runtime_error& e) {
        logFatal(e.what());
        exit(1);
    }
    exit(0);
}
