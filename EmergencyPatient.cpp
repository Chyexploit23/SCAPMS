#include "EmergencyPatient.h"
#include <iostream>

namespace Model {

EmergencyPatient::EmergencyPatient(const std::string& id, const std::string& name, int age, const std::string& contact, int severity)
    : PatientRecord(id, name, age, contact), severityLevel(severity) {}

std::string EmergencyPatient::getPatientType() const {
    return "Emergency";
}

void EmergencyPatient::displayDetails() const {
    std::cout << "[Emergency - Priority " << severityLevel << "] ID: " << patientID << " | Name: " << name << " | Age: " << age << std::endl;
}

int EmergencyPatient::getSeverityLevel() const {
    return severityLevel;
}

} // namespace Model