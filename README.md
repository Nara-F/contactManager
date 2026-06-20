# ContactManager

C++17 命令行联系人管理系统。当前版本以 SQLite 为主要持久化后端，支持文件导入/导出、联系人关系维护、关系矩阵展示，并提供基础 Docker 运行方式。

程序启动时会自动初始化 SQLite schema，并把数据库中的联系人和关系加载到内存缓存；菜单中的导入、新增、修改、删除会同步写入 SQLite。

## 功能

- SQLite 持久化联系人和联系人关系
- 从 `contactList/` 导入样例联系人文件
- 查看全部联系人，按 ID 查询详情
- 按 ID 覆盖修改联系人，并同步迁移关系引用
- 为已有联系人添加新联系人
- 按姓名删除联系人
- 导出某个联系人的联系人列表
- 构建联系人关系矩阵
- 支持多字符 ID，例如 `w1`、`w2`
- 支持 Docker / Docker Compose 运行

## 本地运行

依赖：

- CMake 3.10+
- C++17 编译器
- SQLite3 开发库

Ubuntu/Debian：

```bash
sudo apt-get update
sudo apt-get install -y g++ cmake libsqlite3-dev
```

构建并运行：

```bash
cmake -S . -B build
cmake --build build
./build/ContactManager
```

## Docker 运行

```bash
docker build -t contact-manager .
docker run --rm -it -v contact-manager-data:/data contact-manager
```

Docker 默认配置：

- 数据库：`/data/contacts.db`
- 样例文件：`/app/contactList`

Compose：

```bash
docker compose build
docker compose run --rm contact-manager
```

非交互式 smoke test：

```bash
printf '0\n' | docker compose run --rm -T contact-manager
```

## 配置

默认数据库路径为当前工作目录下的 `contacts.db`，可通过环境变量覆盖：

```bash
CONTACT_DB_PATH=/path/to/contacts.db ./build/ContactManager
```

联系人文件目录查找顺序：

1. `CONTACT_DATA_DIR`
2. `contactList/`
3. `src/contactList/`

```bash
CONTACT_DATA_DIR=/path/to/contactList ./build/ContactManager
```

## 菜单

```text
1. 查看文件列表（从 contactList/ 选择并导入）
2. 查询全部已录入人员信息
3. 删除已记录人员信息（按姓名删除）
4. 修改已记录人员信息（按 ID 覆盖更新）
5. 添加特定人员的联系人信息
6. 导出特定人员的所有通讯录信息
7. 构建联系人关系网络
0. 退出程序
```

输入 `0` 可取消当前操作或返回主菜单。性别当前只接受 `男` 或 `女`，年龄必须是数字。

## 文件格式

联系人文件使用 `|` 分隔字段：

```text
id|姓名|性别|年龄|电话|城市|学校|地址
```

示例：

```text
a|张三|男|20|13800000001|北京|北京大学|北京市海淀区
```

导入 `a.txt` 时，文件名 stem `a` 会作为关系拥有者 ID，文件内联系人会记录为 `a` 的联系人。

## 数据库

核心表：

```sql
CREATE TABLE IF NOT EXISTS persons (
    id TEXT PRIMARY KEY,
    name TEXT,
    gender TEXT,
    age INTEGER,
    telephone TEXT,
    city TEXT,
    school TEXT,
    address TEXT
);

CREATE TABLE IF NOT EXISTS relationships (
    owner_id TEXT NOT NULL,
    contact_id TEXT NOT NULL,
    PRIMARY KEY (owner_id, contact_id),
    FOREIGN KEY (owner_id) REFERENCES persons(id) ON DELETE CASCADE,
    FOREIGN KEY (contact_id) REFERENCES persons(id) ON DELETE CASCADE
);
```

运行时开启 `PRAGMA foreign_keys = ON`。

## 结构

```text
include/
  repository/ContactRepository.h
  repository/SqliteContactRepository.h
  Person.h, Types.h, Service.h, DataManager.h, FileManager.h, App.h, UserInterface.h

src/
  repository/SqliteContactRepository.cpp
  Service.cpp, DataManager.cpp, FileManager.cpp, App.cpp, UserInterface.cpp, main.cpp
  contactList/
```

## 验证

```bash
cmake -S . -B /tmp/contactManager-check
cmake --build /tmp/contactManager-check -j2
printf '0\n' | CONTACT_DB_PATH=/tmp/contact-manager-smoke.db /tmp/contactManager-check/ContactManager
```

Docker：

```bash
docker compose config
docker compose build
printf '0\n' | docker compose run --rm -T contact-manager
```

样例文件全部导入后，当前预期为：

- `persons = 10`
- `relationships = 11`
