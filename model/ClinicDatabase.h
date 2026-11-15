#pragma once

#include <vector>
#include <string>
#include "PatientRecord.h"
#include "Appointment.h"

namespace Model {
class ClinicDatabase {
private:
    std::vector<PatientRecord*> patients;
    std::vector<Appointment> appointments;

public:
    ClinicDatabase() = default;
    ~ClinicDatabase();

    void addPatient(PatientRecord* patient);
    void deletePatient(const std::string& patientID);
    PatientRecord* findPatientByID(const std::string& id);
    const std::vector<PatientRecord*>& getPatients() const;
    std::vector<PatientRecord*>& getPatientsMutable(); 
    
    void addAppointment(const Appointment& appointment);
    const std::vector<Appointment>& getAppointments() const;
    void cancelAppointment(const std::string& appointmentID);

    void clear();
};
}