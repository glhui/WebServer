#pragma once

#include <string>
// public interface
#include <noncopyable.h>

class AppendFile : public noncopyable {
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();
    void append(const char *logline, const size_t len);
    void flush();
private:
    size_t write(const char *logline, size_t len);
    FILE *fp_;
    char buffer_[64 * 1024];
};