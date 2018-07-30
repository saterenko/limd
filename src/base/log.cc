#include "log.h"

#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>


using namespace limd;

Log *Log::instance_ = NULL;

Log::Log(const std::string& file): file_(file), currentTime_(0), fileSize_(0)
{
    initConfig();
    /**/
    pidStr_ = "(" + std::to_string(getpid()) + ")";
    fd_ = fopen(file_.c_str(), "a");
    if (!fd_) {
        throw LogException("can't fopen " + file_ + " for append");
    }
}

Log::~Log()
{
    fclose(fd_);
}

void
Log::initConfig()
{
    /*  default  */
    level_ = LOG_ERROR;
    maxFileSize_ = 0;
    maxFilesCount_ = 0;
    maxLineSize_ = 0;
    // /*  parse  */
    // if (!config.HasMember("log")) {
    //     return;
    // }
    // if (!config["log"].IsObject()) {
    //     throw ConfigException("section log must be an object");
    // }
    // const auto& log = config["log"]; 
    // if (log.HasMember("file")) {
    //     if (!log["file"].IsString()) {
    //         throw ConfigException("log.file must be a string");
    //     }
    //     file_ = log["file"].GetString();
    // }
    // if (log.HasMember("level")) {
    //     if (!log["level"].IsString()) {
    //         throw ConfigException("log.level must be a string");
    //     }
    //     std::string levelName =  log["level"].GetString();
    //     if (levelName == "fatal") {
    //         level_ = LOG_FATAL;
    //     } else if (levelName == "error") {
    //         level_ = LOG_ERROR;
    //     } else if (levelName == "warn") {
    //         level_ = LOG_WARN;
    //     } else if (levelName == "info") {
    //         level_ = LOG_INFO;
    //     } else if (levelName == "debug") {
    //         level_ = LOG_DEBUG;
    //     } else {
    //         throw ConfigException("log.level must be one of \"fatal\", \"error\", \"warn\", \"info\", \"debug\"");
    //     }
    // }
    // if (log.HasMember("max_file_size")) {
    //     if (log["max_file_size"].IsUint()) {
    //         maxFileSize_ = log["max_file_size"].GetUint();
    //     } else if (log["max_file_size"].IsString()) {
    //         maxFileSize_ = Utils::atoz(log["max_file_size"].GetString());
    //     } else {
    //         throw ConfigException("log.max_file_size must be an unsigned int or string");
    //     }
    //     if (maxFileSize_ && maxFileSize_ < 1024) {
    //         throw ConfigException("log.max_file_size must be at least 1024");
    //     }
    // }
    // if (log.HasMember("files_count")) {
    //     if (!log["files_count"].IsUint()) {
    //         throw ConfigException("log.files_count must be an unsigned int");
    //     }
    //     maxFilesCount_ = log["files_count"].GetUint();
    //     if (maxFilesCount_ && maxFilesCount_ < 2) {
    //         throw ConfigException("log.files_count must be at least 2");
    //     }
    // }
    // if (maxFileSize_ && !maxFilesCount_) {
    //     throw ConfigException("can't use log.max_file_size without log.files_count");
    // }
    // if (log.HasMember("max_line_size")) {
    //     if (log["max_line_size"].IsUint()) {
    //         maxLineSize_ = log["max_line_size"].GetUint();
    //     } else if (log["max_line_size"].IsString()) {
    //         maxLineSize_ = Utils::atoz(log["max_line_size"].GetString());
    //     } else {
    //         throw ConfigException("log.max_line_size must be an unsigned int or string");
    //     }
    //     if (maxLineSize_ && maxLineSize_ < 1024) {
    //         throw ConfigException("log.max_line_size must be at least 1024");
    //     }
    // }
}

void
Log::init(const std::string& file)
{
    auto log = new Log(file);
    if (instance_) {
        delete instance_;
    }
    instance_ = log;
}

