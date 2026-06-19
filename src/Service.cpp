#include "Service.h"
#include "DataManager.h"
#include "FileManager.h"
#include "Person.h"
#include "repository/SqliteContactRepository.h"

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>

namespace
{
std::string withTrailingSeparator(const std::filesystem::path &path)
{
    std::string value = path.string();
    if (!value.empty() && value.back() != '/' && value.back() != '\\')
    {
        value += std::filesystem::path::preferred_separator;
    }
    return value;
}

std::string resolveDataDir()
{
    if (const char *env = std::getenv("CONTACT_DATA_DIR"))
    {
        if (*env != '\0')
        {
            return withTrailingSeparator(env);
        }
    }

    const std::filesystem::path defaultDir = "contactList";
    if (std::filesystem::is_directory(defaultDir))
    {
        return withTrailingSeparator(defaultDir);
    }

    const std::filesystem::path sourceDir = "src/contactList";
    if (std::filesystem::is_directory(sourceDir))
    {
        return withTrailingSeparator(sourceDir);
    }

    return withTrailingSeparator(defaultDir);
}

std::string dataPath(const std::string &dataDir, const std::string &filename)
{
    return (std::filesystem::path(dataDir) / filename).string();
}

std::string resolveDbPath()
{
    if (const char *env = std::getenv("CONTACT_DB_PATH"))
    {
        if (*env != '\0')
        {
            return env;
        }
    }
    return "contacts.db";
}

bool runTransaction(ContactRepository &repository, const std::function<bool()> &work)
{
    if (!repository.beginTransaction())
    {
        return false;
    }

    if (!work())
    {
        repository.rollbackTransaction();
        return false;
    }

    if (!repository.commitTransaction())
    {
        repository.rollbackTransaction();
        return false;
    }

    return true;
}

Person<> *findMutableById(DataManager &dataManager, const IdType &id)
{
    for (auto &person : dataManager.getAll())
    {
        if (person.getId() == id)
        {
            return &person;
        }
    }
    return nullptr;
}
}

Service::Service() {}

Service::~Service() {}

bool Service::initialize()
{
    dataDir = resolveDataDir();
    fileList = fileManager.getFileList(dataDir);
    currentFileName.clear();

    std::cout << dataDir << std::endl;
    repository = std::make_unique<SqliteContactRepository>(resolveDbPath());
    if (!repository->initialize())
    {
        return false;
    }

    dataManager.clear();
    dataManager.load(repository->loadPersons());
    dataManager.loadRelationships(repository->loadRelationships());
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
    std::vector<Person<>> persons = fileManager.read(dataPath(dataDir, filename));
    if (persons.empty() || !repository)
    {
        currentFileName.clear();
        return 0;
    }
    IdType idFile = std::filesystem::path(filename).stem().string();

    const bool ownerExists = dataManager.existsId(idFile);
    const bool ownerInFile = std::any_of(persons.begin(), persons.end(),
                                         [&](const Person<> &person)
                                         { return person.getId() == idFile; });

    const bool persisted = runTransaction(*repository, [&]() {
        for (const auto &person : persons)
        {
            if (!repository->upsertPerson(person))
            {
                return false;
            }
        }

        if (!idFile.empty())
        {
            if (!ownerExists && !ownerInFile && !repository->upsertPerson(Person<>(idFile)))
            {
                return false;
            }

            for (const auto &newContact : persons)
            {
                if (!repository->upsertRelationship(idFile, newContact.getId()))
                {
                    return false;
                }
            }
        }

        return true;
    });

    if (!persisted)
    {
        currentFileName.clear();
        return 0;
    }

    dataManager.load(persons);
    if (!idFile.empty())
    {
        Person<> *owner = findMutableById(dataManager, idFile);
        if (!owner)
        {
            dataManager.addById(idFile);
            owner = findMutableById(dataManager, idFile);
        }
        if (owner)
        {
            for (auto &newContact : persons)
            {
                owner->addContactMember(newContact.getId());
            }
        }
    }

    currentFileName = filename;
    return 1;
}

bool Service::saveToFile(const Person<> &p)
{
    std::string filename = p.getId() + ".txt";

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
    bool result = fileManager.write(dataPath(dataDir, filename), allList);
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

bool Service::addContact(const std::string &infoStr, const IdType &addingId) // 增
{
    if (!repository || !dataManager.existsId(addingId))
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
    if (newId == addingId)
    {
        return false;
    }

    const bool persisted = runTransaction(*repository, [&]() {
        return repository->upsertPerson(newP) &&
               repository->upsertRelationship(addingId, newId);
    });
    if (!persisted)
    {
        return false;
    }

    Person<> *newContact = findMutableById(dataManager, newId);
    if (!newContact)
    {
        dataManager.add(newP);
    }
    else
    {
        newContact->update(newP);
    }

    Person<> *owner = findMutableById(dataManager, addingId);
    if (owner)
    {
        owner->addContactMember(newId);
    }
    return true;
}

const Person<> *Service::findContactByName(const std::string &name) const
{
    return dataManager.findByName(name);
}

bool Service::deleteContactByName(const std::string &name)
{
    if (!repository)
    {
        return false;
    }

    std::vector<IdType> idsToDelete;
    for (const auto &person : dataManager.getAll())
    {
        if (person.getName() == name)
        {
            idsToDelete.push_back(person.getId());
        }
    }
    if (idsToDelete.empty())
    {
        return false;
    }

    const bool persisted = runTransaction(*repository, [&]() {
        for (const auto &id : idsToDelete)
        {
            if (!repository->deletePerson(id))
            {
                return false;
            }
        }
        return true;
    });
    if (!persisted)
    {
        return false;
    }

    return dataManager.removeByName(name);
}

bool Service::updateContactById(const IdType &id, const std::string &newInfoStr)
{
    if (!repository)
    {
        return false;
    }

    if (!dataManager.existsId(id))
    {
        return false;
    }

    Person<> p = parseContactInfo(newInfoStr);
    if (p.getId() == InvalidId)
    {
        return false;
    }

    const IdType oldId = id;
    if (p.getId() != oldId && dataManager.existsId(p.getId()))
    {
        return false;
    }

    const bool persisted = runTransaction(*repository, [&]() {
        return repository->replacePerson(oldId, p);
    });
    if (!persisted)
    {
        return false;
    }

    return dataManager.updateById(oldId, p);
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
