#include "UserInterface.h"

#include <iostream>
#include <sstream>
#include <iomanip>

UserInterface::UserInterface() {}

UserInterface::~UserInterface() {}

int UserInterface::getChoice()
{
    // TO DO: 读取并验证用户主菜单选择，返回选择的整数值
    std::string line;
    int number;
    while (true)
    {
        std::cout << "请输入选项（输入0以返回主菜单）: ";
        std::getline(std::cin, line);
        std::stringstream ss(line);
        if (std::cin.eof())
        {
            return EOF;
        }
        if (ss >> number)
        {
            // 成功读到第一个整数
            return number;
        }
        else
        {
            // 没有读到整数，输出错误信息
            std::cerr << "输入非法，请输入整数！" << std::endl;
        }
    }
}

int UserInterface::getChoice(std::string prompt)
{
    // TO DO: 读取并验证用户主菜单选择，返回选择的整数值
    std::cout << prompt << std::endl;
    std::string line;
    int number;
    while (true)
    {
        std::cout << "请输入选项: ";
        std::getline(std::cin, line);
        std::stringstream ss(line);
        if (std::cin.eof())
        {
            return EOF;
        }
        if ((ss >> number) && (number == 0 || number == 1))
        {
            // 成功读到第一个整数
            return number;
        }
        else
        {
            // 没有读到整数，输出错误信息
            std::cerr << "输入非法，请输入0或1！" << std::endl;
            return -1;
        }
    }
}

std::string UserInterface::getInputString(const std::string &prompt)
{
    // TO DO: 显示提示信息 `prompt` 并从标准输入读取一行文本，返回该字符串
    std::cout << prompt << std::endl;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

char UserInterface::getInputId(const std::string &prompt)
{
    // TO DO: 显示提示信息 `prompt` 并读取一个字符作为 ID，返回该字符
    std::string line;
    char id;
    if (std::cin.eof())
    {
        return '\0';
    }
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        std::stringstream ss(line);

        if (ss >> id)
        {
            return id;
        }
        else
        {
            std::cerr << "输入非法，请正确输入id！" << std::endl;
        }
    }
}

void UserInterface::displayMainMenu()
{
    // 主界面ui
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║               联系人管理系统 ContactManager            ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║  1. 查看文件列表（从 contactList/ 选择并加载）         ║\n";
    std::cout << "║  2. 查询全部已录入人员信息                             ║\n";
    std::cout << "║  3. 删除已记录人员信息（按姓名删除）                   ║\n";
    std::cout << "║  4. 修改已记录人员信息（按姓名覆盖更新）               ║\n";
    std::cout << "║  5. 添加特定人员的联系人信息                           ║\n";
    std::cout << "║  6. 保存特定人员的所有通讯录信息                       ║\n";
    std::cout << "║  7. 构建联系人关系网络                                 ║\n";
    std::cout << "║  0. 退出程序                                           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    for (int i = 0; i < 3; ++i)
    {
        std::cout << std::endl;
    }
}

void UserInterface::showInvalidInput()
{
    // TO DO: 显示输入无效的错误信息并提示重试
    std::cerr << "输入非法，请输入菜单编号或提示的合法值后再试。" << std::endl;
}

void UserInterface::displayExitMessage()
{
    // TO DO: 显示退出应用程序的信息
    std::cout << "已成功退出，程序将在3秒后自动关闭" << std::endl;
}

void UserInterface::displayFileList(const std::vector<std::string> &fileList)
{
    // TO DO: 列出可用的文件名列表（`fileList`），并按序号或换行显示
    std::cout << "可用文件列表（输入序号选择，0 返回主菜单）：" << std::endl;
    for (size_t i = 0; i < fileList.size(); ++i)
    {
        std::cout << i + 1 << ". " << fileList[i] << std::endl;
    }
}

void UserInterface::showFileReadSuccessMessage()
{
    // TO DO: 显示文件读取成功的提示
    std::cout << "文件读取成功！" << std::endl;
}

void UserInterface::showFileReadErrorMessage()
{
    // TO DO: 显示文件读取失败的错误提示
    std::cerr << "文件读取失败！请检查文件路径和格式。" << std::endl;
}

