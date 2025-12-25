// person数据类声明
#pragma once

#include <string>
#include <vector>

template <typename Tid = char>
class Person
{
public:
    enum class Gender
    {
        Female = 0,
        Male = 1
    };

    Person();
    Person(Tid id,
           const std::string &name,
           Gender gender,
           int age,
           const std::string &telephone,
           const std::string &city,
           const std::string &school,
           const std::string &address);
    Person(Tid id);

    virtual ~Person();

    Tid getId() const;
    const std::string &getName() const;
    Gender getGender() const;
    int getAge() const;
    const std::string &getTelephone() const;
    const std::string &getCity() const;
    const std::string &getSchool() const;
    const std::string &getAddress() const;
    const std::vector<Tid> &getContactMember() const;

    void setId(Tid id);
    void setName(const std::string &name);
    void setGender(Gender gender);
    void setAge(int age);
    void setTelephone(const std::string &telephone);
    void setCity(const std::string &city);
    void setSchool(const std::string &school);
    void setAddress(const std::string &address);
    void addContactMember(Tid id);
    bool removeContactMember(Tid id);

    void displayInfo() const;
    std::vector<std::string> returnInfo() const;

    void update(const Person &newInfo);

private:
    Tid id;
    std::string name;
    Gender gender;
    int age;
    std::string telephone;
    std::string city;
    std::string school;
    std::string address;
    std::vector<Tid> contactMember; // 按id存储联系人列表
};

#include "Person.tpp"