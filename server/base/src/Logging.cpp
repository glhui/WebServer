//
// Created by 19948 on 2025/4/21.
//
#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

static std::once_flag  onceFlag;
static std::unique_ptr<AsyncLogging> AsyncLogger_;

std::string Logger::logFileName_ = "./WebServer.log";

void once_init()
{
    AsyncLogger_.reset(new AsyncLogging(Logger::getLogFileName()));
    AsyncLogger_->start();
}

void output(const char* msg, int len)
{
    std::call_once(onceFlag, once_init);
    AsyncLogger_->append(msg, len);
}

Logger::Impl::Impl(const char *fileName, int line)
  : stream_(),
    line_(line),
    basename_(fileName)
{
    formatTime();
}

void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char *fileName, int line)
  : impl_(fileName, line)
{ }

Logger::~Logger()
{
    impl_.stream_ << '\n' << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}

