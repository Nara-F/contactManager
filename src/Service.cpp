#include "Service.h"
#include "DataManager.h"
#include "FileManager.h"
#include "Person.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
Service::Service() {}

Service::~Service() {}

bool Service::initialize()
{
    fileList = fileManager.getFileList(dataDir);
    currentFileName.clear();

    std::cout << dataDir << std::endl;
    return true;
}

Person<> Service::parseContactInfo(std::string infoStr) const
{
    Person<> p = Person<>::fromString(infoStr);
    if (p.getId() == InvalidId && !infoStr.empty())
    {
        std::cerr << "parseErrInformation: " << infoStr << std::endl;
    }
    return p;
}

std::vector<std::string> Service::getFileList() const
{
    return fileList;
}

int Service::loadFromFile(const std::string &filename)
{
    std::vector<Person<>> persons = fileManager.read(dataDir + filename);
    if (persons.empty())
    {
        currentFileName.clear();
        return 0;
    }
    IdType idFile;
    std::istringstream ss(filename);
    if (ss >> idFile)
    {
        auto &all = dataManager.getAll();
        bool find = false;
        for (auto &p : all)
        {
            if (p.getId() == idFile)
            {
                find = true;
                for (auto &newContact : persons)
                {
                    p.addContactMember(newContact.getId());
                }
            }
        }
        if (!find)
        {
            Person<> newPerson(idFile);
            for (auto &newContact : persons)
            {
                newPerson.addContactMember(newContact.getId());
            }
            all.push_back(newPerson);
        }
    }

    std::vector<Person<>> vec(persons.begin(), persons.end());
    dataManager.load(vec);
    currentFileName = filename;
    return 1;
}

bool Service::saveToFile(const Person<> &p)
{
    std::string filename;
    filename += p.getId();
    filename += ".txt";

    std::vector<IdType> contactList = p.getContactMember();
    std::vector<Person<>> allList;
    for (auto &cId : contactList)
    {
        const Person<> *contact = dataManager.findById(cId);
        if (contact)
        {
            allList.push_back(*contact);
        }
    }
    bool result = fileManager.write(dataDir + filename, allList);
    if (result)
    {
        fileList = fileManager.getFileList(dataDir);
    }
    return result;
}

const std::string &Service::getCurrentFileName() const
{
    return currentFileName;
}

std::vector<std::string> Service::getAllContacts() const
{
    std::vector<std::string> all;
    all.reserve(100);
    for (const auto &p : dataManager.getAll())
    {
        auto info = p.returnInfo();
        all.insert(all.end(), info.begin(), info.end());
    }
    return all;
}

std::vector<std::string> Service::getCertainContact(const IdType &id) const
{
    std::vector<std::string> info;
    const auto *p = findContactById(id);
    if (p)
    {
        info = p->returnInfo();
    }
    return info;
}

const Person<> *Service::findContactById(const IdType &id) const
{
    return dataManager.findById(id);
}

bool Service::addContact(const std::string &infoStr, IdType addingId) // 增
{
    Person<> *owner = nullptr;
    for (auto &person : dataManager.getAll())
    {
        if (person.getId() == addingId)
        {
            owner = &person;
        }
    }
    if (!owner)
    {
        return false;
    }
    Person<> newP = parseContactInfo(infoStr);
    // std::cout << infoStr << std::endl;
    if (newP.getId() == InvalidId)
    {
        // std::cerr << "parse failed\n";
        return false;
    }

    const IdType newId = newP.getId();

    if (!dataManager.existsId(newId))
    {
        dataManager.add(newP);
    }
    else
    {
        auto &all = dataManager.getAll();
        for (auto &person : all)
        {
            if (person.getId() == newId)
            {
                person.update(newP);
                break;
            }
        }
    }

    owner->addContactMember(newId);
    return true;
}

const Person<> *Service::findContactByName(const std::string &name) const
{
    return dataManager.findByName(name);
}

bool Service::deleteContactByName(const std::string &name)
{
    bool ok = dataManager.removeByName(name);
    return ok;
}

bool Service::updateContact(const std::string &name, const std::string &newInfoStr)
{
    Person<> p = parseContactInfo(newInfoStr);
    bool ok = dataManager.updateByName(name, p);
    return ok;
}

std::vector<std::vector<double>> Service::buildRelationNetwork() const
{
    std::vector<IdType> idList;
    for (const auto &person : dataManager.getAll())
    {
        idList.push_back(person.getId());
    }

    int n = idList.size();
    std::vector<std::vector<double>> network(n, std::vector<double>(n, 0.0));

    // 预构建每个人员的联系人集合，O(1) 查找
    std::vector<std::unordered_set<IdType>> contactSets;
    contactSets.reserve(n);
    for (IdType id : idList)
    {
        const Person<> *p = dataManager.findById(id);
        if (p)
        {
            const auto &members = p->getContactMember();
            contactSets.emplace_back(members.begin(), members.end());
        }
        else
        {
            contactSets.emplace_back();
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            bool find1 = contactSets[i].count(idList[j]) > 0;
            bool find2 = contactSets[j].count(idList[i]) > 0;

            if (find1 && find2)
            {
                network[i][j] = 1.0;
            }
            else if (find1 || find2)
            {
                network[i][j] = 0.5;
            }
        }
    }

    return network;
}

std::vector<IdType> Service::getSortedIdList() const
{
    std::vector<IdType> idList;
    for (const auto &person : dataManager.getAll())
    {
        idList.push_back(person.getId());
    }
    return idList;
}