#pragma once

#include <inttypes.h>

#include <string>

#include "base/log.h"

namespace limd {

class Client
{
public:

    Client();
    ~Client();

    bool connect(const std::string& uri);

    uint64_t limit(const std::string& key);
    bool setLimit(const std::string& key, int64_t value);

    bool set(const std::string& key, int64_t value);
    uint64_t hold(const std::string& key, int64_t value, uint32_t ttl = 0);
    bool commit(uint64_t id, int64_t value = 0);
    bool release(uint64_t id);
    bool inc(const std::string& key, int64_t by = 0);
    bool dec(const std::string& key, int64_t by = 0);

private:

    bool reconnect();

};

}
