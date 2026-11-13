#include "Appointment.h"
#include "ClinicException.h"

namespace Model {

Appointment::Appointment(const std::string& appID, const std::string& pID, const std::string& date, const std::string& time, const std::string& r, const std::string& s)
    : appointmentID(appID), patientID(pID), appointmentDate(date), appointmentTime(time), reason(r), status(s) {}

std::string Appointment::getAppointmentID() const { return appointmentID; }
std::string Appointment::getPatientID() const { return patientID; }
std::string Appointment::getAppointmentDate() const { return appointmentDate; }
std::string Appointment::getAppointmentTime() const { return appointmentTime; }
std::string Appointment::getReason() const { return reason; }
std::string Appointment::getStatus() const { return status; }

void Appointment::setAppointmentID(const std::string& id) {
    if (id.empty()) throw Utils::ClinicException("Appointment ID cannot be empty.");
    appointmentID = id;
}

void Appointment::setPatientID(const std::string& id) {
    if (id.empty()) throw Utils::ClinicException("Patient ID cannot be empty.");
    patientID = id;
}

void Appointment::setAppointmentDate(const std::string& date) {
    if (date.empty()) throw Utils::ClinicException("Appointment date cannot be empty.");
    appointmentDate = date;
}

void Appointment::setAppointmentTime(const std::string& time) {
    if (time.empty()) throw Utils::ClinicException("Appointment time cannot be empty.");
    appointmentTime = time;
}

void Appointment::setReason(const std::string& r) {
    reason = r;
}

void Appointment::setStatus(const std::string& s) {
    status = s;
}

} // namespace Model