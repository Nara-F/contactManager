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
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
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

        Person<> p = Person<>::fromString(line);
        if (p.getId() != InvalidId)
        {
            result.push_back(std::move(p));
        }
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