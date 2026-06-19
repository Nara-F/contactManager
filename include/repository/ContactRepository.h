// Persistence boundary for contact data.
#pragma once

#include <utility>
#include <vector>

#include "Person.h"
#include "Types.h"

using Relationship = std::pair<IdType, IdType>;

class ContactRepository
{
public:
    virtual ~ContactRepository() = default;

    virtual bool initialize() = 0;

    virtual std::vector<Person<>> loadPersons() const = 0;
    virtual std::vector<Relationship> loadRelationships() const = 0;

    virtual bool upsertPerson(const Person<> &person) = 0;
    virtual bool replacePerson(const IdType &oldId, const Person<> &person) = 0;
    virtual bool deletePerson(const IdType &id) = 0;

    virtual bool upsertRelationship(const IdType &ownerId, const IdType &contactId) = 0;
    virtual bool deleteRelationship(const IdType &ownerId, const IdType &contactId) = 0;

    virtual bool beginTransaction() = 0;
    virtual bool commitTransaction() = 0;
    virtual bool rollbackTransaction() = 0;
};