void
Log::clean()
{
    if (instance_) {
        delete instance_;
        instance_ = NULL;
    }
}

void
Log::fork()
{
    assert(instance_ != NULL);
    instance_->pidStr_ = "(" + std::to_string(getpid()) + ")";
}

void
Log::reopen()
{
    assert(instance_ != NULL);
    std::lock_guard<std::mutex> lock(instance_->writeLock_);
    FILE *newFd = fopen(instance_->file_.c_str(), "a");
    if (!newFd) {
        return;
    }
    FILE *oldFd = instance_->fd_;
    instance_->fd_ = newFd;
    fclose(oldFd);
    instance_->fileSize_ = 0;
}

void
Log::updateCurrentTimeStr(struct timeval& now)
{
    std::lock_guard<std::mutex> lock(instance_->writeLock_);
    if (now.tv_sec != currentTime_) {
        currentTime_ = now.tv_sec;
        currentTimeStr_[0] = '[';
        char *p = Log::appendLocalDatetime(currentTimeStr_ + 1, currentTime_);
        *p++ = '.';
        *p = '\0';
    }
}

void
Log::rotateLogFiles()
{
    /*  extract path and file name  */
    std::string path;
    std::string fileName;
    auto p = file_.rfind('/');
    if (p == std::string::npos) {
        path = "./";
        fileName = file_;
    } else {
        path = file_.substr(0, p + 1);
        fileName = file_.substr(p + 1);
    }
    /*  search all files to rotate  */
    for (int i = maxFilesCount_ - 2; i > 0; i--) {
        struct stat st;
        std::string oldName = path + fileName + '.' + std::to_string(i);
        if ((stat(oldName.c_str(), &st)) == 0) {
            std::string newName = path + fileName + '.' + std::to_string(i + 1);
            if (rename(oldName.c_str(), newName.c_str()) != 0) {
                std::cerr << "can't rename " << oldName << " to " << newName << std::endl;
            }
        }
    }
    /*  rename current file  */
    std::string newName = path + fileName + ".1";
    if (rename(file_.c_str(), newName.c_str()) != 0) {
        std::cerr << "can't rename " << file_ << " to " << newName << std::endl;
    }
}

void
Log::addAndCheckFileSize(size_t size)
{
    if (!maxFileSize_) {
        return;
    }
    fileSize_ += size;
    if (fileSize_ < maxFileSize_) {
        return;
    }
    /*  rotate log files  */
    rotateLogFiles();
    /*  reopen file  */
    FILE *newFd = fopen(file_.c_str(), "a");
    if (!newFd) {
        return;
    }
    FILE *oldFd = fd_;
    fd_ = newFd;
    fclose(oldFd);
    fileSize_ = 0;
}

