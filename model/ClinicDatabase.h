#pragma once

#include "PatientRecord.h"
#include "Appointment.h"
#include <vector>

namespace Model {

class ClinicDatabase {
private:
    std::vector<PatientRecord*> patients;
    std::vector<Model::Appointment> appointments;

    static const int CACHE_SIZE = 10;
    PatientRecord* recentPatientsCache[CACHE_SIZE];
    int cacheCount;

public:
    ClinicDatabase();
    ~ClinicDatabase();

    void addPatient(PatientRecord* patient);
    void deletePatient(const std::string& patientID);
    PatientRecord* findPatientByID(const std::string& patientID);

    std::vector<PatientRecord*>& getPatients() { return patients; }
    const std::vector<Model::Appointment>& getAppointments() const { return appointments; }

    void addAppointment(const Model::Appointment& app);
    void cancelAppointment(const std::string& appID);

    void updateRecentCache(PatientRecord* patient);
};

} // namespace Model