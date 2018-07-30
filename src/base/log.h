#pragma once

#include <mutex>
#include <stdexcept>
#include <string>


enum LogLevel
{
    LOG_FATAL = 0,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
};

#define logFatal(args...) limd::Log::put(LOG_FATAL, __FILE__, __LINE__, args)
#define logError(args...) limd::Log::put(LOG_ERROR, __FILE__, __LINE__, args)
#define logWarn(args...) limd::Log::put(LOG_WARN, __FILE__, __LINE__, args)
#define logInfo(args...) limd::Log::put(LOG_INFO, __FILE__, __LINE__, args)

#ifdef DEBUG_ON
    #define logDebug(args...) Log::put(LOG_DEBUG, __FILE__, __LINE__, args)
#else
    #define logDebug(args...)
#endif

namespace limd {

class LogException: public std::runtime_error
{
public:
    explicit LogException(const std::string& what) : std::runtime_error(what) {};
    explicit LogException(const char *what) : std::runtime_error(what) {};
};

class Log
{
public:
    static void init(const std::string& file);
    static void clean();
    static void fork();
    static void reopen();
    static void put(enum LogLevel level, const char *file, int line, const char *format, ...);
    static void setLevel(enum LogLevel level);
    static std::string getFile();
    static int getLevel();
    static size_t getMaxFileSize();
    static int getMaxFilesCount();
    static int getMaxLineSize();

private:
    Log(const std::string& file);
    Log(const Log&);
    Log& operator=(const Log&);
    ~Log();
    void initConfig();
    void updateCurrentTimeStr(struct timeval& now);
    void rotateLogFiles();
    void addAndCheckFileSize(size_t size);
    static char *appendLocalDatetime(char *s, time_t t);

    static Log *instance_;
    /**/
    static const int lineSize_ = 1024;
    static const int timeStrSize_ = sizeof("[YYYY-MM-DD HH:MM:SS.");
    /*  config  */
    std::string file_;
    int level_;
    size_t maxFileSize_;
    int maxFilesCount_;
    int maxLineSize_;
    /**/
    time_t currentTime_;
    char currentTimeStr_[timeStrSize_];
    std::string pidStr_;
    FILE *fd_;
    size_t fileSize_;
    std::mutex writeLock_;
};

} // end of limd namespace
