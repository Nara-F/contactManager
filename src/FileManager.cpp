#include "FileManager.h"
#include "Person.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

FileManager::FileManager() {}

FileManager::~FileManager() {}

std::vector<std::string> FileManager::getFileList(const std::string &directory) const
{
    std::vector<std::string> list;

    const std::filesystem::path dirPath = directory.empty() ? "." : directory;
    std::error_code ec;
    if (!std::filesystem::exists(dirPath, ec) || !std::filesystem::is_directory(dirPath, ec))
    {
        return list;
    }

    for (const auto &entry : std::filesystem::directory_iterator(dirPath, ec))
    {
        if (ec)
        {
            break;
        }
        if (entry.is_regular_file(ec))
        {
            list.emplace_back(entry.path().filename().string());
        }
    }

    std::sort(list.begin(), list.end());
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
