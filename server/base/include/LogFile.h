#pragma once

// std lib
#include <memory>
#include <string>
// public interface
#include <FileUtil.h>
#include <MutexLock.h>
#include <noncopyable.h>

class LogFile : noncopyable {
public:
    LogFile(std::string basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char *logline, int len);
    void flush();
    bool rollFile();

private:
      void append_unlocked(const char *logline, int len);

      std::string basename_;
      const int FlushEveryN_;
      int count_;
      std::unique_ptr<MutexLock> mutex_lock_;
      std::unique_ptr<AppendFile> file_;
};
