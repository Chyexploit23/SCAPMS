#include "MainWindow.h"
#include "../model/GeneralPatient.h"
#include "../model/EmergencyPatient.h"
#include "../model/FollowUpPatient.h"
#include "../model/Appointment.h"
#include "../controller/SearchEngine.h"
#include <QAction>
#include <QColor>
#include <QDateEdit>
#include <QFont>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QMap>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStringList>
#include <QStatusBar>
#include <QTableWidget>
#include <QTimeEdit>
#include <QVBoxLayout>
#include <memory>
#include <numeric>
#include <QStyle>
#include <utility>

namespace {
const QString Blue = QStringLiteral("#347fe5");
const QString Green = QStringLiteral("#21a27e");
const QString Amber = QStringLiteral("#f0a33d");
const QString Purple = QStringLiteral("#9a7ae0");
const QString Ink = QStringLiteral("#26364a");
const QString Muted = QStringLiteral("#8290a2");
const QStringList ChartColors{Blue, Green, Amber, Purple};

QFrame *makeCard(QWidget *parent = nullptr)
{
    auto *card = new QFrame(parent);
    card->setObjectName("card");
    card->setFrameShape(QFrame::NoFrame);
    return card;
}

QLabel *makeLabel(const QString &text, int size = 10, bool bold = false, const QString &color = Ink)
{
    auto *label = new QLabel(text);
    label->setStyleSheet(QString("color:%1;font-size:%2px;%3").arg(color).arg(size).arg(bold ? "font-weight:700;" : ""));
    return label;
}

QPushButton *makeButton(const QString &text, bool primary = false)
{
    auto *button = new QPushButton(text);
    button->setObjectName(primary ? "primaryButton" : "secondaryButton");
    button->setCursor(Qt::PointingHandCursor);
    button->setMinimumHeight(36);
    return button;
}

QTableWidget *makeTable(const QStringList &headers)
{
    auto *table = new QTableWidget(0, headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setAlternatingRowColors(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);
    table->setFocusPolicy(Qt::NoFocus);
    table->setMinimumHeight(230);
    return table;
}

QWidget *withTitle(const QString &title, QWidget *content)
{
    auto *card = makeCard();
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(12);
    layout->addWidget(makeLabel(title, 13, true));
    layout->addWidget(content);
    return card;
}

QString statusColor(const QString &status)
{
    if (status.compare("Completed", Qt::CaseInsensitive) == 0) return Green;
    if (status.compare("Cancelled", Qt::CaseInsensitive) == 0) return Amber;
    return Blue;
}
}

class DonutChart final : public QWidget {
public:
    explicit DonutChart(QWidget *parent = nullptr) : QWidget(parent) { setMinimumSize(132, 132); setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); }
    void setData(const QVector<QPair<QString, int>> &data, const QString &centerText) { m_data = data; m_centerText = centerText; update(); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const int total = std::accumulate(m_data.cbegin(), m_data.cend(), 0, [](int sum, const auto &item) { return sum + item.second; });
        QRectF ring(10, 10, width() - 20, height() - 20);
        int angle = 90 * 16;
        for (int i = 0; i < m_data.size(); ++i) {
            const int span = total > 0 ? qRound(360.0 * 16 * m_data[i].second / total) : 0;
            QPen pen(QColor(ChartColors[i % ChartColors.size()]));
            pen.setWidth(16);
            pen.setCapStyle(Qt::FlatCap);
            painter.setPen(pen);
            painter.drawArc(ring, angle, -span);
            angle -= span;
        }
        painter.setPen(QColor(Ink));
        QFont font = painter.font(); font.setPixelSize(20); font.setWeight(QFont::DemiBold); painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, m_centerText);
    }
