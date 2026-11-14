#include "SearchEngine.h"

namespace Controller {

Model::PatientRecord* SearchEngine::linearSearchByID(const std::vector<Model::PatientRecord*>& patients, const std::string& targetID) {
    for (auto p : patients) {
        if (p->getPatientID() == targetID) return p;
    }
    return nullptr;
}

Model::PatientRecord* SearchEngine::binarySearchByID(std::vector<Model::PatientRecord*> patients, const std::string& targetID) {
    // Binary search must not change the user-visible ordering of the database.
    for (size_t i = 1; i < patients.size(); ++i) {
        Model::PatientRecord *key = patients[i];
        size_t j = i;
        while (j > 0 && patients[j - 1]->getPatientID() > key->getPatientID()) {
            patients[j] = patients[j - 1];
            --j;
        }
        patients[j] = key;
    }
    int low = 0, high = static_cast<int>(patients.size()) - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (patients[mid]->getPatientID() == targetID) return patients[mid];
        if (patients[mid]->getPatientID() < targetID) low = mid + 1;
        else high = mid - 1;
    }
    return nullptr;
}

Model::PatientRecord* SearchEngine::recursiveSearchByName(const std::vector<Model::PatientRecord*>& patients, const std::string& targetName, size_t index) {
    if (index >= patients.size()) return nullptr;
    if (patients[index]->getName() == targetName) return patients[index];
    return recursiveSearchByName(patients, targetName, index + 1);
}

void SearchEngine::selectionSortByName(std::vector<Model::PatientRecord*>& patients) {
    size_t n = patients.size();
    for (size_t i = 0; i + 1 < n; ++i) {
        size_t minIdx = i;
        for (size_t j = i + 1; j < n; ++j) {
            if (patients[j]->getName() < patients[minIdx]->getName()) {
                minIdx = j;
            }
        }
        std::swap(patients[i], patients[minIdx]);
    }
}

void SearchEngine::insertionSortByAge(std::vector<Model::PatientRecord*>& patients) {
    size_t n = patients.size();
    for (size_t i = 1; i < n; ++i) {
        Model::PatientRecord* key = patients[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0 && patients[j]->getAge() > key->getAge()) {
            patients[j + 1] = patients[j];
            j--;
        }
        patients[j + 1] = key;
    }
}

void SearchEngine::quickSortByID(std::vector<Model::PatientRecord*>& patients, int low, int high) {
    if (low < high) {
        std::string pivot = patients[high]->getPatientID();
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (patients[j]->getPatientID() < pivot) {
                i++;
                std::swap(patients[i], patients[j]);
            }
        }
        std::swap(patients[i + 1], patients[high]);
        int pi = i + 1;

        quickSortByID(patients, low, pi - 1);
        quickSortByID(patients, pi + 1, high);
    }
}

} // namespace Controller