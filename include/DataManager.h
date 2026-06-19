// 数据（链表）管理类声明
#pragma once

#include <vector>
#include <string>

#include "Person.h"

class DataManager
{
public:
    DataManager();
    ~DataManager();

    void clear();
    void load(const std::vector<Person<>> &persons);

    std::vector<Person<>> &getAll();             // 可写访问
    const std::vector<Person<>> &getAll() const; // 只读访问

    const Person<> *findByName(const std::string &name) const; // 按名字查找

    const Person<> *findById(const IdType &id) const; // 按id查找

    bool existsId(const IdType &id) const;

    // 增删改
    bool add(const Person<> &p); // 尾插人员
    bool addById(const IdType &id); // 尾插仅出现id的人员
    bool removeByName(const std::string &name);
    bool updateByName(const std::string &name, const Person<> &newInfo);

private:
    std::vector<Person<>> persons;
};
