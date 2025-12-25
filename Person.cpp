#include "Person.h"

#include <iostream>
#include <algorithm>
#include <sstream>

Person::Person()
    : id('\0'), name(), gender(Gender::Female), age(0), telephone(), city(), school(), address(), contactMember()
{
}

Person::~Person() {}

Person::Person(char id,
               const std::string &name,
               Gender gender,
               int age,
               const std::string &telephone,
               const std::string &city,
               const std::string &school,
               const std::string &address)
    : id(id), name(name), gender(gender), age(age), telephone(telephone), city(city), school(school), address(address) {}

Person::Person(char id)
    : id(id), name("尚未录入"), gender(Gender::Female), age(0), telephone("尚未录入"), city("尚未录入"), school("尚未录入"), address("尚未录入"), contactMember()
{
}

char Person::getId() const
{
    return id;
}

const std::string &Person::getName() const
{
    return name;
}

Person::Gender Person::getGender() const
{
    return gender;
}

int Person::getAge() const
{
    return age;
}

const std::string &Person::getTelephone() const
{
    return telephone;
}

const std::string &Person::getCity() const
{
    return city;
}

const std::string &Person::getSchool() const
{
    return school;
}

const std::string &Person::getAddress() const
{
    return address;
}

const std::vector<char> &Person::getContactMember() const
{
    return contactMember;
}

void Person::setId(char id)
{
    this->id = id;
}

void Person::setName(const std::string &name)
{
    this->name = name;
}

void Person::setGender(Gender gender)
{
    this->gender = gender;
}

void Person::setAge(int age)
{
    this->age = age;
}

void Person::setTelephone(const std::string &telephone)
{
    this->telephone = telephone;
}

void Person::setCity(const std::string &city)
{
    this->city = city;
}

void Person::setSchool(const std::string &school)
{
    this->school = school;
}

void Person::setAddress(const std::string &address)
{
    this->address = address;
}

void Person::addContactMember(char id)
{
    contactMember.push_back(id);
    sort(contactMember.begin(), contactMember.end());
}

bool Person::removeContactMember(char id)
{
    for (auto it = contactMember.begin(); it != contactMember.end(); ++it)
    {
        if (*it == id)
        {
            contactMember.erase(it);
            return true;
            break;
        }
    }
    return false;
}

void Person::displayInfo() const
{
    std::cout << "ID: " << id << "  名字: " << name
              << "性别: " << (gender == Gender::Male ? "男" : "女") << std::endl;
    std::cout
        << "  年龄: " << age << "  电话: " << telephone
        << "  城市: " << city << "  学校: " << school
        << "  地址: " << address << std::endl;
    std::cout << "  联系人列表: " << std::endl;
    for (auto &contactId : contactMember)
    {
        std::cout << contactId << ' ';
    }
    std::cout << std::endl;
}

std::vector<std::string> Person::returnInfo() const
{
    std::vector<std::string> retstr;
    std::ostringstream ss;

    ss << "ID: " << id << "  名字: " << name
       << "  性别: " << (gender == Gender::Male ? "男" : "女");
    retstr.push_back(ss.str());

    ss.str("");
    ss.clear();
    ss << "  年龄: " << age << "  电话: " << telephone
       << "  城市: " << city << "  学校: " << school
       << "  地址: " << address;
    retstr.push_back(ss.str());

    ss.str("");
    ss.clear();
    ss << "  联系人列表: ";
    for (size_t i = 0; i < contactMember.size(); ++i)
    {
        ss << contactMember[i];
        if (i + 1 < contactMember.size())
            ss << ' ';
    }
    retstr.push_back(ss.str());

    return retstr;
}

void Person::update(const Person &newInfo)
{
    // 保留原有联系人列表，更新其他字段
    const auto oldContacts = contactMember;
    *this = newInfo;
    for (char c : oldContacts)
    {
        addContactMember(c);
    }
}