private:
    QVector<QPair<QString, int>> m_data;
    QString m_centerText;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("SCAPMS | Smart Clinic Management");
    setMinimumSize(1050, 700);
    resize(1480, 920);
    setStyleSheet(R"(
        QMainWindow, QWidget#appCanvas { background:#f3f6fb; color:#26364a; font-family:'Segoe UI', 'Arial'; font-size:10pt; }
        QFrame#card { background:#ffffff; border:1px solid #e3eaf2; border-radius:9px; }
        QLabel { background:transparent; }
        QLineEdit { background: white; border: 1px solid #dce4ed; border-radius: 5px; padding: 8px 9px; color: #26364a; }
        QPushButton#primaryButton { color:white; background:#286da9; border:0; border-radius:5px; padding:7px 14px; font-weight:600; }
        QPushButton#primaryButton:hover { background:#1e5b91; }
        QPushButton#secondaryButton { color:#34465e; background:#ffffff; border:1px solid #dce4ed; border-radius:5px; padding:7px 13px; }
        QPushButton#secondaryButton:hover { background:#f5f8fc; border-color:#aebed0; }
       QComboBox, QDateEdit, QTimeEdit {
    background: white;
    border: 1px solid #dce4ed;
    border-radius: 5px;
    padding: 8px 9px;
    min-height: 20px;
    color: #26364a;
}

QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QTimeEdit:focus {
    border: 1px solid #347fe5;
}

QComboBox QAbstractItemView {
    background-color: white;
    color: #26364a;
    selection-background-color: #e8f0fe;
    selection-color: #26364a;
    border: 1px solid #dce4ed;
    outline: none;
}

QComboBox QAbstractItemView::item {
    padding: 8px;
    min-height: 25px;
    color: #26364a;
}
        QTableWidget { background:white; alternate-background-color:#f8fafd; border:1px solid #e8edf3; border-radius:5px; color:#33445a; }
        QTableWidget::item { padding:7px; border:0; }
        QHeaderView::section { background:#f6f8fb; color:#718096; border:0; border-bottom:1px solid #e5ebf1; padding:10px 7px; font-weight:600; }
        QListWidget { background:white; border:1px solid #e3eaf2; border-radius:6px; padding:5px; }
        QProgressBar { background:#edf1f6; border:0; border-radius:5px; text-align:center; min-height:10px; max-height:10px; color:transparent; }
        QProgressBar::chunk { background:#21a27e; border-radius:5px; }
        QMenuBar { background:white; border-bottom:1px solid #e8edf3; padding:3px 8px; }
        QMenuBar::item { padding:6px 10px; }
        QStatusBar { background:white; color:#718096; border-top:1px solid #e3eaf2; }
        QScrollArea { border:0; background:#f3f6fb; }
        QTabWidget::pane { border:1px solid #e3eaf2; background:white; }
    )");

    seedDashboardExampleData();
    createMenuBar();

    auto *central = new QWidget(this);
    central->setObjectName("appCanvas");
    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    root->addWidget(createSidebar());

    pageStack = new QStackedWidget(central);
    pageStack->setObjectName("pageStack");
    dashboardPage = createDashboardPage();
    patientPage = createPatientPage();
    appointmentPage = createAppointmentPage();
    queuePage = createQueuePage();
    searchPage = createSearchPage();
    reportsPage = createReportsPage();
    pageStack->addWidget(dashboardPage);
    pageStack->addWidget(patientPage);
    pageStack->addWidget(appointmentPage);
    pageStack->addWidget(queuePage);
    pageStack->addWidget(searchPage);
    pageStack->addWidget(reportsPage);
    root->addWidget(pageStack, 1);
    setCentralWidget(central);

    statusBar()->showMessage("Patients: 10  |  Appointments: 14  |  Doctors: 4  |  Waiting: 0  |  Pending: 8");
    auto *findShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(findShortcut, &QShortcut::activated, this, &MainWindow::showFindPatient);
    auto *saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(saveShortcut, &QShortcut::activated, this, [this] { QMessageBox::information(this, "Save", "Persistence is not yet connected in this repository."); });
    refreshPatientTable();
    refreshAppointmentTable();
    refreshWaitingQueue();
    refreshDashboard();
    refreshReports();
}

void MainWindow::createMenuBar()
{
    auto *fileMenu = menuBar()->addMenu("&File");
    auto *saveAction = fileMenu->addAction("&Save"); saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, [this] { QMessageBox::information(this, "Save", "Persistence is not yet connected in this repository."); });
    fileMenu->addSeparator();
    auto *exitAction = fileMenu->addAction("E&xit"); exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    auto *editMenu = menuBar()->addMenu("&Edit");
    auto *undoAction = editMenu->addAction("Undo last administrative operation"); undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, &MainWindow::onUndoClicked);
    auto *reportsMenu = menuBar()->addMenu("&Reports");
    auto *analyticsAction = reportsMenu->addAction("Open analytics");
    connect(analyticsAction, &QAction::triggered, this, [this] { navigateTo(6); });
    auto *helpMenu = menuBar()->addMenu("&Help");
    auto *aboutAction = helpMenu->addAction("About SCAPMS");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

QWidget *MainWindow::createSidebar()
{
    auto *sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(194);
    sidebar->setStyleSheet("QFrame#sidebar { background:#172f52; border:0; }");
    auto *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(12, 18, 12, 12);
    layout->setSpacing(5);
    auto *brand = makeLabel("SCAPMS", 22, true, "#ffffff");
    auto *subtitle = makeLabel("SMART CLINIC SYSTEM", 8, false, "#b8c8dc");
    layout->addWidget(brand);
    layout->addWidget(subtitle);
    layout->addSpacing(20);
    const auto addSection = [layout](const QString &name) {
        auto *label = makeLabel(name, 8, true, "#93a8c2");
        label->setContentsMargins(8, 10, 0, 3);
        layout->addWidget(label);
    };
    const auto addNav = [this, layout](const QString &text, int page) {
        auto *button = new QPushButton(text);
        button->setProperty("pageIndex", page);
        button->setCursor(Qt::PointingHandCursor);
        button->setMinimumHeight(36);
        button->setStyleSheet("QPushButton { text-align:left; color:#e6edf6; background:transparent; border:0; border-radius:5px; padding:7px 9px; } QPushButton:hover { background:#24466f; } QPushButton[active=\"true\"] { background:#2b5587; border-left:3px solid #56a2f6; font-weight:600; }");
        if (page == 0) button->setProperty("active", true);
        connect(button, &QPushButton::clicked, this, &MainWindow::onNavigationClicked);
        layout->addWidget(button);
    };
    addSection("WORKSPACE");
    addNav("▦   Dashboard", 0); addNav("♙   Patient records", 1); addNav("▣   Appointments", 2); addNav("☷   Waiting queue", 3);
    addSection("INSIGHTS");
    addNav("⌕   Search & Sort", 4); addNav("▦   Statistics & Analytics", 5); addNav("⚙   Reports & Admin", 6);
    layout->addStretch();
    layout->addWidget(makeLabel("SCAPMS 1.0 · COS202 Project", 8, false, "#a9bdd5"));
    return sidebar;
}

void MainWindow::onNavigationClicked()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    if (button) navigateTo(button->property("pageIndex").toInt());
}

void MainWindow::navigateTo(int pageIndex)
{
    if (!pageStack || pageIndex < 0 || pageIndex >= pageStack->count()) return;
    pageStack->setCurrentIndex(pageIndex);
    for (auto *button : findChildren<QPushButton *>()) {
        if (button->property("pageIndex").isValid()) button->setProperty("active", button->property("pageIndex").toInt() == pageIndex);
        button->style()->unpolish(button); button->style()->polish(button);
    }
}

QWidget *MainWindow::createDashboardPage()
{
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    auto *content = new QWidget;
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(18, 14, 18, 18);
    layout->setSpacing(12);

    auto *topBar = new QHBoxLayout;

    // Capture the dashboard title label and make it bold, white, and larger
    auto *dashboardTitle = makeLabel("Dashboard", 18, true);
    dashboardTitle->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 18pt;");
    topBar->addWidget(dashboardTitle); 

    topBar->addStretch();

    auto *findButton = makeButton("⌕  Find patient"); 
    connect(findButton, &QPushButton::clicked, this, &MainWindow::showFindPatient);

    auto *userBadge = makeLabel("Clinic Administrator · Administrator", 9, true, "#617187");
    userBadge->setStyleSheet("background:#edf2f8;border-radius:15px;padding:8px 12px;color:#617187;font-size:9px;font-weight:600;");

    topBar->addWidget(findButton); 
    topBar->addWidget(userBadge);
    layout->addLayout(topBar);

    auto *welcomeRow = new QHBoxLayout;
    auto *greeting = new QVBoxLayout;

    // Capture the welcome label in a variable and set its text color to white (#FFFFFF)
    auto *welcomeLabel = makeLabel("Good day, welcome to SCAPMS", 20, true);
    welcomeLabel->setStyleSheet("color: #FFFFFF; font-size: 20px; font-weight: bold; background-color: #347fe5; padding: 10px; border-radius: 5px;"); 
    greeting->addWidget(welcomeLabel);

    greeting->addWidget(makeLabel(QDate::currentDate().toString("dddd, d MMMM yyyy"), 9, false, Muted));
    welcomeRow->addLayout(greeting); 
    welcomeRow->addStretch();

    auto *newPatient = makeButton("+  New patient", true); 
    connect(newPatient, &QPushButton::clicked, this, [this] { navigateTo(1); });

    auto *newAppointment = makeButton("+  Schedule appointment"); 
    connect(newAppointment, &QPushButton::clicked, this, [this] { navigateTo(2); });

    // (The calendarButton code has been removed here)

    auto *analyticsButton = makeButton("View analytics"); 
    connect(analyticsButton, &QPushButton::clicked, this, [this] { navigateTo(6); });

    // Add only the buttons you want to keep to the layout (omitting calendarButton)
    welcomeRow->addWidget(newPatient); 
    welcomeRow->addWidget(newAppointment); 
    welcomeRow->addWidget(analyticsButton);

    layout->addLayout(welcomeRow);
    auto *metrics = new QHBoxLayout; metrics->setSpacing(10);
    const auto addMetric = [metrics](const QString &title, const QString &value, const QString &caption, const QString &accent, QLabel **valueOut = nullptr) {
        auto *card = makeCard();
        card->setStyleSheet(QString("QFrame#card { background:white;border:1px solid #e3eaf2;border-left:3px solid %1;border-radius:8px; }").arg(accent));
        auto *v = new QVBoxLayout(card); v->setContentsMargins(13, 11, 12, 10); v->setSpacing(3);
        v->addWidget(makeLabel(title, 9, false, Muted));
        auto *number = makeLabel(value, 24, true); v->addWidget(number);
        v->addWidget(makeLabel(caption, 8, false, Muted));
        metrics->addWidget(card, 1);
        if (valueOut) *valueOut = number;
    };
    addMetric("Registered patients", "10", "All patient categories", Blue, &patientCountValue);
    addMetric("Doctors", "4", "Clinical team", "#6476b2");
    addMetric("Appointments", "14", "All-time records", Green, &appointmentCountValue);
    addMetric("Pending appointments", "8", "Awaiting consultation", Amber, &pendingCountValue);
    addMetric("Waiting queue", "0", "Patients awaiting service", Purple, &queueCountValue);
    layout->addLayout(metrics);

    auto *charts = new QHBoxLayout; charts->setSpacing(10);
    auto chartCard = [this](const QString &title, DonutChart **chartOut, QVector<QLabel *> *legendLabels) {
        auto *card = makeCard(); auto *v = new QVBoxLayout(card); v->setContentsMargins(14, 13, 14, 12); v->setSpacing(8);
        v->addWidget(makeLabel(title, 11, true));
        auto *body = new QHBoxLayout; body->setSpacing(12);
        auto *chart = new DonutChart; body->addWidget(chart, 0, Qt::AlignVCenter);
        auto *legend = new QVBoxLayout; legend->setSpacing(8);
        for (int i = 0; i < 3; ++i) {
            auto *legendLabel = makeLabel("", 9, false, "#637187");
            legend->addWidget(legendLabel);
            legendLabels->append(legendLabel);
        }
        legend->addStretch(); body->addLayout(legend, 1); v->addLayout(body);
        *chartOut = chart;
        return card;
    };
    charts->addWidget(chartCard("Patient demographics", &demographicsChart, &demographicLegendLabels), 1);
    charts->addWidget(chartCard("Appointment status", &appointmentStatusChart, &appointmentLegendLabels), 1);
    layout->addLayout(charts);

    auto *ageCard = makeCard(); auto *ageLayout = new QVBoxLayout(ageCard); ageLayout->setContentsMargins(15, 13, 15, 12); ageLayout->setSpacing(10);
    ageLayout->addWidget(makeLabel("Age distribution", 11, true));
    const QStringList ageGroups{"0–17 years", "18–35 years", "36–55 years", "56+ years"};
    for (int i = 0; i < ageGroups.size(); ++i) {
        auto *row = new QHBoxLayout; row->setSpacing(12);
        auto *group = makeLabel(ageGroups[i], 9, false, "#526278"); group->setFixedWidth(66);
        auto *bar = new QProgressBar; bar->setRange(0, 10); bar->setValue(0); bar->setTextVisible(false);
        const QString color = i == 0 ? Blue : (i == 1 ? Green : (i == 2 ? Amber : Purple));
        bar->setStyleSheet(QString("QProgressBar { background:#edf1f6;border:0;border-radius:5px;min-height:10px;max-height:10px; } QProgressBar::chunk { background:%1;border-radius:5px; }").arg(color));
        auto *count = makeLabel("0", 9, true, "#526278"); count->setAlignment(Qt::AlignRight | Qt::AlignVCenter); count->setFixedWidth(18);
        row->addWidget(group); row->addWidget(bar, 1); row->addWidget(count); ageLayout->addLayout(row);
        ageBars.append(bar); ageValueLabels.append(count);
    }
    layout->addWidget(ageCard);

    auto *activityCard = makeCard(); auto *activityLayout = new QVBoxLayout(activityCard); activityLayout->setContentsMargins(10, 9, 10, 9); activityLayout->setSpacing(4);
    activityLayout->addWidget(makeLabel("Daily activity summary", 11, true));
    dailyActivityLabel = makeLabel("", 9, false, Muted); activityLayout->addWidget(dailyActivityLabel);
    layout->addWidget(activityCard);

    auto *todayCard = makeCard(); auto *todayLayout = new QVBoxLayout(todayCard); todayLayout->setContentsMargins(10, 9, 10, 10); todayLayout->setSpacing(7);
    todayLayout->addWidget(makeLabel("Today's appointments", 11, true));
    todayTable = makeTable({"Time", "Patient", "Reason", "Status"}); todayTable->setMinimumHeight(170); todayLayout->addWidget(todayTable);
    layout->addWidget(todayCard);
    layout->addStretch();
    scroll->setWidget(content);
    return scroll;
}

QWidget *MainWindow::createPatientPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page); layout->setContentsMargins(20, 18, 20, 18); layout->setSpacing(14);
    auto *heading = new QHBoxLayout; heading->addWidget(makeLabel("Patient records", 18, true)); heading->addStretch();
    auto *newButton = makeButton("+  Register patient", true); connect(newButton, &QPushButton::clicked, this, [this] { nameInput->setFocus(); });
    heading->addWidget(newButton); layout->addLayout(heading);
    auto *body = new QHBoxLayout; body->setSpacing(12);
    auto *formCard = makeCard(); formCard->setMinimumWidth(260); formCard->setMaximumWidth(330);
    auto *form = new QVBoxLayout(formCard); form->setContentsMargins(16, 15, 16, 15); form->setSpacing(10);
    form->addWidget(makeLabel("Add patient", 12, true));
    auto *formLayout = new QFormLayout; formLayout->setSpacing(8);
    idInput = new QLineEdit; idInput->setPlaceholderText("e.g. P011");
    nameInput = new QLineEdit; nameInput->setPlaceholderText("Patient full name");
    ageInput = new QLineEdit; ageInput->setPlaceholderText("Enter age between 1–120");
    contactInput = new QLineEdit; contactInput->setPlaceholderText("Enter contact information");
    typeCombo = new QComboBox; typeCombo->addItems({"General", "Emergency", "Follow-Up"});
    formLayout->addRow("Patient ID", idInput); formLayout->addRow("Full name", nameInput); formLayout->addRow("Age", ageInput);
    formLayout->addRow("Contact", contactInput); formLayout->addRow("Category", typeCombo); form->addLayout(formLayout);
    auto *addButton = makeButton("Register patient", true); connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddPatientClicked);
    auto *deleteButton = makeButton("Delete selected ID"); connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeletePatientClicked);
    auto *undoButton = makeButton("Undo last operation"); connect(undoButton, &QPushButton::clicked, this, &MainWindow::onUndoClicked);
    form->addWidget(addButton); form->addWidget(deleteButton); form->addWidget(undoButton);
    form->addStretch(); form->addWidget(makeLabel("Patient records are kept in the ClinicDatabase vector.", 8, false, Muted));
    patientTable = makeTable({"Patient ID", "Patient name", "Age", "Contact", "Category"});
    auto *tableCard = withTitle("Registered patients", patientTable);
    body->addWidget(formCard); body->addWidget(tableCard, 1); layout->addLayout(body, 1);
    return page;
}

QWidget *MainWindow::createAppointmentPage()
{
    auto *page = new QWidget; auto *layout = new QVBoxLayout(page); layout->setContentsMargins(20, 18, 20, 18); layout->setSpacing(14);
    auto *heading = new QHBoxLayout; heading->addWidget(makeLabel("Appointments", 18, true)); heading->addStretch();
    auto *formCard = makeCard(); auto *form = new QHBoxLayout(formCard); form->setContentsMargins(12, 10, 12, 10); form->setSpacing(8);
    appointmentPatientInput = new QLineEdit; appointmentPatientInput->setPlaceholderText("Enter Patient ID");
    appointmentDateInput = new QDateEdit(QDate::currentDate()); appointmentDateInput->setCalendarPopup(true); appointmentDateInput->setDisplayFormat("yyyy-MM-dd");
    appointmentTimeInput = new QTimeEdit(QTime::currentTime()); appointmentTimeInput->setDisplayFormat("HH:mm");
    appointmentReasonInput = new QLineEdit; appointmentReasonInput->setPlaceholderText("Reason for visit");
    auto *scheduleButton = makeButton("+  Schedule appointment", true); connect(scheduleButton, &QPushButton::clicked, this, &MainWindow::onScheduleAppointmentClicked);
    form->addWidget(appointmentPatientInput, 1); form->addWidget(appointmentDateInput); form->addWidget(appointmentTimeInput); form->addWidget(appointmentReasonInput, 2); form->addWidget(scheduleButton);
    heading->addWidget(makeLabel("Create an appointment", 10, false, Muted)); layout->addLayout(heading); layout->addWidget(formCard);
    appointmentTable = makeTable({"Appointment ID", "Patient ID", "Date", "Time", "Reason", "Status"});
    layout->addWidget(withTitle("Appointment schedule", appointmentTable), 1);
    auto *cancelButton = makeButton("Cancel selected appointment"); connect(cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelAppointmentClicked);
    layout->addWidget(cancelButton, 0, Qt::AlignRight);
    return page;
}

QWidget *MainWindow::createQueuePage()
{
    auto *page = new QWidget; auto *layout = new QVBoxLayout(page); layout->setContentsMargins(20, 18, 20, 18); layout->setSpacing(14);
    layout->addWidget(makeLabel("Waiting queue", 18, true));
    auto *controls = makeCard(); auto *row = new QHBoxLayout(controls); row->setContentsMargins(12, 10, 12, 10);
    queuePatientInput = new QLineEdit; queuePatientInput->setPlaceholderText("Patient ID to add to queue");
    auto *enqueueButton = makeButton("Add to waiting queue", true); connect(enqueueButton, &QPushButton::clicked, this, &MainWindow::onEnqueuePatientClicked);
    auto *callButton = makeButton("Call next patient"); connect(callButton, &QPushButton::clicked, this, &MainWindow::onCallNextClicked);
    row->addWidget(queuePatientInput, 1); row->addWidget(enqueueButton); row->addWidget(callButton); layout->addWidget(controls);
    waitingList = new QListWidget; layout->addWidget(withTitle("Patients waiting for consultation · first in, first served", waitingList), 1);
    return page;
}

QWidget *MainWindow::createSearchPage()
{
    auto *page = new QWidget; auto *layout = new QVBoxLayout(page); layout->setContentsMargins(20, 18, 20, 18); layout->setSpacing(14);
    layout->addWidget(makeLabel("Search & sort", 18, true));
    auto *card = makeCard(); auto *form = new QGridLayout(card); form->setContentsMargins(16, 15, 16, 15); form->setHorizontalSpacing(10); form->setVerticalSpacing(9);
    form->addWidget(makeLabel("Search records", 11, true), 0, 0, 1, 4);
    searchInput = new QLineEdit; searchInput->setPlaceholderText("Enter patient ID or exact name");
    searchAlgCombo = new QComboBox; searchAlgCombo->addItems({"Linear search · Patient ID", "Binary search · Patient ID", "Recursive search · Name"});
    auto *searchButton = makeButton("Search", true); connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    form->addWidget(searchInput, 1, 0, 1, 2); form->addWidget(searchAlgCombo, 1, 2); form->addWidget(searchButton, 1, 3);
    form->addWidget(makeLabel("Sort records", 11, true), 2, 0, 1, 4);
    sortAlgCombo = new QComboBox; sortAlgCombo->addItems({"Selection sort · Name", "Insertion sort · Age", "Quick sort · ID"});
    auto *sortButton = makeButton("Sort patient list"); connect(sortButton, &QPushButton::clicked, this, &MainWindow::onSortClicked);
    form->addWidget(sortAlgCombo, 3, 0, 1, 2); form->addWidget(sortButton, 3, 2);
    form->addWidget(makeLabel("Selection sort is transparent and useful for small clinic lists; insertion sort performs well on nearly-sorted age lists; quick sort efficiently orders IDs on average.", 9, false, Muted), 4, 0, 1, 4);
    layout->addWidget(card);
    searchPatientTable = makeTable({"Patient ID", "Patient name", "Age", "Contact", "Category"});
    layout->addWidget(withTitle("Patient records", searchPatientTable), 1);
    return page;
}

QWidget *MainWindow::createReportsPage()
{
    auto *page = new QWidget; auto *layout = new QVBoxLayout(page); layout->setContentsMargins(20, 18, 20, 18); layout->setSpacing(14);
    layout->addWidget(makeLabel("Statistics & reports", 18, true));
    auto *distribution = makeCard(); auto *v = new QVBoxLayout(distribution); v->setContentsMargins(16, 14, 16, 14); v->setSpacing(9);
    v->addWidget(makeLabel("Patient category distribution", 12, true));
    const QStringList categories{"General", "Emergency", "Follow-Up"};
    for (int i = 0; i < categories.size(); ++i) {
        auto *row = new QHBoxLayout; auto *name = makeLabel(categories[i], 9, false, "#526278"); name->setFixedWidth(90);
        auto *bar = new QProgressBar; bar->setRange(0, 100); bar->setTextVisible(false);
        bar->setStyleSheet(QString("QProgressBar { background:#edf1f6;border:0;border-radius:5px;min-height:12px;max-height:12px; } QProgressBar::chunk { background:%1;border-radius:5px; }").arg(ChartColors[i]));
        auto *count = makeLabel("0", 9, true); count->setFixedWidth(30); count->setAlignment(Qt::AlignRight);
        row->addWidget(name); row->addWidget(bar, 1); row->addWidget(count); v->addLayout(row); reportBars.append(bar); reportCountLabels.append(count);
    }
    layout->addWidget(distribution);
    reportSummary = new QPlainTextEdit; reportSummary->setReadOnly(true); reportSummary->setMinimumHeight(220);
    layout->addWidget(withTitle("Daily appointment summary & pending appointments", reportSummary), 1);
    refreshReports();
    return page;
}

QWidget *MainWindow::createInfoPage(const QString &title, const QString &description)
{
    auto *page = new QWidget; auto *layout = new QVBoxLayout(page); layout->setContentsMargins(24, 24, 24, 24);
    auto *card = makeCard(); auto *cardLayout = new QVBoxLayout(card); cardLayout->setContentsMargins(22, 22, 22, 22); cardLayout->setSpacing(10);
    cardLayout->addWidget(makeLabel(title, 18, true)); cardLayout->addWidget(makeLabel(description, 10, false, Muted)); cardLayout->addStretch();
    layout->addWidget(card, 1); return page;
}

void MainWindow::seedDashboardExampleData()
{
    auto &database = clinicManager.getDatabase();
    if (!database.getPatients().empty()) return;
    const QStringList names{"John Oke", "Issac Stone", "Mary Owusu", "Ethan Bola", "Zahara Leo", "Joy Addo", "Abraham Dike", "Jayden Okeke", "Love Arthur", "Kate Samuel"};
    const QList<int> ages{24, 31, 42, 58, 27, 39, 63, 18, 48, 33};
    const QStringList types{"General", "General", "General", "General", "General", "Emergency", "Emergency", "Emergency", "Follow-Up", "Follow-Up"};
    for (int i = 0; i < names.size(); ++i) {
        const std::string id = QString("P%1").arg(i + 1, 3, 10, QLatin1Char('0')).toStdString();
        const std::string name = names[i].toStdString();
        const std::string contact = QString("+234 20 555 %1").arg(101 + i).toStdString();
        if (types[i] == "General") database.addPatient(new Model::GeneralPatient(id, name, ages[i], contact));
        else if (types[i] == "Emergency") database.addPatient(new Model::EmergencyPatient(id, name, ages[i], contact));
        else database.addPatient(new Model::FollowUpPatient(id, name, ages[i], contact));
    }
    const QDate today = QDate::currentDate();
    const QStringList statuses{"Completed", "Pending", "Completed", "Completed", "Completed", "Completed", "Pending", "Pending", "Pending", "Pending", "Pending", "Pending", "Pending", "Cancelled"};
    const QStringList reasons{"General check-up", "Blood pressure review", "Follow-up visit", "Consultation", "Wellness screening", "Routine check-up", "Medication review", "New patient visit", "Follow-up consultation", "Lab results", "Health screening", "Consultation", "Review appointment", "Rescheduled visit"};
    for (int i = 0; i < statuses.size(); ++i) {
        const QString appointmentDate = today.addDays(i < 2 ? 0 : (i % 6) - 2).toString("yyyy-MM-dd");
        const QString appointmentTime = QTime(8, 30).addSecs(i * 30 * 60).toString("HH:mm");
        database.addAppointment(Model::Appointment(QString("A%1").arg(i + 1, 3, 10, QLatin1Char('0')).toStdString(),
            QString("P%1").arg((i % 10) + 1, 3, 10, QLatin1Char('0')).toStdString(), appointmentDate.toStdString(), appointmentTime.toStdString(), reasons[i].toStdString(), statuses[i].toStdString()));
    }
}

void MainWindow::onAddPatientClicked()
{
    QString id = idInput->text().trimmed();
    QString rawName = nameInput->text().trimmed();
    bool ageValid = false;
    int age = ageInput->text().toInt(&ageValid);
    QString contact = contactInput->text().trimmed();

    // Validate ID Format (e.g., must start with 'P' followed by numbers, or match your exact pattern)
    QRegularExpression idRegex("^P\\d+$", QRegularExpression::CaseInsensitiveOption);
    if (!idRegex.match(id).hasMatch()) {
        showError("Patient ID must start with 'P' followed by numbers (e.g., P011).");
        return;
    }

    // Validate Contact: Must be exactly 11 digits (or match a specific phone layout)
    // Strip out non-digit characters to check the length properly
    QString digitsOnly = contact;
    digitsOnly.remove(QRegularExpression("\\D")); // removes spaces, plus signs, etc. for counting
    if (digitsOnly.length() != 11) {
        showError("Contact number must be exactly 11 digits.");
        return;
    }

    // Format Name to Title Case (Capitalize first letter of each word, rest lowercase)
    QStringList nameParts = rawName.split(' ', Qt::SkipEmptyParts);
    QString formattedName = "";
    for (QString part : nameParts) {
        if (!part.isEmpty()) {
            part = part.toLower();
            part[0] = part[0].toUpper();
            if (!formattedName.isEmpty()) formattedName += " ";
            formattedName += part;
        }
    }

    if (id.isEmpty() || formattedName.isEmpty() || !ageValid || age < 1 || age > 120) {
        showError("Enter a valid patient ID, name, and an age between 1 and 120.");
        return;
    }

    if (clinicManager.getDatabase().findPatientByID(id.toStdString())) {
        showError("That patient ID is already registered.");
        return;
    }

    try {
        std::unique_ptr<Model::PatientRecord> patient;
        std::string stdId = id.toStdString();
        std::string stdName = formattedName.toStdString();
        std::string stdContact = contact.toStdString();

        if (typeCombo->currentText() == "General") 
            patient = std::make_unique<Model::GeneralPatient>(stdId, stdName, age, stdContact);
        else if (typeCombo->currentText() == "Emergency") 
            patient = std::make_unique<Model::EmergencyPatient>(stdId, stdName, age, stdContact);
        else 
            patient = std::make_unique<Model::FollowUpPatient>(stdId, stdName, age, stdContact);

        clinicManager.registerPatient(patient.release());
        
        // Clear input fields after successful registration
        idInput->clear(); 
        nameInput->clear(); 
        ageInput->clear(); 
        contactInput->clear();
        
        refreshPatientTable(); 
        refreshDashboard(); 
        refreshReports();
    } catch (const std::exception &exception) {
        showError(QString::fromUtf8(exception.what()));
    }
}
void MainWindow::onDeletePatientClicked()
{
    QString id = idInput->text().trimmed();
    if (id.isEmpty() && patientTable->currentRow() >= 0) id = patientTable->item(patientTable->currentRow(), 0)->text();
    if (id.isEmpty()) { showError("Select a patient row or enter the patient's ID before deleting."); return; }
    if (!clinicManager.getDatabase().findPatientByID(id.toStdString())) { showError("No patient with that ID was found."); return; }
    clinicManager.deletePatient(id.toStdString());
    refreshPatientTable(); refreshDashboard(); refreshReports();
}

void MainWindow::onUndoClicked()
{
    clinicManager.undoLastOperation();
    refreshPatientTable(); refreshDashboard(); refreshReports(); refreshWaitingQueue();
}

void MainWindow::onSearchClicked()
{
    const std::string target = searchInput->text().trimmed().toStdString();
    if (target.empty()) { showError("Enter a patient ID or name to search."); return; }
    const auto &patients = clinicManager.getDatabase().getPatients();
    Model::PatientRecord *result = nullptr;
    if (searchAlgCombo->currentIndex() == 0) result = Controller::SearchEngine::linearSearchByID(patients, target);
    else if (searchAlgCombo->currentIndex() == 1) result = Controller::SearchEngine::binarySearchByID(patients, target);
    else result = Controller::SearchEngine::recursiveSearchByName(patients, target);
    if (result) {
        QMessageBox::information(this, "Patient found", QString("%1 · %2 · age %3 · %4").arg(QString::fromStdString(result->getName()), QString::fromStdString(result->getPatientID())).arg(result->getAge()).arg(QString::fromStdString(result->getPatientType())));
    } else QMessageBox::information(this, "Search result", "No matching patient was found.");
}

void MainWindow::onSortClicked()
{
    auto &patients = clinicManager.getDatabase().getPatients();
    if (sortAlgCombo->currentIndex() == 0) Controller::SearchEngine::selectionSortByName(patients);
    else if (sortAlgCombo->currentIndex() == 1) Controller::SearchEngine::insertionSortByAge(patients);
    else Controller::SearchEngine::quickSortByID(patients, 0, static_cast<int>(patients.size()) - 1);
    refreshPatientTable();
}

void MainWindow::onScheduleAppointmentClicked()
{
    const QString patientId = appointmentPatientInput->text().trimmed();
    if (!clinicManager.getDatabase().findPatientByID(patientId.toStdString())) { showError("Register or select a valid patient ID before scheduling."); return; }
    if (appointmentReasonInput->text().trimmed().isEmpty()) { showError("Enter a reason for the appointment."); return; }
    try {
        const auto &appointments = clinicManager.getDatabase().getAppointments();
        const QString id = QString("A%1").arg(static_cast<qulonglong>(appointments.size() + 1), 3, 10, QLatin1Char('0'));
        clinicManager.getDatabase().addAppointment(Model::Appointment(id.toStdString(), patientId.toStdString(),
            appointmentDateInput->date().toString("yyyy-MM-dd").toStdString(), appointmentTimeInput->time().toString("HH:mm").toStdString(),
            appointmentReasonInput->text().trimmed().toStdString(), "Pending"));
        appointmentPatientInput->clear(); appointmentReasonInput->clear();
        refreshAppointmentTable(); refreshDashboard(); refreshReports();
    } catch (const std::exception &exception) { showError(QString::fromUtf8(exception.what())); }
}

void MainWindow::onCancelAppointmentClicked()
{
    if (appointmentTable->currentRow() < 0) { showError("Select an appointment to cancel."); return; }
    const QString appointmentId = appointmentTable->item(appointmentTable->currentRow(), 0)->text();
    clinicManager.getDatabase().cancelAppointment(appointmentId.toStdString());
    refreshAppointmentTable(); refreshDashboard(); refreshReports();
}

void MainWindow::onEnqueuePatientClicked()
{
    const QString id = queuePatientInput->text().trimmed();
    Model::PatientRecord *patient = clinicManager.getDatabase().findPatientByID(id.toStdString());
    if (!patient) { showError("Enter a registered patient ID."); return; }
    clinicManager.enqueuePatient(patient); queuePatientInput->clear();
    refreshWaitingQueue(); refreshDashboard();
}

void MainWindow::onCallNextClicked()
{
    Model::PatientRecord *patient = clinicManager.dequeuePatient();
    if (!patient) { QMessageBox::information(this, "Waiting queue", "There are no patients waiting."); return; }
    QMessageBox::information(this, "Now serving", QString("Please call %1 (%2) for consultation.").arg(QString::fromStdString(patient->getName()), QString::fromStdString(patient->getPatientID())));
    refreshWaitingQueue(); refreshDashboard();
}

void MainWindow::showFindPatient()
{
    bool ok = false;
    const QString id = QInputDialog::getText(this, "Find patient", "Patient ID", QLineEdit::Normal, {}, &ok);
    if (!ok || id.trimmed().isEmpty()) return;
    auto *patient = clinicManager.getDatabase().findPatientByID(id.trimmed().toStdString());
    if (!patient) QMessageBox::information(this, "Patient not found", "No patient matches that ID.");
    else QMessageBox::information(this, "Patient found", QString("%1 · %2 · %3").arg(QString::fromStdString(patient->getPatientID()), QString::fromStdString(patient->getName()), QString::fromStdString(patient->getPatientType())));
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About SCAPMS", "Smart Clinic Appointment & Patient Management System\nQt Widgets desktop dashboard · COS202");
}

void MainWindow::refreshPatientTable()
{
    const auto &patients = clinicManager.getDatabase().getPatients();
    for (auto *table : {patientTable, searchPatientTable}) {
        if (!table) continue;
        table->setRowCount(0);
        for (auto *patient : patients) {
            const int row = table->rowCount(); table->insertRow(row);
            const QStringList values{QString::fromStdString(patient->getPatientID()), QString::fromStdString(patient->getName()), QString::number(patient->getAge()), QString::fromStdString(patient->getContactInfo()), QString::fromStdString(patient->getPatientType())};
            for (int column = 0; column < values.size(); ++column) table->setItem(row, column, new QTableWidgetItem(values[column]));
            const QString type = QString::fromStdString(patient->getPatientType());
            const QColor tint = type == "Emergency" ? QColor("#fff2ec") : (type == "Follow-Up" ? QColor("#f4efff") : QColor("#ebf4ff"));
            table->item(row, 4)->setBackground(tint);
            table->item(row, 4)->setForeground(QColor(type == "Emergency" ? "#c66b36" : (type == "Follow-Up" ? "#795bc0" : "#3471bd")));
        }
    }
}

void MainWindow::refreshAppointmentTable()
{
    auto fill = [this](QTableWidget *table, bool todayOnly) {
        if (!table) return;
        table->setRowCount(0);
        const auto &appointments = clinicManager.getDatabase().getAppointments();
        for (const auto &appointment : appointments) {
            if (todayOnly && QString::fromStdString(appointment.getAppointmentDate()) != QDate::currentDate().toString("yyyy-MM-dd")) continue;
            const int row = table->rowCount(); table->insertRow(row);
            const auto patientId = QString::fromStdString(appointment.getPatientID());
            const auto *patient = clinicManager.getDatabase().findPatientByID(patientId.toStdString());
            const QString patientName = patient ? QString::fromStdString(patient->getName()) : patientId;
            const QStringList values{QString::fromStdString(appointment.getAppointmentID()), patientId, QString::fromStdString(appointment.getAppointmentDate()), QString::fromStdString(appointment.getAppointmentTime()), QString::fromStdString(appointment.getReason()), QString::fromStdString(appointment.getStatus())};
            if (todayOnly) {
                table->setItem(row, 0, new QTableWidgetItem(values[3])); table->setItem(row, 1, new QTableWidgetItem(patientName));
                table->setItem(row, 2, new QTableWidgetItem(values[4])); table->setItem(row, 3, new QTableWidgetItem(values[5]));
                table->item(row, 3)->setForeground(QColor(statusColor(values[5])));
            } else {
                for (int column = 0; column < values.size(); ++column) table->setItem(row, column, new QTableWidgetItem(values[column]));
                table->item(row, 5)->setForeground(QColor(statusColor(values[5])));
            }
        }
    };
    fill(appointmentTable, false); fill(todayTable, true);
}

void MainWindow::refreshWaitingQueue()
{
    if (!waitingList) return;
    waitingList->clear();
    auto queue = clinicManager.getWaitingQueue();
    int position = 1;
    while (!queue.empty()) {
        auto *patient = queue.front(); queue.pop();
        waitingList->addItem(QString("%1     %2     ·     %3").arg(position++, 2, 10, QLatin1Char('0')).arg(QString::fromStdString(patient->getPatientID()), QString::fromStdString(patient->getName())));
    }
    if (queueCountValue) queueCountValue->setText(QString::number(position - 1));
}

void MainWindow::refreshDashboard()
{
    if (!demographicsChart) return;
    const auto &patients = clinicManager.getDatabase().getPatients();
    const auto &appointments = clinicManager.getDatabase().getAppointments();
    int general = 0, emergency = 0, followUp = 0;
    int age0to17 = 0, age18to35 = 0, age36to55 = 0, age56plus = 0;
    for (const auto *patient : patients) {
        const auto type = patient->getPatientType();
        if (type == "General") ++general; else if (type == "Emergency") ++emergency; else ++followUp;
        if (patient->getAge() <= 17) ++age0to17; else if (patient->getAge() <= 35) ++age18to35; else if (patient->getAge() <= 55) ++age36to55; else ++age56plus;
    }
    int completed = 0, pending = 0, cancelled = 0, todayTotal = 0, todayCompleted = 0, todayPending = 0, todayCancelled = 0;
    for (const auto &appointment : appointments) {
        const QString status = QString::fromStdString(appointment.getStatus());
        if (status == "Completed") ++completed; else if (status == "Cancelled") ++cancelled; else ++pending;
        if (QString::fromStdString(appointment.getAppointmentDate()) == QDate::currentDate().toString("yyyy-MM-dd")) {
            ++todayTotal;
            if (status == "Completed") ++todayCompleted; else if (status == "Cancelled") ++todayCancelled; else ++todayPending;
        }
    }
    demographicsChart->setData({{"General", general}, {"Emergency", emergency}, {"Follow-Up", followUp}}, QString::number(static_cast<qulonglong>(patients.size())));
    appointmentStatusChart->setData({{"Completed", completed}, {"Pending", pending}, {"Cancelled", cancelled}}, QString::number(static_cast<qulonglong>(appointments.size())));
    const QStringList demographics{QString("■  General  %1").arg(general), QString("■  Emergency  %1").arg(emergency), QString("■  Follow-Up  %1").arg(followUp)};
    const QStringList statuses{QString("■  Completed  %1").arg(completed), QString("■  Pending  %1").arg(pending), QString("■  Cancelled  %1").arg(cancelled)};
    for (int i = 0; i < demographicLegendLabels.size(); ++i) {
        demographicLegendLabels[i]->setText(demographics[i]);
        demographicLegendLabels[i]->setStyleSheet(QString("color:%1;font-size:9px;").arg(ChartColors[i]));
    }
    for (int i = 0; i < appointmentLegendLabels.size(); ++i) {
        appointmentLegendLabels[i]->setText(statuses[i]);
        appointmentLegendLabels[i]->setStyleSheet(QString("color:%1;font-size:9px;").arg(ChartColors[i]));
    }
    const QList<int> ageCounts{age0to17, age18to35, age36to55, age56plus};
    for (int i = 0; i < ageBars.size(); ++i) { ageBars[i]->setRange(0, qMax(1, static_cast<int>(patients.size()))); ageBars[i]->setValue(ageCounts[i]); ageValueLabels[i]->setText(QString::number(ageCounts[i])); }
    if (patientCountValue) patientCountValue->setText(QString::number(static_cast<qulonglong>(patients.size())));
    if (appointmentCountValue) appointmentCountValue->setText(QString::number(static_cast<qulonglong>(appointments.size())));
    if (pendingCountValue) pendingCountValue->setText(QString::number(pending));
    if (dailyActivityLabel) dailyActivityLabel->setText(QString("Today (%1): %2 appointment(s) scheduled · %3 completed · %4 pending · %5 cancelled.").arg(QDate::currentDate().toString("yyyy-MM-dd")).arg(todayTotal).arg(todayCompleted).arg(todayPending).arg(todayCancelled));
    const auto queueSize = clinicManager.getWaitingQueue().size();
    statusBar()->showMessage(QString("Patients: %1  |  Appointments: %2  |  Doctors: 4  |  Waiting: %3  |  Pending: %4")
        .arg(static_cast<qulonglong>(patients.size())).arg(static_cast<qulonglong>(appointments.size())).arg(static_cast<qulonglong>(queueSize)).arg(pending));
    refreshAppointmentTable();
    refreshReports();
}

void MainWindow::refreshReports()
{
    if (!reportSummary) return;
    const auto &patients = clinicManager.getDatabase().getPatients();
    const auto &appointments = clinicManager.getDatabase().getAppointments();
    QMap<QString, int> categoryCounts{{"General", 0}, {"Emergency", 0}, {"Follow-Up", 0}};
    QMap<std::string, int> completedVisits;
    QStringList pendingLines;
    int completed = 0, pending = 0, cancelled = 0;
    for (const auto *patient : patients) ++categoryCounts[QString::fromStdString(patient->getPatientType())];
    for (const auto &appointment : appointments) {
        const QString status = QString::fromStdString(appointment.getStatus());
        if (status == "Completed") { ++completed; ++completedVisits[appointment.getPatientID()]; }
        else if (status == "Cancelled") ++cancelled;
        else {
            ++pending;
            const auto *patient = clinicManager.getDatabase().findPatientByID(appointment.getPatientID());
            pendingLines.append(QString("• %1 · %2 · %3 %4 — %5").arg(patient ? QString::fromStdString(patient->getName()) : QString::fromStdString(appointment.getPatientID()), QString::fromStdString(appointment.getAppointmentDate()), QString::fromStdString(appointment.getAppointmentTime()), QString::fromStdString(appointment.getReason()), QString::fromStdString(appointment.getStatus())));
        }
    }
    const int total = qMax(1, static_cast<int>(patients.size()));
    for (int i = 0; i < reportBars.size(); ++i) {
        const QString category = QStringList{"General", "Emergency", "Follow-Up"}[i];
        reportBars[i]->setValue(qRound(100.0 * categoryCounts.value(category) / total));
        reportCountLabels[i]->setText(QString::number(categoryCounts.value(category)));
    }
    int mostVisits = 0; for (auto it = completedVisits.cbegin(); it != completedVisits.cend(); ++it) mostVisits = qMax(mostVisits, it.value());
    QStringList frequentLines;
    if (mostVisits > 0) for (auto it = completedVisits.cbegin(); it != completedVisits.cend(); ++it) if (it.value() == mostVisits) {
        const auto *patient = clinicManager.getDatabase().findPatientByID(it.key());
        frequentLines.append(QString("• %1 (%2 completed visits)").arg(patient ? QString::fromStdString(patient->getName()) : QString::fromStdString(it.key())).arg(mostVisits));
    }
    if (frequentLines.isEmpty()) frequentLines << "• No completed appointments recorded yet.";
    if (pendingLines.isEmpty()) pendingLines << "• No pending appointments.";
    QString text = QString("Daily appointment summary\nAppointments: %1    Completed: %2    Pending: %3    Cancelled: %4\n\nMost frequently attended patients\n%5\n\nPatients with pending appointments\n%6\n\nPatient category distribution\nGeneral: %7    Emergency: %8    Follow-Up: %9")
        .arg(static_cast<qulonglong>(appointments.size())).arg(completed).arg(pending).arg(cancelled).arg(frequentLines.join('\n')).arg(pendingLines.join('\n'))
        .arg(categoryCounts.value("General")).arg(categoryCounts.value("Emergency")).arg(categoryCounts.value("Follow-Up"));
    reportSummary->setPlainText(text);
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::warning(this, "SCAPMS", message);
}
