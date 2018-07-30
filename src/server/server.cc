#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "server.h"
#include "base/log.h"


using namespace limd;


Server::Server(const std::string& uri)
{
    loop_ = ev_loop_new(EVFLAG_AUTO);
    if (!loop_) {
        logFatal("can't ev_loop_new");
        throw new ServerException("can't ev_loop_new");
    }
    initSignals();
}

Server::~Server()
{
    stop();
}

void
Server::start()
{
    daemonize();
    pidSave();
    ev_run(loop_, 0);
    pidDelete();
}

void
Server::stop()
{
    if (loop_) {
        ev_break(loop_, EVBREAK_ALL);
        ev_loop_destroy(loop_);
        loop_ = nullptr;
    }
    Log::clean();
}

void
Server::daemonize()
{
    pid_t pid = fork();
    if (pid == -1) {
        logFatal("can't fork");
        throw ServerException("can't fork");
    } else if (pid) {
        exit(0);
    }
    if (setsid() == -1) {
        logFatal("can't setsid");
        throw ServerException("can't setsid");
    }
    umask(0);
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        logFatal("can't open /dev/null");
        throw ServerException("can't open /dev/null");
    }
    if (dup2(fd, STDIN_FILENO) == -1) {
        close(fd);
        logFatal("can't dup for stdin");
        throw ServerException("can't dup for stdin");
    }
    if (dup2(fd, STDOUT_FILENO) == -1) {
        close(fd);
        logFatal("can't dup for stdout");
        throw ServerException("can't dup for stdout");
    }
    if (dup2(fd, STDERR_FILENO) == -1) {
        close(fd);
        logFatal("can't dup for stderr");
        throw ServerException("can't dup for stderr");
    }
    close(fd);
}

void
Server::pidSave()
{
    FILE *fd = fopen(pidFile_.c_str(), "w");
    if (!fd) {
        logFatal("can't fopen %s for write", pidFile_.c_str());
        throw ServerException("can't fopen " + pidFile_ + " for write");
    }
    pid_t pid = getpid();
    fprintf(fd, "%ld", (long) pid);
    fclose(fd);
}

void
Server::pidDelete()
{
    unlink(pidFile_.c_str());
}

void
Server::initSignals()
{
    /*  SIGHUP  */
    ev_signal_init(&evSigHup_, Server::onSignalHup, SIGHUP);
    evSigHup_.data = (void *) this;
    ev_signal_start(loop_, &evSigHup_);
    /*  SIGUSR1  */
    ev_signal_init(&evSigUsr1_, Server::onSignalUsr1, SIGUSR1);
    evSigUsr1_.data = (void *) this;
    ev_signal_start(loop_, &evSigUsr1_);
    /*  SIGUSR2  */
    ev_signal_init(&evSigUsr2_, Server::onSignalUsr2, SIGUSR2);
    evSigUsr2_.data = (void *) this;
    ev_signal_start(loop_, &evSigUsr2_);
    /*  SIGINT  */
    ev_signal_init(&evSigInt_, Server::onSignalTerm, SIGINT);
    evSigInt_.data = (void *) this;
    ev_signal_start(loop_, &evSigInt_);
    /*  SIGTERM  */
    ev_signal_init(&evSigTerm_, Server::onSignalTerm, SIGTERM);
    evSigTerm_.data = (void *) this;
    ev_signal_start(loop_, &evSigTerm_);
}

void
Server::onSignalUsr1(struct ev_loop *loop, ev_signal *w, int revents)
{
    logDebug("signal USR1 received");
    Log::reopen();
}

void
Server::onSignalUsr2(struct ev_loop *loop, ev_signal *w, int revents)
{
    logDebug("signal USR2 received");
}

void
Server::onSignalHup(struct ev_loop *loop, ev_signal *w, int revents)
{
    logDebug("signal HUP received");
}

void
Server::onSignalTerm(struct ev_loop *loop, ev_signal *w, int revents)
{
    logDebug("signal TERM received");
    Server *server = (Server *) w->data;
    server->stop();
}

