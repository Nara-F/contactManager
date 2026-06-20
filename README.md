# ContactManager

C++17 命令行联系人管理系统，SQLite 持久化，支持关系建模与矩阵可视化。

## 架构

```
main.cpp → App（调度）
             ├── Service（业务编排）
             ├── UserInterface（终端 I/O）
             └── (via Service)
                   ├── DataManager           内存缓存 vector<Person<>>
                   ├── FileManager           文件读写
                   └── ContactRepository     持久化接口（抽象）
                         └── SqliteContactRepository
```

- `Person<Tid = IdType>` 模板类，`IdType = std::string`，天然支持多字符 ID
- Repository 模式：`ContactRepository` 抽象接口，可替换后端
- 启动时 SQLite → 内存；运行时双写，保持一致性
- 关系以 `(owner_id, contact_id)` 对存储

## 功能

导入 · 查询 · 添加 · 修改（自动迁移关系） · 删除（级联） · 导出 · 关系矩阵 · SQLite 持久化 · Docker

## 依赖

- CMake 3.10+
- C++17 编译器（GCC 8+ / Clang 7+）
- libsqlite3-dev

Ubuntu/Debian：

```bash
sudo apt-get install -y g++ cmake libsqlite3-dev
```

## 构建与运行

```bash
cmake -S . -B build && cmake --build build
./build/ContactManager
```

## Docker

```bash
docker build -t contact-manager .
docker run --rm -it -v contact-manager-data:/data contact-manager

# 或 Compose
docker compose build
docker compose run --rm contact-manager
```

多阶段构建（Ubuntu 22.04），默认数据库 `/data/contacts.db`，数据目录 `/app/contactList`。

## 配置

```bash
CONTACT_DB_PATH=/path/to/contacts.db ./build/ContactManager
CONTACT_DATA_DIR=/path/to/contactList ./build/ContactManager
```

数据目录查找顺序：`CONTACT_DATA_DIR` → `contactList/` → `src/contactList/`

## 使用

### 菜单

```
1. 查看文件列表（导入联系人文件）
2. 查询全部已录入人员信息
3. 删除已记录人员信息（按 ID）
4. 修改已记录人员信息（按 ID 覆盖）
5. 添加特定人员的联系人信息
6. 导出特定人员的所有通讯录信息
7. 构建联系人关系网络
0. 退出程序
```

输入 `0` 可取消当前操作。性别仅接受 `男`/`女`，年龄需为数字。

### 文件格式

管道分隔，每行一条记录：

```
id|姓名|性别|年龄|电话|城市|学校|地址
```

导入 `a.txt` 时，文件名 stem `a` 作为关系拥有者，文件内每条记录为该拥有者的联系人。

## 数据库

```sql
CREATE TABLE persons (
    id TEXT PRIMARY KEY, name TEXT, gender TEXT, age INTEGER,
    telephone TEXT, city TEXT, school TEXT, address TEXT
);
CREATE TABLE relationships (
    owner_id TEXT, contact_id TEXT,
    PRIMARY KEY (owner_id, contact_id),
    FOREIGN KEY (owner_id)  REFERENCES persons(id) ON DELETE CASCADE,
    FOREIGN KEY (contact_id) REFERENCES persons(id) ON DELETE CASCADE
);
```

启动时 `PRAGMA foreign_keys = ON`。导入全部示例后：`persons = 10`，`relationships = 11`。

## 项目结构

```
├── CMakeLists.txt
├── Dockerfile / docker-compose.yml
├── include/
│   ├── App.h, Service.h, DataManager.h, FileManager.h
│   ├── Person.h, Types.h, UserInterface.h
│   └── repository/  (ContactRepository.h, SqliteContactRepository.h)
└── src/
    ├── main.cpp, App.cpp, Service.cpp, DataManager.cpp
    ├── FileManager.cpp, UserInterface.cpp
    ├── contactList/  (a.txt ~ e.txt 示例文件)
    └── repository/  (SqliteContactRepository.cpp)
```

## 快速验证

```bash
cmake -S . -B /tmp/contactManager-check && cmake --build /tmp/contactManager-check -j2
printf '0\n' | CONTACT_DB_PATH=/tmp/contact-manager-smoke.db /tmp/contactManager-check/ContactManager

# Docker
docker compose build
printf '0\n' | docker compose run --rm -T contact-manager
```
