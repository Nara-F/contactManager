#include "repository/SqliteContactRepository.h"

#include <utility>

namespace
{
const char *genderToText(Person<>::Gender gender)
{
    return gender == Person<>::Gender::Male ? "男" : "女";
}

Person<>::Gender textToGender(const unsigned char *text)
{
    if (text && std::string(reinterpret_cast<const char *>(text)) == "男")
    {
        return Person<>::Gender::Male;
    }
    return Person<>::Gender::Female;
}

std::string columnText(sqlite3_stmt *stmt, int column)
{
    const unsigned char *text = sqlite3_column_text(stmt, column);
    return text ? reinterpret_cast<const char *>(text) : "";
}

bool bindText(sqlite3_stmt *stmt, int index, const std::string &value)
{
    return sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
}

bool stepDone(sqlite3_stmt *stmt)
{
    return sqlite3_step(stmt) == SQLITE_DONE;
}
}

SqliteContactRepository::SqliteContactRepository(std::string dbPath)
    : dbPath(std::move(dbPath))
{
}

SqliteContactRepository::~SqliteContactRepository()
{
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool SqliteContactRepository::initialize()
{
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
    {
        return false;
    }

    return execute("PRAGMA foreign_keys = ON;") &&
           execute("CREATE TABLE IF NOT EXISTS persons ("
                   "id TEXT PRIMARY KEY,"
                   "name TEXT NOT NULL,"
                   "gender TEXT NOT NULL,"
                   "age INTEGER NOT NULL,"
                   "telephone TEXT,"
                   "city TEXT,"
                   "school TEXT,"
                   "address TEXT"
                   ");") &&
           execute("CREATE TABLE IF NOT EXISTS relationships ("
                   "owner_id TEXT NOT NULL,"
                   "contact_id TEXT NOT NULL,"
                   "PRIMARY KEY(owner_id, contact_id),"
                   "FOREIGN KEY(owner_id) REFERENCES persons(id) ON DELETE CASCADE,"
                   "FOREIGN KEY(contact_id) REFERENCES persons(id) ON DELETE CASCADE"
                   ");");
}

std::vector<Person<>> SqliteContactRepository::loadPersons() const
{
    std::vector<Person<>> persons;
    if (!ensureOpen())
    {
        return persons;
    }

    const char *sql = "SELECT id, name, gender, age, telephone, city, school, address "
                      "FROM persons ORDER BY id;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return persons;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        persons.emplace_back(
            columnText(stmt, 0),
            columnText(stmt, 1),
            textToGender(sqlite3_column_text(stmt, 2)),
            sqlite3_column_int(stmt, 3),
            columnText(stmt, 4),
            columnText(stmt, 5),
            columnText(stmt, 6),
            columnText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return persons;
}

std::vector<Relationship> SqliteContactRepository::loadRelationships() const
{
    std::vector<Relationship> relationships;
    if (!ensureOpen())
    {
        return relationships;
    }

    const char *sql = "SELECT owner_id, contact_id FROM relationships ORDER BY owner_id, contact_id;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return relationships;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        relationships.emplace_back(columnText(stmt, 0), columnText(stmt, 1));
    }

    sqlite3_finalize(stmt);
    return relationships;
}

bool SqliteContactRepository::upsertPerson(const Person<> &person)
{
    if (!ensureOpen() || person.getId() == InvalidId)
    {
        return false;
    }

    const char *sql = "INSERT INTO persons(id, name, gender, age, telephone, city, school, address) "
                      "VALUES(?, ?, ?, ?, ?, ?, ?, ?) "
                      "ON CONFLICT(id) DO UPDATE SET "
                      "name = excluded.name,"
                      "gender = excluded.gender,"
                      "age = excluded.age,"
                      "telephone = excluded.telephone,"
                      "city = excluded.city,"
                      "school = excluded.school,"
                      "address = excluded.address;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    bool ok = bindText(stmt, 1, person.getId()) &&
              bindText(stmt, 2, person.getName()) &&
              bindText(stmt, 3, genderToText(person.getGender())) &&
              sqlite3_bind_int(stmt, 4, person.getAge()) == SQLITE_OK &&
              bindText(stmt, 5, person.getTelephone()) &&
              bindText(stmt, 6, person.getCity()) &&
              bindText(stmt, 7, person.getSchool()) &&
              bindText(stmt, 8, person.getAddress()) &&
              stepDone(stmt);

    sqlite3_finalize(stmt);
    return ok;
}

