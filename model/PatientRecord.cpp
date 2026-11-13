#include "PatientRecord.h"
#include "ClinicException.h"
#include <iostream>

namespace Model {

PatientRecord::PatientRecord(const std::string& id, const std::string& name, int age, const std::string& contact)
    : patientID(id), name(name), age(age), contactInfo(contact) {
    if (id.empty()) {
        throw Utils::ClinicException("Patient ID cannot be empty.");
    }
    if (age <= 0 || age > 120) {
        throw Utils::ClinicException("Invalid patient age provided.");
    }
}

void PatientRecord::setPatientID(const std::string& id) {
    if (id.empty()) {
        throw Utils::ClinicException("Patient ID cannot be empty.");
    }
    patientID = id;
}

void PatientRecord::setName(const std::string& n) {
    if (n.empty()) {
        throw Utils::ClinicException("Patient name cannot be empty.");
    }
    name = n;
}

void PatientRecord::setAge(int a) {
    if (a <= 0 || a > 120) {
        throw Utils::ClinicException("Age must be between 1 and 120.");
    }
    age = a;
}

void PatientRecord::setContactInfo(const std::string& c) {
    contactInfo = c;
}

} // namespace Model