void
Log::put(enum LogLevel level, const char *file, int line, const char *format, ...)
{
    if (!instance_) {
        /*  print to stderr  */
        const char *k = strrchr(file, '/');
        if (!k) {
            k = file;
        } else {
            k++;
        }
        fprintf(stderr, "%s:%d ", k, line);
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "\n");
        return;
    }
    static const char *logLevels[] = {"fatal", "error", "warn", "info", "debug"};
    if (instance_->level_ < level) {
        return;
    }
    /*  make time if need  */
    struct timeval now;
    gettimeofday(&now, NULL);
    instance_->updateCurrentTimeStr(now);
    /*  write line begining  */
    char buf[lineSize_];
    char *p = buf;
    char *begin = p;
    char *end = p + lineSize_;
    if (file) {
        const char *k = strrchr(file, '/');
        if (!k) {
            k = file;
        } else {
            k++;
        }
        int rc = snprintf(p, lineSize_, "%s%06u] %s %s in %s:%d ", instance_->currentTimeStr_,
            (unsigned int) now.tv_usec, instance_->pidStr_.c_str(), logLevels[level], k, line);
        p += rc;
    } else {
        int rc = snprintf(p, lineSize_, "%s%06u] %s %s ", instance_->currentTimeStr_,
            (unsigned int) now.tv_usec, instance_->pidStr_.c_str(), logLevels[level]);
        p += rc;
    }
    /*  write message  */
    va_list args;
    while (1) {
        va_start(args, format);
        int rc = vsnprintf(p, end - p, format, args);
        va_end(args);
        if (rc < 0) {
            if (begin != buf) {
                free(begin);
            }
            return;
        }
        if (instance_->maxLineSize_ && end - begin >= instance_->maxLineSize_) {
            p = begin + instance_->maxLineSize_ - 1;
            break;
        }
        if (rc >= end - p) {
            size_t size = instance_->lineSize_;
            size_t need = rc + p - begin;
            while (size <= need) { \
                size = (size << 1) - (size >> 1);
            }
            char *nb = (char *) malloc(size);
            if (!nb) {
                if (begin != buf) {
                    free(begin);
                }
                return;
            }
            memcpy(nb, begin, p - begin);
            p = nb + (p - begin);
            end = nb + size;
            if (begin != buf) {
                free(begin);
            }
            begin = nb;
            continue;
        }
        p += rc;
        break;
    }
    *p++ = '\n';
    size_t size = p - begin;
    {
        std::lock_guard<std::mutex> lock(instance_->writeLock_);
        fwrite(begin, 1, size, instance_->fd_);
        fflush(instance_->fd_);
        /*  check size  */
        instance_->addAndCheckFileSize(size);
    }
    /**/
    if (begin != buf) {
        free(begin);
    }
}

void
Log::setLevel(enum LogLevel level)
{
    assert(instance_ != NULL);
    instance_->level_ = level;    
}

std::string
Log::getFile()
{
    assert(instance_ != NULL);
    return instance_->file_;
}

int
Log::getLevel()
{
    assert(instance_ != NULL);
    return instance_->level_;
}

size_t
Log::getMaxFileSize()
{
    assert(instance_ != NULL);
    return instance_->maxFileSize_;
}

int
Log::getMaxFilesCount()
{
    assert(instance_ != NULL);
    return instance_->maxFilesCount_;
}

int
Log::getMaxLineSize()
{
    assert(instance_ != NULL);
    return instance_->maxLineSize_;
}

char *
Log::appendLocalDatetime(char *s, time_t t)
{
    /* format: yyyy-mm-dd hh:ii:ss  */
    static const char digits[201] = {
        "00010203040506070809"
        "10111213141516171819"
        "20212223242526272829"
        "30313233343536373839"
        "40414243444546474849"
        "50515253545556575859"
        "60616263646566676869"
        "70717273747576777879"
        "80818283848586878889"
        "90919293949596979899"
    };
    struct tm tm;
    localtime_r(&t, &tm);
    /*  year  */
    int n = tm.tm_year + 1900;
    int d1 = (n / 100) << 1;
    int d2 = (n % 100) << 1;
    *s++ = digits[d1];
    *s++ = digits[d1 + 1];
    *s++ = digits[d2];
    *s++ = digits[d2 + 1];
    *s++ = '-';
    /*  month  */
    d1 = (tm.tm_mon + 1) << 1;
    *s++ = digits[d1];
    *s++ = digits[d1 + 1];
    *s++ = '-';
    /*  day  */
    d1 = tm.tm_mday << 1;
    *s++ = digits[d1];
    *s++ = digits[d1 + 1];
    *s++ = ' ';
    /*  hour  */
    d1 = tm.tm_hour << 1;
    *s++ = digits[d1];
    *s++ = digits[d1 + 1];
    *s++ = ':';
    /*  minute  */
    d1 = tm.tm_min << 1;
    *s++ = digits[d1];
    *s++ = digits[d1 + 1];
    *s++ = ':';
    /*  secunde  */
    d1 = tm.tm_sec << 1;
    *s++ = digits[d1];
    *s++ = digits[d1 + 1];
    return s;
}
