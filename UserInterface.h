// 用户界面类声明
#pragma once

#include <vector>
#include <list>
#include <string>

using IdType = char;

class Service;

class UserInterface
{
public:
    UserInterface();
    virtual ~UserInterface();

    int getChoice();                                       // 获取用户选择
    int getChoice(std::string prompt);                     // 获取用户选择带提示
    std::string getInputString(const std::string &prompt); // 获取字符串输入
    IdType getInputId(const std::string &prompt);          // 获取id输入

    void displayMainMenu();    // 初始化界面
    void showInvalidInput();   // 显示非法输入信息
    void displayExitMessage(); // 显示退出信息

    void displayFileList(const std::vector<std::string> &fileList); // 显示文件列表
    void showFileReadSuccessMessage();
    void showFileReadErrorMessage();
    // void showReadDuplicateId(); // 读取文件时遇到重复id
    void showNoFileMessage(); // 尚未读入文件

    bool displayAllContacts(const std::vector<std::string> &printList); // 1、查询通讯录信息
    void displayCertainContact(const std::vector<std::string> &printList);

    void showDeleteSuccessMessage(const std::string &name); // 2、删除通讯录记录信息
    void showDeleteErrorMessage(const std::string &name);

    void showModifySuccessMessage(const std::string &name); // 3、修改通讯录记录信息
    void showModifyErrorMessage(const std::string &name);
    void showFindNoPeopleMessage(const std::string &name);
    std::string getNewInformationName(const std::string &prompt, std::string name);

    void showIdExistingMessage(const IdType &id); // 4、添加通讯录记录信息
    void showAddSuccessMessage(const IdType &id, const IdType &idNew);
    void showAddErrorMessage(const IdType &id, const IdType &idNew);
    std::string getNewInformationId(const std::string &prompt, IdType id);
    void displayAllContactsSimple(const std::vector<std::string> &printList);

    void showSavingSuccessMessage(const std::string &filename); // 5、保存通讯录信息
    void showSavingErrorMessage(const std::string &filename);

    void showConstructingMessage(); // 6、构建联系人关系网络
    void showRelationMap(const std::vector<std::vector<double>> &relationList, const std::vector<IdType> &idList);

    void outputStr(std::string str);
};