#pragma once

#include "../model/ClinicDatabase.h"
#include <queue>
#include <stack>
#include <string>

namespace Controller {

enum OperationType { ADD_PATIENT, DELETE_PATIENT, SCHEDULE_APPOINTMENT };

struct AdminOperation {
    OperationType type;
    Model::PatientRecord* patientCopy;
    Model::Appointment appointment;
};

class ClinicManager {
private:
    Model::ClinicDatabase database;
    std::queue<Model::PatientRecord*> waitingQueue;
    std::stack<AdminOperation> undoStack;

public:
    ClinicManager() = default;

    Model::ClinicDatabase& getDatabase() { return database; }
    std::queue<Model::PatientRecord*>& getWaitingQueue() { return waitingQueue; }

    void registerPatient(Model::PatientRecord* patient);
    void deletePatient(const std::string& patientID);
    
    void enqueuePatient(Model::PatientRecord* patient);
    Model::PatientRecord* dequeuePatient();

    void undoLastOperation();
};

} // namespace Controller