//
// Created by 19948 on 2025/4/14.
//

// std lib
#include <sys/stat.h>
#include <stdio.h>
// public interface
#include <FileUtil.h>

AppendFile::AppendFile(std::string filename) : fp_(fopen(filename.c_str(), "a")) {
    setvbuf(fp_, buffer_, _IOFBF, sizeof buffer_);
}

AppendFile::~AppendFile() { fclose(fp_); }

void AppendFile::append(const char* logline, const size_t len) {
  	size_t n = this->write(logline, len);
  	size_t remain = len - n;
  	while (remain > 0) {
    	size_t x = this->write(logline + n, remain);
    	if (x == 0) {
      		int err = ferror(fp_);
      		if (err) fprintf(stderr, "AppendFile::append() failed !\n");
      		break;
    	}
    n += x;
    remain = len - n;
  	}
}

void AppendFile::flush() { fflush(fp_); }

size_t AppendFile::write(const char* logline, size_t len) {
	size_t written = fwrite(logline, 1, len, fp_);
	return written;
}

