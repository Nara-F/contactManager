#include "DataManager.h"
#include "Person.h"

#include <algorithm>

DataManager::DataManager() {}

DataManager::~DataManager() {}

void DataManager::clear()
{
    persons.clear();
}

void DataManager::load(const std::vector<Person<>> &persons)
{
    for (const auto &newp : persons)
    {
        auto it = std::find_if(this->persons.begin(), this->persons.end(),
                               [&](const Person<> &p)
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
    std::sort(this->persons.begin(), this->persons.end(),
              [](const Person<> &a, const Person<> &b)
              { return a.getId() < b.getId(); });
}

void DataManager::loadRelationships(const std::vector<std::pair<IdType, IdType>> &relationships)
{
    for (const auto &[ownerId, contactId] : relationships)
    {
        if (!existsId(ownerId))
        {
            addById(ownerId);
        }
        if (!existsId(contactId))
        {
            addById(contactId);
        }

        for (auto &person : persons)
        {
            if (person.getId() == ownerId)
            {
                person.addContactMember(contactId);
                break;
            }
        }
    }
}

std::vector<Person<>> &DataManager::getAll()
{
    return persons;
}

const std::vector<Person<>> &DataManager::getAll() const
{
    return persons;
}

const Person<> *DataManager::findByName(const std::string &name) const
{
    for (auto it = persons.cbegin(); it != persons.cend(); ++it)
    {
        if (it->getName() == name)
        {
            return &(*it);
        }
    }
    return nullptr;
}

const Person<> *DataManager::findById(const IdType &id) const
{
    for (auto it = persons.cbegin(); it != persons.cend(); ++it)
    {
        if (it->getId() == id)
        {
            return &(*it);
        }
    }
    return nullptr;
}

bool DataManager::existsId(const IdType &id) const
{
    for (const auto &p : persons)
    {
        if (p.getId() == id)
        {
            return true;
        }
    }
    return false;
}

bool DataManager::add(const Person<> &p)
{
    if (existsId(p.getId()))
        return false;
    auto it = std::lower_bound(persons.begin(), persons.end(), p,
                               [](const Person<> &a, const Person<> &b)
                               { return a.getId() < b.getId(); });
    persons.insert(it, p);
    return true;
}

bool DataManager::addById(const IdType &id)
{
    if (existsId(id))
    {
        return false;
    }
    Person<> p(id);
    auto it = std::lower_bound(persons.begin(), persons.end(), p,
                               [](const Person<> &a, const Person<> &b)
                               { return a.getId() < b.getId(); });
    persons.insert(it, p);
    return true;
}

bool DataManager::removeByName(const std::string &name)
{
    bool found = false;
    std::vector<IdType> removedIds;
    for (auto it = persons.begin(); it != persons.end();)
    {
        if (it->getName() == name)
        {
            removedIds.push_back(it->getId());
            found = true;
            it = persons.erase(it);
        }
        else
        {
            ++it;
        }
    }
    if (!found)
    {
        return false;
    }
    for (auto &p : persons)
    {
        for (const auto &id : removedIds)
        {
            p.removeContactMember(id);
        }
    }
    return true;
}

bool DataManager::updateByName(const std::string &name, const Person<> &newInfo)
{
    bool found = false;
    IdType oldId = InvalidId;
    IdType newId = InvalidId;
    for (auto it = persons.begin(); it != persons.end(); ++it)
    {
        if (it->getName() == name)
        {
            oldId = it->getId();
            it->update(newInfo);
            newId = newInfo.getId();
            found = true;
        }
    }
    if (!found)
    {
        return false;
    }
    for (auto &p : persons)
    {
        if (p.removeContactMember(oldId))
            p.addContactMember(newId);
    }
    return true;
}
