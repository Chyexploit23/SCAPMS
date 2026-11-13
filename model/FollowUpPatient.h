#pragma once

#include "PatientRecord.h"
#include <string>

namespace Model {

class FollowUpPatient : public PatientRecord {
private:
    std::string previousVisitDate;

public:
    FollowUpPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& lastVisit = "2026-01-01");

    std::string getPatientType() const override;
    void displayDetails() const override;

    std::string getPreviousVisitDate() const;
};

} // namespace Model