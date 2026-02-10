// 文件管理类声明
#pragma once

#include <string>
#include <vector>

#include "Person.h"

class FileManager
{
public:
    FileManager();
    ~FileManager();

    std::vector<std::string> getFileList(const std::string &directory) const; // 获取目录下文件列表

    std::vector<Person<>> read(const std::string &fileName) const; // 读/写 Person
    bool write(const std::string &fileName, const std::vector<Person<>> &persons) const;
};
