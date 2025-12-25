#include "Person.h"

#include <iostream>
#include <algorithm>
#include <sstream>

template <typename Tid>
Person<Tid>::Person()
    : id('\0'), name(), gender(Gender::Female), age(0), telephone(), city(), school(), address(), contactMember()
{
}

template <typename Tid>
Person<Tid>::Person(Tid id)
    : id(id), name("尚未录入"), gender(Gender::Female), age(0), telephone("尚未录入"), city("尚未录入"), school("尚未录入"), address("尚未录入"), contactMember()
{
}

template <typename Tid>
Person<Tid>::~Person() {}

template <typename Tid>
Person<Tid>::Person(Tid id,
                    const std::string &name,
                    Gender gender,
                    int age,
                    const std::string &telephone,
                    const std::string &city,
                    const std::string &school,
                    const std::string &address)
    : id(id), name(name), gender(gender), age(age), telephone(telephone), city(city), school(school), address(address) {}

template <typename Tid>
Tid Person<Tid>::getId() const
{
    return id;
}

template <typename Tid>
const std::string &Person<Tid>::getName() const
{
    return name;
}

template <typename Tid>
typename Person<Tid>::Gender Person<Tid>::getGender() const
{
    return gender;
}

template <typename Tid>
int Person<Tid>::getAge() const
{
    return age;
}

template <typename Tid>
const std::string &Person<Tid>::getTelephone() const
{
    return telephone;
}

template <typename Tid>
const std::string &Person<Tid>::getCity() const
{
    return city;
}

template <typename Tid>
const std::string &Person<Tid>::getSchool() const
{
    return school;
}

template <typename Tid>
const std::string &Person<Tid>::getAddress() const
{
    return address;
}

template <typename Tid>
const std::vector<Tid> &Person<Tid>::getContactMember() const
{
    return contactMember;
}

template <typename Tid>
void Person<Tid>::setId(Tid id)
{
    this->id = id;
}

template <typename Tid>
void Person<Tid>::setName(const std::string &name)
{
    this->name = name;
}

template <typename Tid>
void Person<Tid>::setGender(Gender gender)
{
    this->gender = gender;
}

template <typename Tid>
void Person<Tid>::setAge(int age)
{
    this->age = age;
}

template <typename Tid>
void Person<Tid>::setTelephone(const std::string &telephone)
{
    this->telephone = telephone;
}

template <typename Tid>
void Person<Tid>::setCity(const std::string &city)
{
    this->city = city;
}

template <typename Tid>
void Person<Tid>::setSchool(const std::string &school)
{
    this->school = school;
}

template <typename Tid>
void Person<Tid>::setAddress(const std::string &address)
{
    this->address = address;
}

template <typename Tid>
void Person<Tid>::addContactMember(Tid id)
{
    contactMember.push_back(id);
    sort(contactMember.begin(), contactMember.end());
}

template <typename Tid>
bool Person<Tid>::removeContactMember(Tid id)
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

template <typename Tid>
void Person<Tid>::displayInfo() const
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

template <typename Tid>
std::vector<std::string> Person<Tid>::returnInfo() const
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

template <typename Tid>
void Person<Tid>::update(const Person<Tid> &newInfo)
{
    // 保留原有联系人列表，更新其他字段
    const auto oldContacts = contactMember;
    *this = newInfo;
    for (char c : oldContacts)
    {
        addContactMember(c);
    }
}