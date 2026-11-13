#include "GeneralPatient.h"
#include <iostream>

namespace Model {

GeneralPatient::GeneralPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& physician)
    : PatientRecord(id, name, age, contact), primaryPhysician(physician) {}

std::string GeneralPatient::getPatientType() const {
    return "General";
}

void GeneralPatient::displayDetails() const {
    std::cout << "[General] ID: " << patientID << " | Name: " << name << " | Age: " << age << " | Physician: " << primaryPhysician << std::endl;
}

std::string GeneralPatient::getPrimaryPhysician() const {
    return primaryPhysician;
}

} // namespace Model