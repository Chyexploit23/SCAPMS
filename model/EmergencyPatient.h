#pragma once

#include "PatientRecord.h"
#include <string>

namespace Model {

class EmergencyPatient : public PatientRecord {
private:
    int severityLevel; // 1 (High) to 5 (Low)

public:
    EmergencyPatient(const std::string& id, const std::string& name, int age, const std::string& contact, int severity = 1);

    std::string getPatientType() const override;
    void displayDetails() const override;

    int getSeverityLevel() const;
};

} // namespace Model