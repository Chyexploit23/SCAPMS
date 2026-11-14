#pragma once

#include <QMainWindow>
#include <QDate>
#include <QVector>
#include <QPair>
#include <QString>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QListWidget>
#include <QDateEdit>
#include <QTimeEdit>
#include <QProgressBar>
#include <QPlainTextEdit>
#include "../controller/ClinicManager.h"

class DonutChart;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onAddPatientClicked();
    void onDeletePatientClicked();
    void onUndoClicked();
    void onSearchClicked();
    void onSortClicked();
    void onScheduleAppointmentClicked();
    void onCancelAppointmentClicked();
    void onEnqueuePatientClicked();
    void onCallNextClicked();
    void onNavigationClicked();
    void showFindPatient();
    void showAbout();

private:
    Controller::ClinicManager clinicManager;
    QStackedWidget *pageStack = nullptr;
    QWidget *dashboardPage = nullptr;
    QWidget *patientPage = nullptr;
    QWidget *appointmentPage = nullptr;
    QWidget *queuePage = nullptr;
    QWidget *searchPage = nullptr;
    QWidget *reportsPage = nullptr;

    QLineEdit *idInput = nullptr;
    QLineEdit *nameInput = nullptr;
    QLineEdit *ageInput = nullptr;
    QLineEdit *contactInput = nullptr;
    QComboBox *typeCombo = nullptr;
    QTableWidget *patientTable = nullptr;
    QTableWidget *searchPatientTable = nullptr;
    QTableWidget *appointmentTable = nullptr;
    QTableWidget *todayTable = nullptr;
    QLineEdit *appointmentPatientInput = nullptr;
    QDateEdit *appointmentDateInput = nullptr;
    QTimeEdit *appointmentTimeInput = nullptr;
    QLineEdit *appointmentReasonInput = nullptr;
    QLineEdit *queuePatientInput = nullptr;
    QListWidget *waitingList = nullptr;
    QLineEdit *searchInput = nullptr;
    QComboBox *searchAlgCombo = nullptr;
    QComboBox *sortAlgCombo = nullptr;
    DonutChart *demographicsChart = nullptr;
    DonutChart *appointmentStatusChart = nullptr;
    QLabel *patientCountValue = nullptr;
    QLabel *appointmentCountValue = nullptr;
    QLabel *pendingCountValue = nullptr;
    QLabel *queueCountValue = nullptr;
    QLabel *dailyActivityLabel = nullptr;
    QVector<QProgressBar *> ageBars;
    QVector<QLabel *> ageValueLabels;
    QVector<QLabel *> demographicLegendLabels;
    QVector<QLabel *> appointmentLegendLabels;
    QVector<QProgressBar *> reportBars;
    QVector<QLabel *> reportCountLabels;
    QPlainTextEdit *reportSummary = nullptr;

    void createMenuBar();
    QWidget *createSidebar();
    QWidget *createDashboardPage();
    QWidget *createPatientPage();
    QWidget *createAppointmentPage();
    QWidget *createQueuePage();
    QWidget *createSearchPage();
    QWidget *createReportsPage();
    QWidget *createInfoPage(const QString &title, const QString &description);
    void seedDashboardExampleData();
    void navigateTo(int pageIndex);
    void refreshDashboard();
    void refreshPatientTable();
    void refreshAppointmentTable();
    void refreshWaitingQueue();
    void refreshReports();
    void showError(const QString &message);
};
