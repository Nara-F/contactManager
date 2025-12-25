#include "App.h"
#include "Service.h"
#include "UserInterface.h"

#include <iostream>
#include <windows.h>

App::App() {}

App::~App() {}

void App::run()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    service.initialize();
    bool running = true;
    while (running)
    {
        ui.displayMainMenu();
        int choice = ui.getChoice();
        switch (choice)
        {
        case 1: // 查看文件列表
            handleFileSelection();
            break;
        case 2: // 查询通讯录信息
            handleContactSearch();
            break;
        case 3: // 删除通讯录记录信息
            handleContactDeletion();
            break;
        case 4: // 修改通讯录记录信息
            handleContactModification();
            break;
        case 5: // 添加通讯录记录信息
            handleContactAddition();
            break;
        case 6: // 保存通讯录信息
            handleDataSaving();
            break;
        case 7: // 构建联系人关系网络
            handleRelationNetworkConstruction();
            break;
        case 0: // 退出程序
            handleExit();
            running = false;
            break;
        default:
            handleInvalidChoice();
            break;
        }
    }
}

void App::handleFileSelection()
{
    // TODO: 实现选择文件逻辑
    ui.displayFileList(service.getFileList());
    int choice = 0;
    do
    {
        choice = ui.getChoice();
        const auto fileCount = static_cast<int>(service.getFileList().size());
        if (choice == 0)
        {
            return;
        }
        if (choice < 1 || choice > fileCount)
        {
            ui.showInvalidInput();
            continue;
        }
        int flag = service.loadFromFile(service.getFileList()[choice - 1]);
        if (flag)
        {
            ui.showFileReadSuccessMessage();
        }
        /*else if (flag == 0)//重复id
        {

        }
        */
        else
        {
            ui.showFileReadErrorMessage();
        }
    } while (choice != 0);
}

void App::handleContactSearch()
{
    // TODO: 实现查询通讯录信息逻辑
    while (true)
    {
        if (!ui.displayAllContacts(service.getAllContacts()))
        {
            Sleep(2000);
            return;
        }
        else
        {
            char id = ui.getInputId("请输入要查询的人员id（输入0返回主菜单）：");
            if (id == '0')
            {
                return;
            }

            const auto contact = service.findContactById(id);
            if (!contact)
            {
                ui.showInvalidInput();
                continue;
            }

            ui.displayCertainContact(service.getCertainContact(id));
            Sleep(3000);
            return;
        }
    }
}

void App::handleContactDeletion() // Beta*
{
    // TODO: 实现删除通讯录记录信息逻辑
    if (!ui.displayAllContacts(service.getAllContacts()))
    {
        Sleep(2000);
        return;
    }
    while (true)
    {
        std::string name = ui.getInputString("请输入要删除的联系人姓名（输入0返回主菜单）：");
        if (name == "0")
        {
            return;
        }

        if (service.deleteContactByName(name))
        {
            ui.showDeleteSuccessMessage(name);
            Sleep(3000);
            return;
        }

        ui.showDeleteErrorMessage(name);
        Sleep(2000);
    }
}

void App::handleContactModification()
{
    // TODO: 实现修改通讯录记录信息逻辑
    if (!ui.displayAllContacts(service.getAllContacts()))
    {
        Sleep(2000);
        return;
    }
    while (true)
    {
        std::string name = ui.getInputString("请输入要修改的人员姓名（输入0返回主菜单）：");
        if (name == "0")
        {
            return;
        }

        if (!service.findContactByName(name))
        {
            ui.showInvalidInput();
            continue;
        }

        if (service.updateContact(name, ui.getNewInformationName("请输入新的人员信息：", name)))
        {
            ui.showModifySuccessMessage(name);
            Sleep(3000);
            return;
        }

        ui.showModifyErrorMessage(name);
    }
}

void App::handleContactAddition() // 可以拓展加上同id信息覆盖的选择
{
    // TODO: 实现添加通讯录记录信息逻辑
    if (!ui.displayAllContacts(service.getAllContacts()))
    {
        Sleep(2000);
        return;
    }
    char addingId = ui.getInputId("请输入要添加新联系人的人员id（输入0返回主菜单）：");
    if (addingId == '0')
    {
        return;
    }
    char id;
    while (true)
    {
        id = ui.getInputId("请输入要添加的新联系人id（输入0返回主菜单）：");
        if (id == '0')
        {
            return;
        }
        if (id == addingId)
        {
            ui.showInvalidInput();
        }
        else
        {
            break;
        }
    }

    /*if (service.findContactById(id) != nullptr)
    {
        ui.showIdExistingMessage(id);
    }
    */
    // else
    //{
    while (!service.addContact(ui.getNewInformationId("请输入新的联系人信息：", id), addingId))
    {
        ui.showAddErrorMessage(addingId, id);
    }
    ui.showAddSuccessMessage(addingId, id);
    //}
    Sleep(3000);
}

void App::handleDataSaving()
{
    // TODO: 实现保存通讯录信息逻辑
    while (true)
    {
        if (!ui.displayAllContacts(service.getAllContacts()))
        {
            Sleep(2000);
            return;
        }
        char id = ui.getInputId("请输入要保存的联系人id（输入0返回主菜单）：");
        if (id == '0')
            return;
        int choice = 0;
        ui.displayCertainContact(service.getCertainContact(id));
        while (true)
        {
            choice = ui.getChoice("确认保存该联系人信息到文件？（1-确认，0-取消并返回主菜单）：");
            if (choice == 1)
            {
                break;
            }
            else if (choice == 0)
            {
                return;
            }
        }
        const auto contact = service.findContactById(id);
        if (!contact)
        {
            ui.showInvalidInput();
            continue;
        }

        if (service.saveToFile(*contact))
        {
            std::string filename;
            filename += id;
            filename += ".txt";
            ui.showSavingSuccessMessage(filename);
            Sleep(3000);
            return;
        }

        ui.showSavingErrorMessage(service.getCurrentFileName());
        // 继续循环，允许重试或输入0退出
    }
}

void App::handleRelationNetworkConstruction()
{
    // TODO: 实现构建联系人关系网络逻辑
    ui.showConstructingMessage();
    Sleep(2000);

    auto relationNetwork = service.buildRelationNetwork();
    auto idList = service.getSortedIdList();

    ui.showRelationMap(relationNetwork, idList);
    Sleep(3000);
}

void App::handleInvalidChoice()
{
    // TODO: 实现非法输入处理逻辑
    ui.showInvalidInput();
}

void App::handleExit()
{
    // TODO: 实现退出程序逻辑
    ui.displayExitMessage();
    Sleep(3000);
}