bool SqliteContactRepository::replacePerson(const IdType &oldId, const Person<> &person)
{
    if (oldId == person.getId())
    {
        return upsertPerson(person);
    }
    if (!upsertPerson(person))
    {
        return false;
    }

    const char *migrateOwnerSql =
        "INSERT OR IGNORE INTO relationships(owner_id, contact_id) "
        "SELECT ?, contact_id FROM relationships WHERE owner_id = ?;";
    const char *deleteOwnerSql = "DELETE FROM relationships WHERE owner_id = ?;";
    const char *migrateContactSql =
        "INSERT OR IGNORE INTO relationships(owner_id, contact_id) "
        "SELECT owner_id, ? FROM relationships WHERE contact_id = ?;";
    const char *deleteContactSql = "DELETE FROM relationships WHERE contact_id = ?;";
    const char *deleteOldPersonSql = "DELETE FROM persons WHERE id = ?;";

    auto runTwoText = [&](const char *sql, const IdType &first, const IdType &second) {
        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        {
            return false;
        }
        bool ok = bindText(stmt, 1, first) && bindText(stmt, 2, second) && stepDone(stmt);
        sqlite3_finalize(stmt);
        return ok;
    };

    auto runOneText = [&](const char *sql, const IdType &value) {
        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        {
            return false;
        }
        bool ok = bindText(stmt, 1, value) && stepDone(stmt);
        sqlite3_finalize(stmt);
        return ok;
    };

    return runTwoText(migrateOwnerSql, person.getId(), oldId) &&
           runOneText(deleteOwnerSql, oldId) &&
           runTwoText(migrateContactSql, person.getId(), oldId) &&
           runOneText(deleteContactSql, oldId) &&
           runOneText(deleteOldPersonSql, oldId);
}

bool SqliteContactRepository::deletePerson(const IdType &id)
{
    if (!ensureOpen())
    {
        return false;
    }

    const char *sql = "DELETE FROM persons WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    bool ok = bindText(stmt, 1, id) && stepDone(stmt);
    sqlite3_finalize(stmt);
    return ok;
}

bool SqliteContactRepository::upsertRelationship(const IdType &ownerId, const IdType &contactId)
{
    if (!ensureOpen() || ownerId == InvalidId || contactId == InvalidId)
    {
        return false;
    }

    const char *sql = "INSERT OR IGNORE INTO relationships(owner_id, contact_id) VALUES(?, ?);";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    bool ok = bindText(stmt, 1, ownerId) && bindText(stmt, 2, contactId) && stepDone(stmt);
    sqlite3_finalize(stmt);
    return ok;
}

bool SqliteContactRepository::deleteRelationship(const IdType &ownerId, const IdType &contactId)
{
    if (!ensureOpen())
    {
        return false;
    }

    const char *sql = "DELETE FROM relationships WHERE owner_id = ? AND contact_id = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    bool ok = bindText(stmt, 1, ownerId) && bindText(stmt, 2, contactId) && stepDone(stmt);
    sqlite3_finalize(stmt);
    return ok;
}

bool SqliteContactRepository::beginTransaction()
{
    return execute("BEGIN TRANSACTION;");
}

bool SqliteContactRepository::commitTransaction()
{
    return execute("COMMIT;");
}

bool SqliteContactRepository::rollbackTransaction()
{
    return execute("ROLLBACK;");
}

bool SqliteContactRepository::execute(const char *sql) const
{
    if (!ensureOpen())
    {
        return false;
    }

    char *error = nullptr;
    const int rc = sqlite3_exec(db, sql, nullptr, nullptr, &error);
    if (error)
    {
        sqlite3_free(error);
    }
    return rc == SQLITE_OK;
}

bool SqliteContactRepository::ensureOpen() const
{
    return db != nullptr;
}
