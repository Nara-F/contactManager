# ContactManager 联系人管理系统

一个基于 C++ 的命令行联系人管理系统，支持联系人的增删改查、SQLite 持久化、文件导入导出和关系网络构建。

## 功能特性

- **文件管理**：从 `contactList/` 目录导入联系人文件，并可导出联系人通讯录
- **SQL 持久化**：使用 SQLite 保存联系人和联系人关系
- **查询功能**：查看所有联系人或查询特定联系人详情
- **添加联系人**：为已有人员添加新的联系人
- **修改信息**：按 ID 修改联系人信息
- **删除联系人**：按姓名删除联系人记录
- **保存数据**：将联系人信息保存到文件
- **关系网络**：构建并显示联系人关系矩阵

## 项目结构

```
ContactManager/
├──src/
   ├── main.cpp           # 程序入口
   ├── App.cpp/h          # 应用主控制器
   ├── Service.cpp/h      # 业务逻辑层
   ├── DataManager.cpp/h  # 数据管理层
   ├── FileManager.cpp/h  # 文件导入/导出管理
   ├── UserInterface.cpp/h # 用户界面交互
   ├── Person.h           # 联系人模板类声明
   ├── contactList/       # 联系人数据文件目录
       ├── a.txt
       ├── b.txt
       └── ...
├──include/
   ├── Types.h            # 共享 ID 类型定义
   ├── Person.h
   ├── repository/        # 持久化仓储接口和 SQLite 实现声明
   └── detail/Person.tpp  # 联系人模板类实现
```

## 数据格式

联系人数据使用 `|` 作为字段分隔符：

```
id|姓名|性别|年龄|电话|城市|学校|地址
```

示例：

```
a|张三|男|20|13800000001|北京|北京大学|北京市海淀区
```

## 编译运行

### 编译

```bash
cmake -S . -B build
cmake --build build
```

构建需要 C++17 编译器和 SQLite3 开发库。

### 运行

```bash
./build/ContactManager
```

程序默认查找 `contactList/`，找不到时会回退到 `src/contactList/`。也可以通过环境变量指定数据目录：

```bash
CONTACT_DATA_DIR=/path/to/contactList ./build/ContactManager
```

程序默认使用当前工作目录下的 `contacts.db` 作为 SQLite 数据库。也可以通过环境变量指定数据库路径：

```bash
CONTACT_DB_PATH=/path/to/contacts.db ./build/ContactManager
```

## 使用说明

### 主菜单

```
             联系人管理系统 ContactManager

  1. 查看文件列表（从 contactList/ 选择并导入）
  2. 查询全部已录入人员信息
  3. 删除已记录人员信息（按姓名删除）
  4. 修改已记录人员信息（按 ID 覆盖更新）
  5. 添加特定人员的联系人信息
  6. 导出特定人员的所有通讯录信息
  7. 构建联系人关系网络
  0. 退出程序
```

### 操作提示

- 在任何输入过程中，输入 `0` 可以取消当前操作并返回主菜单
- 性别只能输入 `男` 或 `女`
- 年龄必须为数字
- 电话、城市、学校、地址等字段留空会自动填充为"未填写"

## 技术实现

### 语言与标准

- **C++17** 标准，使用现代 C++ 特性

### 架构设计

采用**分层架构**，职责分离清晰：

```
┌─────────────────────────────────────┐
│         UserInterface (UI层)        │  用户交互、输入输出
├─────────────────────────────────────┤
│              App (控制层)            │  流程控制、菜单调度
├─────────────────────────────────────┤
│           Service (业务层)           │  业务逻辑、数据解析
├─────────────────────────────────────┤
│         DataManager (数据层)         │  内存数据管理、CRUD操作
├─────────────────────────────────────┤
│     ContactRepository (持久层)       │  SQLite 数据持久化
├─────────────────────────────────────┤
│         FileManager (文件适配)       │  文件导入导出、目录遍历
└─────────────────────────────────────┘
```

### 核心技术

| 技术                             | 应用场景                                  |
| :------------------------------- | :---------------------------------------- |
| **模板类 `Person<>`**            | 联系人数据结构，支持泛型 ID 类型          |
| **STL 容器**                     | `std::vector` 存储联系人集合              |
| **文件系统 `std::filesystem`**   | 跨平台目录遍历和路径处理                  |
| **SQLite3**                      | 联系人和关系数据持久化                    |
| **字符串流 `std::stringstream`** | 数据解析与格式化                          |
| **文件流 `std::fstream`**        | 文件读写操作                              |

### 数据结构

- **`std::vector<Person<>>`**：主数据容器，支持排序和遍历
- **`std::vector<IdType>`**：联系人关系列表
- **`std::vector<std::vector<double>>`**：关系网络邻接矩阵
- **`persons` 表**：保存联系人详情
- **`relationships` 表**：保存联系人有向关系

### 输入处理

- 统一使用 **`std::getline()`** 读取输入，避免 `cin >>` 与 `getline` 混用导致的缓冲区问题
- 使用 **`|` 分隔符** 解析和存储数据，支持字段包含空格
- 输入验证：性别、年龄等字段的合法性检查
- 空输入处理：自动填充默认值 "未填写"

### 跨平台处理

- **目录遍历**：使用 `std::filesystem`，不依赖 Windows API
- **控制台编码**：仅在 Windows 下设置 UTF-8 控制台编码
- **延时反馈**：使用 `std::this_thread::sleep_for`

### 设计模式

- **MVC 模式**：Model（Person/DataManager）、View（UserInterface）、Controller（App/Service）
- **单一职责原则**：每个类专注于特定功能

## 注意事项

1. 确保系统安装 SQLite3 开发库
2. 确保 `contactList/`、`src/contactList/` 或 `CONTACT_DATA_DIR` 指向的目录存在且包含有效的联系人文件
3. 文件编码需为 UTF-8
4. 程序支持 C++17 编译器下的 Windows/Linux/macOS 构建
