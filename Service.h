// 服务类声明
#pragma once

#include <string>
#include <vector>

#include "FileManager.h"
#include "DataManager.h"
#include "Person.h"

class DataManager;
class FileManager;

class Service
{
public:
    Service();
    ~Service();

    bool initialize();

    Person<> parseContactInfo(std::string infoStr) const; // 解析信息为Person

    std::vector<std::string> getFileList() const;  // 获取文件列表
    int loadFromFile(const std::string &filename); // 从文件加载联系人数据 //需要判断id是否重复并预处理为persons链表传入dataManager
    bool saveToFile(const Person<> &p);            // 保存单个人的所有通讯人信息到文件
    const std::string &getCurrentFileName() const; // 返回文件名

    std::vector<std::string> getAllContacts() const; // 查
    const Person<> *findContactByName(const std::string &name) const;
    const Person<> *findContactById(const IdType &id) const;
    bool addContact(const std::string &infoStr, IdType addingId); // 增
    std::vector<std::string> getCertainContact(const IdType &id) const;

    bool deleteContactByName(const std::string &name); // 删

    bool updateContact(const std::string &name, const std::string &newInfoStr); // 改

    std::vector<std::vector<double>> buildRelationNetwork() const; // 构建关系网络
    std::vector<IdType> getSortedIdList() const;                   // 获取排序后的ID列表

private:
    DataManager dataManager;
    FileManager fileManager;

    // 状态
    std::vector<std::string> fileList;
    std::string currentFileName;
    std::string dataDir = "contactList/"; // 默认文件目录
};