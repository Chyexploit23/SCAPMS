#include "ClinicManager.h"
#include "../model/GeneralPatient.h"
#include "../model/EmergencyPatient.h"
#include "../model/FollowUpPatient.h"
#include <utility>

namespace {
void removeFromQueue(std::queue<Model::PatientRecord *> &queue, const std::string &patientID)
{
    std::queue<Model::PatientRecord *> remaining;
    while (!queue.empty()) {
        Model::PatientRecord *patient = queue.front();
        queue.pop();
        if (patient && patient->getPatientID() != patientID) remaining.push(patient);
    }
    queue.swap(remaining);
}
}

namespace Controller {

ClinicManager::~ClinicManager()
{
    while (!undoStack.empty()) {
        delete undoStack.top().patientCopy;
        undoStack.pop();
    }
}

void ClinicManager::registerPatient(Model::PatientRecord *patient)
{
    if (!patient) return;
    const std::string id = patient->getPatientID();
    try {
        database.addPatient(patient);
    } catch (...) {
        delete patient;
        throw;
    }
    try {
        undoStack.push({ADD_PATIENT, id, nullptr, Model::Appointment()});
    } catch (...) {
        database.deletePatient(id);
        throw;
    }
}

void ClinicManager::deletePatient(const std::string &patientID)
{
    Model::PatientRecord *patient = database.findPatientByID(patientID);
    if (!patient) return;

    // Keep a polymorphic snapshot so Undo remains safe after the database deletes the original.
    Model::PatientRecord *snapshot = nullptr;
    if (patient->getPatientType() == "General") {
        auto *general = static_cast<Model::GeneralPatient *>(patient);
        snapshot = new Model::GeneralPatient(patient->getPatientID(), patient->getName(), patient->getAge(), patient->getContactInfo(), general->getPrimaryPhysician());
    } else if (patient->getPatientType() == "Emergency") {
        auto *emergency = static_cast<Model::EmergencyPatient *>(patient);
        snapshot = new Model::EmergencyPatient(patient->getPatientID(), patient->getName(), patient->getAge(), patient->getContactInfo(), emergency->getSeverityLevel());
    } else {
        auto *followUp = static_cast<Model::FollowUpPatient *>(patient);
        snapshot = new Model::FollowUpPatient(patient->getPatientID(), patient->getName(), patient->getAge(), patient->getContactInfo(), followUp->getPreviousVisitDate());
    }

    try {
        undoStack.push({DELETE_PATIENT, patientID, snapshot, Model::Appointment()});
    } catch (...) {
        delete snapshot;
        throw;
    }
    std::queue<Model::PatientRecord *> remaining;
    while (!waitingQueue.empty()) {
        Model::PatientRecord *queuedPatient = waitingQueue.front();
        waitingQueue.pop();
        if (queuedPatient != patient) remaining.push(queuedPatient);
    }
    waitingQueue.swap(remaining);
    database.deletePatient(patientID);
}

void ClinicManager::enqueuePatient(Model::PatientRecord *patient)
{
    if (patient) waitingQueue.push(patient);
}

Model::PatientRecord *ClinicManager::dequeuePatient()
{
    if (waitingQueue.empty()) return nullptr;
    Model::PatientRecord *next = waitingQueue.front();
    waitingQueue.pop();
    return next;
}

void ClinicManager::undoLastOperation()
{
    if (undoStack.empty()) return;
    AdminOperation operation = undoStack.top();
    undoStack.pop();

    if (operation.type == ADD_PATIENT) {
        removeFromQueue(waitingQueue, operation.patientID);
        database.deletePatient(operation.patientID);
    } else if (operation.type == DELETE_PATIENT && operation.patientCopy) {
        try {
            database.addPatient(operation.patientCopy);
            operation.patientCopy = nullptr; // Ownership moves back to ClinicDatabase.
        } catch (...) {
            undoStack.push(std::move(operation));
            throw;
        }
    }
    delete operation.patientCopy;
}

} // namespace Controller
