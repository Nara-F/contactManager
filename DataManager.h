// 数据（链表）管理类声明
#pragma once

#include <list>
#include <string>

#include "Person.h"

class DataManager
{
public:
    DataManager();
    ~DataManager();

    void clear();
    void load(const std::list<Person<>> &persons);

    std::list<Person<>> &getAll();             // 可写访问
    const std::list<Person<>> &getAll() const; // 只读访问

    const Person<> *findByName(const std::string &name) const; // 按名字查找
    template <typename T>
    const Person<> *findById(const T &id) const // 按id查找
    {
        for (auto it = persons.cbegin(); it != persons.cend(); ++it)
        {
            if (it->getId() == id)
                return &*it;
        }
        return nullptr;
    }
    bool existsId(char id) const;

    // 增删改
    bool add(const Person<> &p); // 尾插人员
    bool addById(char id);       // 尾插仅出现id的人员
    bool removeByName(const std::string &name);
    bool updateByName(const std::string &name, const Person<> &newInfo);

private:
    std::list<Person<>> persons;
};