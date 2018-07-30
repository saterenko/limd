#pragma once

#include <string>

namespace limd {

class Server
{
public:

    Server();
    ~Server();

    bool start(const std::string& uri);

};

}
