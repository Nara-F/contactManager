// person数据类声明
#pragma once

#include <string>
#include <vector>

class Person
{
public:
    enum class Gender
    {
        Female = 0,
        Male = 1
    };

    Person();
    Person(char id,
           const std::string &name,
           Gender gender,
           int age,
           const std::string &telephone,
           const std::string &city,
           const std::string &school,
           const std::string &address);
    Person(char id);

    virtual ~Person();

    char getId() const;
    const std::string &getName() const;
    Gender getGender() const;
    int getAge() const;
    const std::string &getTelephone() const;
    const std::string &getCity() const;
    const std::string &getSchool() const;
    const std::string &getAddress() const;
    const std::vector<char> &getContactMember() const;

    void setId(char id);
    void setName(const std::string &name);
    void setGender(Gender gender);
    void setAge(int age);
    void setTelephone(const std::string &telephone);
    void setCity(const std::string &city);
    void setSchool(const std::string &school);
    void setAddress(const std::string &address);
    void addContactMember(char id);
    bool removeContactMember(char id);

    void displayInfo() const;
    std::vector<std::string> returnInfo() const;

    void update(const Person &newInfo);

private:
    char id;
    std::string name;
    Gender gender;
    int age;
    std::string telephone;
    std::string city;
    std::string school;
    std::string address;
    std::vector<char> contactMember; // 按id存储联系人列表
};