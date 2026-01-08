#include "Service.h"
#include "DataManager.h"
#include "FileManager.h"
#include "Person.h"

#include <list>
#include <sstream>
#include <iostream>
#include <algorithm>
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
    // 期望格式：id|name|gender|age|telephone|city|school|address（使用|分隔）
    if (infoStr.empty())
    {
        return Person<>(InvalidId, "", Person<>::Gender::Female, 0, "00000000000", "", "", "");
    }

    std::vector<std::string> fields;
    std::string field;
    std::istringstream ss(infoStr);

    while (std::getline(ss, field, '|'))
    {
        fields.push_back(field);
    }

    if (fields.size() < 8)
    {
        std::cerr << "parseErrInformation: 字段不足 " << infoStr << std::endl;
        return Person<>(InvalidId, "", Person<>::Gender::Female, 0, "00000000000", "", "", "");
    }

    IdType id = fields[0].empty() ? InvalidId : fields[0][0];
    std::string name = fields[1];
    std::string genderStr = fields[2];
    int age = 0;
    try
    {
        age = std::stoi(fields[3]);
    }
    catch (...)
    {
        age = 0;
    }
    std::string telephone = fields[4];
    std::string city = fields[5];
    std::string school = fields[6];
    std::string address = fields[7];

    Person<>::Gender gender = (genderStr == "男") ? Person<>::Gender::Male : Person<>::Gender::Female;

    return Person<>(id, name, gender, age, telephone, city, school, address);
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

    std::list<Person<>> lst(persons.begin(), persons.end());
    dataManager.load(lst);
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
    // 从已排序的 persons 列表中提取 ID
    std::vector<IdType> idList;
    for (const auto &person : dataManager.getAll())
    {
        idList.push_back(person.getId());
    }

    int n = idList.size();
    std::vector<std::vector<double>> network(n, std::vector<double>(n, 0.0));

    // 构建 n*n 关系矩阵，数值代表联系强度：1.0 双向，0.5 单向，0 无联系
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            IdType idI = idList[i];
            IdType idJ = idList[j];

            const Person<> *personI = dataManager.findById(idI);
            const Person<> *personJ = dataManager.findById(idJ);

            if (!personI || !personJ)
            {
                continue;
            }

            bool find1 = false, find2 = false;

            // 检查 i 是否包含 j
            for (const auto &cid : personI->getContactMember())
            {
                if (cid == idJ)
                {
                    find1 = true;
                    break;
                }
            }

            // 检查 j 是否包含 i
            for (const auto &cid : personJ->getContactMember())
            {
                if (cid == idI)
                {
                    find2 = true;
                    break;
                }
            }

            if (find1 && find2)
            {
                network[i][j] = 1.0; // 双向联系
            }
            else if (find1 || find2)
            {
                network[i][j] = 0.5; // 单向联系
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