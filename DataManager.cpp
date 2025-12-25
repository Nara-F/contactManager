#include "DataManager.h"
#include "Person.h"

#include <algorithm>

DataManager::DataManager() {}

DataManager::~DataManager() {}

void DataManager::clear()
{
    persons.clear();
}

void DataManager::load(const std::list<Person> &persons)
{
    for (const auto &newp : persons)
    {
        auto it = std::find_if(this->persons.begin(), this->persons.end(),
                               [&](const Person &p)
                               { return p.getId() == newp.getId(); });

        if (it == this->persons.end())
        {
            this->persons.push_back(newp);
        }
        else
        {
            // 可以拓展为加入可选更新或不更新
            it->update(newp);
        }
    }
    this->persons.sort([](const Person &a, const Person &b)
                       { return a.getId() < b.getId(); });
}

std::list<Person> &DataManager::getAll()
{
    return persons;
}

const std::list<Person> &DataManager::getAll() const
{
    return persons;
}

const Person *DataManager::findByName(const std::string &name) const
{
    for (auto it = persons.cbegin(); it != persons.cend(); ++it)
    {
        if (it->getName() == name)
            return &*it;
    }
    return nullptr;
}

const Person *DataManager::findById(const char &id) const
{
    for (auto it = persons.cbegin(); it != persons.cend(); ++it)
    {
        if (it->getId() == id)
            return &*it;
    }
    return nullptr;
}

bool DataManager::existsId(char id) const
{
    for (const auto &p : persons)
    {
        if (p.getId() == id)
            return true;
    }
    return false;
}

bool DataManager::add(const Person &p)
{
    if (existsId(p.getId()))
        return false;
    persons.push_back(p);
    this->persons.sort([](const Person &a, const Person &b)
                       { return a.getId() < b.getId(); });
    return true;
}

bool DataManager::addById(char id)
{
    if (existsId(id))
        return false;
    Person p(id);
    persons.push_back(p);
    this->persons.sort([](const Person &a, const Person &b)
                       { return a.getId() < b.getId(); });
    return true;
}

bool DataManager::removeByName(const std::string &name)
{
    char id;
    auto it = persons.begin();
    for (; it != persons.end();)
    {
        if (it->getName() == name)
        {
            id = it->getId();
            it = persons.erase(it);
        }
        else
        {
            ++it;
        }
    }
    if (it == persons.end() && id == '\0')
    {
        return false; // 未找到该姓名
    }
    for (auto &p : persons)
    {
        p.removeContactMember(id);
    }
    return true;
}

bool DataManager::updateByName(const std::string &name, const Person &newInfo)
{
    char id, newId;
    for (auto it = persons.begin(); it != persons.end(); ++it)
    {
        if (it->getName() == name)
        {
            id = it->getId();
            *it = newInfo;
            newId = it->getId();
        }
    }
    for (auto &p : persons)
    {
        if (p.removeContactMember(id))
            p.addContactMember(newId);
    }
    return true;
}