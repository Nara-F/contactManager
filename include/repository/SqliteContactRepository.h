// SQLite implementation of ContactRepository.
#pragma once

#include <sqlite3.h>

#include <string>

#include "repository/ContactRepository.h"

class SqliteContactRepository : public ContactRepository
{
public:
    explicit SqliteContactRepository(std::string dbPath);
    ~SqliteContactRepository() override;

    bool initialize() override;

    std::vector<Person<>> loadPersons() const override;
    std::vector<Relationship> loadRelationships() const override;

    bool upsertPerson(const Person<> &person) override;
    bool replacePerson(const IdType &oldId, const Person<> &person) override;
    bool deletePerson(const IdType &id) override;

    bool upsertRelationship(const IdType &ownerId, const IdType &contactId) override;
    bool deleteRelationship(const IdType &ownerId, const IdType &contactId) override;

    bool beginTransaction() override;
    bool commitTransaction() override;
    bool rollbackTransaction() override;

private:
    bool execute(const char *sql) const;
    bool ensureOpen() const;

    std::string dbPath;
    sqlite3 *db = nullptr;
};
