#pragma once

#include <string>

namespace Model {

class Appointment {
private:
    std::string appointmentID;
    std::string patientID;
    std::string date;
    std::string time;
    std::string status;

public:
    Appointment(const std::string& appID, const std::string& pID, const std::string& d, const std::string& t, const std::string& s = "Scheduled")
        : appointmentID(appID), patientID(pID), date(d), time(t), status(s) {}

    std::string getAppointmentID() const { return appointmentID; }
    std::string getPatientID() const { return patientID; }
    std::string getDate() const { return date; }
    std::string getTime() const { return time; }
    std::string getStatus() const { return status; }

    void setStatus(const std::string& newStatus) { status = newStatus; }
};

} // namespace Model