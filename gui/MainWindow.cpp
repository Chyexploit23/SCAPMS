#include "MainWindow.h"
#include "../model/GeneralPatient.h"
#include "../model/EmergencyPatient.h"
#include "../model/FollowUpPatient.h"
#include "../controller/SearchEngine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Smart Clinic Appointment & Patient Management System (SCAPMS)");
    resize(1050, 700);

    tabWidget = new QTabWidget(this);

    QWidget *patientTab = new QWidget();
    QWidget *appointmentTab = new QWidget();
    QWidget *queueTab = new QWidget();
    QWidget *searchSortTab = new QWidget();

    setupPatientTab(patientTab);
    setupAppointmentTab(appointmentTab);
    setupQueueTab(queueTab);
    setupSearchSortTab(searchSortTab);

    tabWidget->addTab(patientTab, "Patient Records");
    tabWidget->addTab(appointmentTab, "Appointments");
    tabWidget->addTab(queueTab, "Waiting Queue");
    tabWidget->addTab(searchSortTab, "Search & Sort");

    setCentralWidget(tabWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::setupPatientTab(QWidget *tab) {
    QHBoxLayout *mainLayout = new QHBoxLayout(tab);

    // Form Section
    QFormLayout *formLayout = new QFormLayout();
    idInput = new QLineEdit();
    nameInput = new QLineEdit();
    ageInput = new QLineEdit();
    contactInput = new QLineEdit();
    typeCombo = new QComboBox();
    typeCombo->addItems({"General", "Emergency", "Follow-Up"});

    formLayout->addRow("Patient ID:", idInput);
    formLayout->addRow("Full Name:", nameInput);
    formLayout->addRow("Age:", ageInput);
    formLayout->addRow("Contact Info:", contactInput);
    formLayout->addRow("Patient Category:", typeCombo);

    QPushButton *addBtn = new QPushButton("Register Patient");
    QPushButton *deleteBtn = new QPushButton("Delete Patient");
    QPushButton *undoBtn = new QPushButton("Undo Last Action");

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddPatientClicked);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeletePatientClicked);
    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::onUndoClicked);

    formLayout->addRow(addBtn);
    formLayout->addRow(deleteBtn);
    formLayout->addRow(undoBtn);

    // Table Section
    patientTable = new QTableWidget(0, 5);
    patientTable->setHorizontalHeaderLabels({"ID", "Name", "Age", "Contact", "Category"});
    patientTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mainLayout->addLayout(formLayout, 1);
    mainLayout->addWidget(patientTable, 2);
}

void MainWindow::setupAppointmentTab(QWidget *tab) {
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->addWidget(new QLabel("Appointment Scheduling Dashboard"));
}

void MainWindow::setupQueueTab(QWidget *tab) {
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->addWidget(new QLabel("Waiting Room Queue Management"));
}

void MainWindow::setupSearchSortTab(QWidget *tab) {
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Enter Patient ID or Name...");

    searchAlgCombo = new QComboBox();
    searchAlgCombo->addItems({"Linear Search (ID)", "Binary Search (ID)", "Recursive Search (Name)"});

    sortAlgCombo = new QComboBox();
    sortAlgCombo->addItems({"Selection Sort (Name)", "Insertion Sort (Age)", "Quick Sort (ID)"});

    QPushButton *searchBtn = new QPushButton("Search");
    QPushButton *sortBtn = new QPushButton("Sort Table");

    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(sortBtn, &QPushButton::clicked, this, &MainWindow::onSortClicked);

    controlsLayout->addWidget(searchInput);
    controlsLayout->addWidget(searchAlgCombo);
    controlsLayout->addWidget(searchBtn);
    controlsLayout->addWidget(sortAlgCombo);
    controlsLayout->addWidget(sortBtn);

    layout->addLayout(controlsLayout);
}

void MainWindow::onAddPatientClicked() {
    QString id = idInput->text();
    QString name = nameInput->text();
    int age = ageInput->text().toInt();
    QString contact = contactInput->text();
    QString type = typeCombo->currentText();

    if (id.isEmpty() || name.isEmpty() || age <= 0 || age > 120) {
        QMessageBox::warning(this, "Validation Error", "Please provide valid patient information (Age between 1-120).");
        return;
    }

    Model::PatientRecord *patient = nullptr;
    if (type == "General") {
        patient = new Model::GeneralPatient(id.toStdString(), name.toStdString(), age, contact.toStdString());
    } else if (type == "Emergency") {
        patient = new Model::EmergencyPatient(id.toStdString(), name.toStdString(), age, contact.toStdString());
    } else {
        patient = new Model::FollowUpPatient(id.toStdString(), name.toStdString(), age, contact.toStdString());
    }

    clinicManager.registerPatient(patient);
    refreshPatientTable();

    idInput->clear();
    nameInput->clear();
    ageInput->clear();
    contactInput->clear();
}

void MainWindow::onDeletePatientClicked() {
    QString id = idInput->text();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a Patient ID to delete.");
        return;
    }
    clinicManager.deletePatient(id.toStdString());
    refreshPatientTable();
}

void MainWindow::onUndoClicked() {
    clinicManager.undoLastOperation();
    refreshPatientTable();
}

void MainWindow::onSearchClicked() {
    std::string target = searchInput->text().toStdString();
    int searchType = searchAlgCombo->currentIndex();
    auto patients = clinicManager.getDatabase().getPatients();
    Model::PatientRecord *result = nullptr;

    if (searchType == 0) {
        result = Controller::SearchEngine::linearSearchByID(patients, target);
    } else if (searchType == 1) {
        result = Controller::SearchEngine::binarySearchByID(patients, target);
    } else {
        result = Controller::SearchEngine::recursiveSearchByName(patients, target);
    }

    if (result) {
        QMessageBox::information(this, "Patient Found", QString::fromStdString("Found: " + result->getName() + " (" + result->getPatientType() + ")"));
    } else {
        QMessageBox::warning(this, "Search Result", "No patient found matching query.");
    }
}

void MainWindow::onSortClicked() {
    int sortType = sortAlgCombo->currentIndex();
    auto& patients = clinicManager.getDatabase().getPatients();

    if (sortType == 0) {
        Controller::SearchEngine::selectionSortByName(patients);
    } else if (sortType == 1) {
        Controller::SearchEngine::insertionSortByAge(patients);
    } else {
        Controller::SearchEngine::quickSortByID(patients, 0, static_cast<int>(patients.size()) - 1);
    }

    refreshPatientTable();
}

void MainWindow::refreshPatientTable() {
    patientTable->setRowCount(0);
    const auto& patients = clinicManager.getDatabase().getPatients();
    for (auto p : patients) {
        int row = patientTable->rowCount();
        patientTable->insertRow(row);
        patientTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(p->getPatientID())));
        patientTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p->getName())));
        patientTable->setItem(row, 2, new QTableWidgetItem(QString::number(p->getAge())));
        patientTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p->getContactInfo())));
        patientTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(p->getPatientType())));
    }
}