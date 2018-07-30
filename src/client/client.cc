#include "client.h"


using namespace limd;

Client::Client()
{

}

Client::~Client()
{

}

bool
Client::connect(const std::string& uri)
{
    return true;
}

uint64_t
Client::limit(const std::string& key)
{
    return 0;
}

bool
Client::setLimit(const std::string& key, int64_t value)
{

    return true;
}

bool
Client::set(const std::string& key, int64_t value)
{

    return true;
}

uint64_t
Client::hold(const std::string& key, int64_t value, uint32_t ttl)
{

    return 0;
}

bool
Client::commit(uint64_t id, int64_t value)
{

    return true;
}

bool
Client::release(uint64_t id)
{

    return true;
}

bool
Client::inc(const std::string& key, int64_t by)
{

    return true;
}

bool
Client::dec(const std::string& key, int64_t by)
{

    return true;
}

