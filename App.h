// 调度类声明
#pragma once

#include "Service.h"
#include "UserInterface.h"

class App
{
public:
    App();
    virtual ~App();

    void run();

private:
    Service service;
    UserInterface ui;

    void handleFileSelection(); // 选择文件

    void handleContactSearch(); // 查询通讯录信息

    void handleContactDeletion(); // 删除通讯录记录信息

    void handleContactModification(); // 修改通讯录记录信息

    void handleContactAddition(); // 添加通讯录记录信息

    void handleDataSaving(); // 保存通讯录信息

    void handleRelationNetworkConstruction(); // 构建联系人关系网络

    void handleInvalidChoice(); // 非法输入

    void handleExit(); // 退出程序
};