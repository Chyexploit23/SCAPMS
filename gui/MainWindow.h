#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include "../controller/ClinicManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddPatientClicked();
    void onDeletePatientClicked();
    void onUndoClicked();
    void onSearchClicked();
    void onSortClicked();

private:
    Controller::ClinicManager clinicManager;

    QTabWidget *tabWidget;
    
    // Patient Controls
    QLineEdit *idInput;
    QLineEdit *nameInput;
    QLineEdit *ageInput;
    QLineEdit *contactInput;
    QComboBox *typeCombo;
    QTableWidget *patientTable;

    // Search/Sort Controls
    QLineEdit *searchInput;
    QComboBox *searchAlgCombo;
    QComboBox *sortAlgCombo;

    void setupPatientTab(QWidget *tab);
    void setupAppointmentTab(QWidget *tab);
    void setupQueueTab(QWidget *tab);
    void setupSearchSortTab(QWidget *tab);
    void refreshPatientTable();
};