void UserInterface::showNoFileMessage()
{
    // TO DO: 提示尚未加载任何文件
    std::cerr << "尚未读入文件！请先选择并加载文件。" << std::endl;
}

bool UserInterface::displayAllContacts(const std::vector<std::string> &printList)
{
    // 逐条格式化显示联系人信息
    if (printList.empty())
    {
        showNoFileMessage();
        return false;
    }
    else
    {
        std::cout << "联系人列表：" << std::endl;
        for (const auto &info : printList)
        {
            std::cout << info << std::endl;
        }
        return true;
    }
}

void UserInterface::displayCertainContact(const std::vector<std::string> &printList)
{
    // TO DO: 格式化并显示单个联系人 `p` 的详细信息
    std::cout << "联系人详细信息：" << std::endl;
    for (const auto &info : printList)
    {
        std::cout << info << std::endl;
    }
}

void UserInterface::showDeleteSuccessMessage(const std::string &name)
{
    // TO DO: 显示删除成功的信息，包含被删除联系人姓名 `name`
    std::cout << "联系人" << name << "的信息已成功删除！" << std::endl;
}

void UserInterface::showDeleteErrorMessage(const std::string &name)
{
    // TO DO: 显示删除失败的信息，包含联系人姓名 `name`，并给出可能原因
    std::cerr << "删除联系人" << name << "的信息失败，请重新检查或重试！" << std::endl;
}

void UserInterface::showModifySuccessMessage(const std::string &name)
{
    // TO DO: 显示修改成功的信息，包含联系人姓名 `name`
    std::cout << "联系人" << name << "的信息已成功修改！" << std::endl;
}

void UserInterface::showModifyErrorMessage(const std::string &name)
{
    // TO DO: 显示修改失败的信息，包含联系人姓名 `name`
    std::cerr << "修改联系人" << name << "的信息失败，请重新检查或重试！" << std::endl;
}

void UserInterface::showFindNoPeopleMessage(const std::string &name)
{
    // TO DO: 显示未找到匹配联系人时的提示，包含查询关键字 `name`
    std::cerr << "未找到匹配联系人：" << name << "，请检查输入或尝试其他关键字！" << std::endl;
}

void UserInterface::showIdExistingMessage(const char id)
{
    // TO DO: 提示 ID 已存在，包含冲突的 `id` 值
    std::cerr << "ID " << id << " 已存在，请使用其他 ID！" << std::endl;
}

void UserInterface::showAddSuccessMessage(const char &id, const char &idNew)
{
    // TO DO: 显示添加联系人成功，包含新联系人 `id`
    std::cout << "联系人 " << id << " 的新联系人 " << idNew << " 已成功添加并关联。" << std::endl;
}

void UserInterface::showAddErrorMessage(const char &id, const char &idNew)
{
    // TO DO: 显示添加联系人失败，包含相关 `id`，并提示原因或下一步
    std::cerr << "添加失败：联系人 " << id << " 不存在，无法关联新联系人。请先确认拥有者已在通讯录中。" << std::endl;
}

void UserInterface::showSavingSuccessMessage(const std::string &filename)
{
    // TO DO: 显示保存成功的信息，包含目标文件名 `filename`
    std::cout << "文件 " << filename << " 已成功保存！" << std::endl;
}

void UserInterface::showSavingErrorMessage(const std::string &filename)
{
    // TO DO: 显示保存失败的错误信息，包含文件名 `filename` 和可能原因
    std::cerr << "保存文件 " << filename << " 失败，请检查是否已加载文件、路径权限或数据是否为空。" << std::endl;
}

void UserInterface::showConstructingMessage()
{
    // TO DO: 提示正在构建联系人关系网络（进度或等待信息）
    std::cout << "正在构建联系人关系网络，请稍候..." << std::endl;
}

