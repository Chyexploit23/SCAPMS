#include "ClinicDatabase.h"
#include "ClinicException.h"
#include <algorithm>

namespace Model {

ClinicDatabase::ClinicDatabase() : cacheCount(0) {
    for (int i = 0; i < CACHE_SIZE; ++i) {
        recentPatientsCache[i] = nullptr;
    }
}

ClinicDatabase::~ClinicDatabase() {
    for (auto p : patients) {
        delete p;
    }
    patients.clear();
}

void ClinicDatabase::addPatient(PatientRecord* patient) {
    if (!patient) return;
    for (const auto* existing : patients) {
        if (existing->getPatientID() == patient->getPatientID()) {
            throw Utils::ClinicException("Patient ID already exists.");
        }
    }
    patients.push_back(patient);
    updateRecentCache(patient);
}

void ClinicDatabase::deletePatient(const std::string& patientID) {
    auto it = std::remove_if(patients.begin(), patients.end(), [&](PatientRecord* p) {
        if (p->getPatientID() == patientID) {
            for (int i = 0; i < cacheCount; ++i) {
                if (recentPatientsCache[i] == p) recentPatientsCache[i] = nullptr;
            }
            delete p;
            return true;
        }
        return false;
    });
    patients.erase(it, patients.end());
}

PatientRecord* ClinicDatabase::findPatientByID(const std::string& patientID) {
    // Search cache first
    for (int i = 0; i < cacheCount; ++i) {
        if (recentPatientsCache[i] && recentPatientsCache[i]->getPatientID() == patientID) {
            return recentPatientsCache[i];
        }
    }
    // Search main vector
    for (auto p : patients) {
        if (p->getPatientID() == patientID) {
            updateRecentCache(p);
            return p;
        }
    }
    return nullptr;
}

void ClinicDatabase::addAppointment(const Appointment& app) {
    for (const auto& existing : appointments) {
        if (!app.getAppointmentID().empty() && existing.getAppointmentID() == app.getAppointmentID()) {
            throw Utils::ClinicException("Appointment ID already exists.");
        }
    }
    appointments.push_back(app);
}

void ClinicDatabase::cancelAppointment(const std::string& appID) {
    for (auto& app : appointments) {
        if (app.getAppointmentID() == appID) {
            app.setStatus("Cancelled");
            break;
        }
    }
}

void ClinicDatabase::updateRecentCache(PatientRecord* patient) {
    if (cacheCount < CACHE_SIZE) {
        recentPatientsCache[cacheCount++] = patient;
    } else {
        // Shift left
        for (int i = 0; i < CACHE_SIZE - 1; ++i) {
            recentPatientsCache[i] = recentPatientsCache[i + 1];
        }
        recentPatientsCache[CACHE_SIZE - 1] = patient;
    }
}

} // namespace Model