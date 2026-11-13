#pragma once

#include <string>

namespace Model {

class PatientRecord {
protected:
    std::string patientID;
    std::string name;
    int age;
    std::string contactInfo;

public:
    PatientRecord(const std::string& id, const std::string& name, int age, const std::string& contact);
    virtual ~PatientRecord() = default;

    virtual std::string getPatientType() const = 0;
    virtual void displayDetails() const = 0;

    std::string getPatientID() const { return patientID; }
    std::string getName() const { return name; }
    int getAge() const { return age; }
    std::string getContactInfo() const { return contactInfo; }

    void setPatientID(const std::string& id);
    void setName(const std::string& n);
    void setAge(int a);
    void setContactInfo(const std::string& c);
};

} // namespace Model