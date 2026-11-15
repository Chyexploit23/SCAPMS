#include "ClinicDatabase.h"
#include <algorithm>

namespace Model {

ClinicDatabase::~ClinicDatabase() {
    clear();
}

void ClinicDatabase::addPatient(PatientRecord* patient) {
    if (patient) {
        patients.push_back(patient);
    }
}

void ClinicDatabase::deletePatient(const std::string& patientID) {
    for (auto it = patients.begin(); it != patients.end(); ++it) {
        if (*it && (*it)->getPatientID() == patientID) {
            delete *it; // Free memory
            patients.erase(it); // Remove from vector
            break;
        }
    }
}

PatientRecord* ClinicDatabase::findPatientByID(const std::string& id) {
    for (auto* patient : patients) {
        if (patient && patient->getPatientID() == id) {
            return patient;
        }
    }
    return nullptr;
}

const std::vector<PatientRecord*>& ClinicDatabase::getPatients() const {
    return patients;
}

std::vector<PatientRecord*>& ClinicDatabase::getPatientsMutable() {
    return patients;
}

void ClinicDatabase::addAppointment(const Appointment& appointment) {
    appointments.push_back(appointment);
}

const std::vector<Appointment>& ClinicDatabase::getAppointments() const {
    return appointments;
}

void ClinicDatabase::cancelAppointment(const std::string& appointmentID) {
    for (auto& appointment : appointments) {
        if (appointment.getAppointmentID() == appointmentID) {
            appointment.setStatus("Cancelled");
            break;
        }
    }
}

void ClinicDatabase::clear() {
    for (auto* patient : patients) {
        delete patient;
    }
    patients.clear();
    appointments.clear();
}

} // namespace Model