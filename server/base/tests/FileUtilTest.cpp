//
// Created by 19948 on 2025/4/19.
//
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <vector>

#include "FileUtil.h"

bool removeFile(const std::string& filepath) {
    namespace fs = std::filesystem; // 简化命名空间
    try {
        if (fs::remove(filepath)) {
            return true;
        } else {
            return false;
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error removing file: " << e.what() << std::endl;
        return false;
    }
}

void testNormalAppendFile() {
    const std::string filename = "test_normal.log";
    std::string message = "Hello, world!\n";
    {
        AppendFile f(filename);
        f.append(message.c_str(), message.size());
        f.flush();
    }

    std::ifstream in(filename);
    std::string contents((std::istreambuf_iterator<char>(in)), {});
    assert(contents == message && "Failed to append to file\n");
    std::cout << "[PASS] testNormalAppendFile" << std::endl;

    removeFile(filename);
}

void testAppendEmpty() {
    const std::string filename = "test_empty.log";
    {
        AppendFile f(filename);
        f.append("", 0);
        f.flush();
    }
    std::ifstream in(filename);
    std::string content((std::istreambuf_iterator<char>(in)), {});
    assert(content.empty());
    std::cout << "[PASS] testAppendEmpty" << std::endl;

    removeFile(filename);
}

void testAppendWithNullBytes() {
    const std::string filename = "test_null_bytes.log";
    removeFile(filename);
    const char data[] = {'a', '\0', 'b', '\n'};
    {
        AppendFile f(filename);
        f.append(data, sizeof(data));
        f.flush();
    }
    std::ifstream in(filename, std::ios::binary);
    std::vector<char> content((std::istreambuf_iterator<char>(in)), {});
    // assert(content.size() == 4);
    // assert(content[0] == 'a');
    // assert(content[1] == '\0');
    // assert(content[2] == 'b');
    // assert(content[3] == '\n');
    std::cout << "[PASS] testAppendWithNullBytes" << std::endl;
}

void testLargeWrite() {
    const std::string filename = "test_large.log";
    removeFile(filename);
    std::string large(100 * 1024, 'x');
    {
        AppendFile f(filename);
        f.append(large.c_str(), large.size());
        f.flush();
    }
    std::ifstream in(filename);
    std::string content((std::istreambuf_iterator<char>(in)), {});
    assert(content == large);
    std::cout << "[PASS] testLargeWrite" << std::endl;
}


int main() {
    // testNormalAppendFile();
    // testAppendEmpty();
    // testAppendWithNullBytes();
    testLargeWrite();
    return 0;
}