void UserInterface::showRelationMap(const std::vector<std::vector<double>> &relationList, const std::vector<char> &idList)
{
    if (relationList.empty())
    {
        showNoFileMessage();
    }
    else
    {
        // TO DO: 格式化并显示关系矩阵 `relationList`，左侧和上方带 ID 索引
        std::cout << "联系人关系网络矩阵：" << std::endl;

        // 显示列标签（上方 ID）
        std::cout << "    ";
        for (char id : idList)
        {
            std::cout << std::setw(3) << id << " ";
        }
        std::cout << std::endl;

        // 显示分隔线
        std::cout << "    ";
        for (size_t i = 0; i < idList.size(); i++)
        {
            std::cout << "----";
        }
        std::cout << std::endl;

        // 显示矩阵数据，左侧带行标签（行 ID）
        for (size_t i = 0; i < relationList.size(); i++)
        {
            std::cout << idList[i] << " | ";
            for (const auto &val : relationList[i])
            {
                std::cout << std::setw(3) << val << " ";
            }
            std::cout << std::endl;
        }
    }
}

void UserInterface::outputStr(std::string str)
{
    std::cout << str << std::endl;
}

std::string UserInterface::getNewInformationName(const std::string &prompt, std::string name)
{
    std::cout << prompt << std::endl;

    char id;
    std::string gender;
    std::string age;
    std::string telephone;
    std::string city;
    std::string school;
    std::string address;

    std::cout << "ID："; // << std::endl;
    std::cin >> id;

    while (!std::cin.eof())
    {
        std::cout << "性别（男/女）："; // << std::endl;
        std::cin >> gender;

        if (gender == "男" || gender == "女")
        {
            break; // 合法，跳出循环
        }

        std::cerr << "输入错误：性别只能是“男”或“女”，请重新输入。\n";
    }

    while (!std::cin.eof())
    {
        std::cout << "年龄："; //<< std::endl;
        std::cin >> age;
        bool digit = false;
        for (auto &c : age)
        {
            if (!isdigit(c))
            {
                digit = false;
                break;
            }
            digit = true;
        }
        if (digit)
        {
            break; // 合法，跳出循环
        }
        std::cerr << "输入错误：请输入数字，请重新输入。\n";
    }

    std::cout << "电话："; // << std::endl;
    std::cin >> telephone;

    std::cout << "城市："; // << std::endl;
    std::cin >> city;

    std::cout << "学校："; // << std::endl;
    std::cin >> school;

    std::cout << "地址："; // << std::endl;
    std::cin >> address;

    // 按约定顺序拼接：id name gender age telephone city school address
    std::string retStr;
    retStr += id;
    retStr = retStr + " " + name + " " + gender + " " + age + " " +
             telephone + " " + city + " " + school + " " + address;
    std::cout << retStr << std::endl;
    return retStr;
}

std::string UserInterface::getNewInformationId(const std::string &prompt, char id)
{
    std::cout << prompt << std::endl;

    std::string name;
    std::string gender;
    std::string age;
    std::string telephone;
    std::string city;
    std::string school;
    std::string address;

    std::cout << "姓名："; // << std::endl;
    std::cin >> name;

    while (!std::cin.eof())
    {
        std::cout << "性别（男/女）："; //<< std::endl;
        std::cin >> gender;

        if (gender == "男" || gender == "女")
        {
            break; // 合法，跳出循环
        }

        std::cerr << "输入错误：性别只能是“男”或“女”，请重新输入。\n";
    }

    while (!std::cin.eof())
    {
        std::cout << "年龄："; //<< std::endl;
        std::cin >> age;
        bool digit = false;
        for (auto &c : age)
        {
            if (!isdigit(c))
            {
                digit = false;
                break;
            }
            digit = true;
        }
        if (digit)
        {
            break; // 合法，跳出循环
        }
        std::cerr << "输入错误：请输入数字，请重新输入。\n";
    }

    std::cout << "电话："; //<< std::endl;
    std::cin >> telephone;

    std::cout << "城市："; //<< std::endl;
    std::cin >> city;

    std::cout << "学校："; //<< std::endl;
    std::cin >> school;

    std::cout << "地址："; //<< std::endl;
    std::cin >> address;

    // 按约定顺序拼接：id name gender age telephone city school address
    std::string retStr;
    retStr += id;
    retStr = retStr + " " + name + " " + gender + " " + age + " " +
             telephone + " " + city + " " + school + " " + address;
    // std::cout << retStr << std::endl;
    return retStr;
}

void UserInterface::displayAllContactsSimple(const std::vector<std::string> &printList) {}