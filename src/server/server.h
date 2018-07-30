#pragma once

#include <stdexcept>
#include <string>

#include <ev.h>


namespace limd {


class ServerException: public std::runtime_error
{
public:
    explicit ServerException(const std::string& what) : std::runtime_error(what) {};
    explicit ServerException(const char *what) : std::runtime_error(what) {};
};


class Server
{
public:

    Server(const std::string& uri);
    ~Server();

    void start();
    void stop();

private:

    void daemonize();
    void pidSave();
    void pidDelete();
    void initSignals();
    static void onSignalUsr1(struct ev_loop *loop, ev_signal *w, int revents);
    static void onSignalUsr2(struct ev_loop *loop, ev_signal *w, int revents);
    static void onSignalHup(struct ev_loop *loop, ev_signal *w, int revents);
    static void onSignalTerm(struct ev_loop *loop, ev_signal *w, int revents);


    const std::string pidFile_ = "limd.pid";

    struct ev_loop *loop_;
    ev_signal evSigHup_;
    ev_signal evSigUsr1_;
    ev_signal evSigUsr2_;
    ev_signal evSigInt_;
    ev_signal evSigTerm_;

};

}
