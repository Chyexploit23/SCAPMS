#include "FollowUpPatient.h"
#include <iostream>

namespace Model {

FollowUpPatient::FollowUpPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& lastVisit)
    : PatientRecord(id, name, age, contact), previousVisitDate(lastVisit) {}

std::string FollowUpPatient::getPatientType() const {
    return "Follow-Up";
}

void FollowUpPatient::displayDetails() const {
    std::cout << "[Follow-Up] ID: " << patientID << " | Name: " << name << " | Last Visit: " << previousVisitDate << std::endl;
}

std::string FollowUpPatient::getPreviousVisitDate() const {
    return previousVisitDate;
}

} // namespace Model