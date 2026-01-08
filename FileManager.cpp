#include "FileManager.h"
#include "Person.h"

#include <windows.h>
#include <fstream>
#include <sstream>

FileManager::FileManager() {}

FileManager::~FileManager() {}

std::vector<std::string> FileManager::getFileList(const std::string &directory) const
{
    std::vector<std::string> list;
    std::string searchPath = directory;
    if (searchPath.empty())
        searchPath = ".";
    // Ensure trailing backslash
    if (searchPath.back() != '\\' && searchPath.back() != '/')
        searchPath += "\\";
    searchPath += "*";

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return list;

    do
    {
        const char *name = findData.cFileName;
        if (strcmp(name, " .") == 0 || strcmp(name, "..") == 0)
            continue;
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            list.emplace_back(name);
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    return list;
}

std::vector<Person<>> FileManager::read(const std::string &fileName) const
{
    std::vector<Person<>> result;
    std::ifstream ifs(fileName);
    if (!ifs)
        return result;

    std::string line;
    while (std::getline(ifs, line))
    {
        if (line.empty())
            continue;
        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> fields;
        while (std::getline(ss, token, '|'))
            fields.push_back(token); // 使用|分隔

        if (fields.size() < 8)
            continue;

        IdType id;
        std::istringstream ssId(fields[0]);
        ssId >> id;

        std::string name = fields[1];
        Person<>::Gender gender = (fields[2] == "男") ? Person<>::Gender::Male : Person<>::Gender::Female;
        int age = 0;
        try
        {
            age = std::stoi(fields[3]);
        }
        catch (...)
        {
        }
        std::string telephone = fields[4];
        std::string city = fields[5];
        std::string school = fields[6];
        std::string address = fields[7];

        Person<> p(id, name, gender, age, telephone, city, school, address);

        result.push_back(std::move(p));
    }

    return result;
}

bool FileManager::write(const std::string &fileName, const std::vector<Person<>> &persons) const
{
    std::ofstream ofs(fileName);
    if (!ofs)
        return false;

    for (const auto &p : persons)
    {
        ofs << p.getId() << '|'
            << p.getName() << '|'
            << (p.getGender() == Person<>::Gender::Male ? "男" : "女") << '|'
            << p.getAge() << '|'
            << p.getTelephone() << '|'
            << p.getCity() << '|'
            << p.getSchool() << '|'
            << p.getAddress() << '\n';
    }

    return true;
}

// ai编写