#pragma once

#include <string>

namespace Model {

class Appointment {
private:
    std::string appointmentID;
    std::string patientID;
    std::string appointmentDate;
    std::string appointmentTime;
    std::string reason;
    std::string status;

public:
    Appointment(const std::string& appID = "", const std::string& pID = "", const std::string& date = "", const std::string& time = "", const std::string& r = "", const std::string& s = "Scheduled");

    std::string getAppointmentID() const;
    std::string getPatientID() const;
    std::string getAppointmentDate() const;
    std::string getAppointmentTime() const;
    std::string getReason() const;
    std::string getStatus() const;

    void setAppointmentID(const std::string& id);
    void setPatientID(const std::string& id);
    void setAppointmentDate(const std::string& date);
    void setAppointmentTime(const std::string& time);
    void setReason(const std::string& r);
    void setStatus(const std::string& s);
};

} // namespace Model