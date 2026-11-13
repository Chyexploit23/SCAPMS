#include "ClinicManager.h"

namespace Controller {

void ClinicManager::registerPatient(Model::PatientRecord* patient) {
    database.addPatient(patient);
    AdminOperation op{ADD_PATIENT, patient, Model::Appointment("", "", "", "")};
    undoStack.push(op);
}

void ClinicManager::deletePatient(const std::string& patientID) {
    Model::PatientRecord* p = database.findPatientByID(patientID);
    if (p) {
        AdminOperation op{DELETE_PATIENT, p, Model::Appointment("", "", "", "")};
        undoStack.push(op);
        database.deletePatient(patientID);
    }
}

void ClinicManager::enqueuePatient(Model::PatientRecord* patient) {
    if (patient) {
        waitingQueue.push(patient);
    }
}

Model::PatientRecord* ClinicManager::dequeuePatient() {
    if (waitingQueue.empty()) return nullptr;
    Model::PatientRecord* next = waitingQueue.front();
    waitingQueue.pop();
    return next;
}

void ClinicManager::undoLastOperation() {
    if (undoStack.empty()) return;
    AdminOperation lastOp = undoStack.top();
    undoStack.pop();

    if (lastOp.type == ADD_PATIENT) {
        if (lastOp.patientCopy) {
            database.deletePatient(lastOp.patientCopy->getPatientID());
        }
    }
}

} // namespace Controller