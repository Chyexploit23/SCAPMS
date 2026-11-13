#pragma once

#include "../model/PatientRecord.h"
#include <vector>
#include <string>

namespace Controller {

class SearchEngine {
public:
    // Manual Searching Algorithms (Requirement 4)
    static Model::PatientRecord* linearSearchByID(const std::vector<Model::PatientRecord*>& patients, const std::string& targetID);
    static Model::PatientRecord* binarySearchByID(std::vector<Model::PatientRecord*> patients, const std::string& targetID);
    static Model::PatientRecord* recursiveSearchByName(const std::vector<Model::PatientRecord*>& patients, const std::string& targetName, size_t index = 0);

    // Manual Sorting Algorithms (Requirement 5)
    static void selectionSortByName(std::vector<Model::PatientRecord*>& patients);
    static void insertionSortByAge(std::vector<Model::PatientRecord*>& patients);
    static void quickSortByID(std::vector<Model::PatientRecord*>& patients, int low, int high);
};

} // namespace Controller