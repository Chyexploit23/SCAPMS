#pragma once

#include "PatientRecord.h"
#include <string>

namespace Model {

class GeneralPatient : public PatientRecord {
private:
    std::string primaryPhysician;

public:
    GeneralPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& physician = "Dr. Smith");

    std::string getPatientType() const override;
    void displayDetails() const override;

    std::string getPrimaryPhysician() const;
};

} // namespace Model