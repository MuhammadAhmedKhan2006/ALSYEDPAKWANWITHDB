#include "ownerdashboard.h"
#include "inventory.h"
#include "employeesalary.h"
#include "salesmandashboard.h"
#include "mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QtGlobal>

OwnerDashboard::OwnerDashboard(QWidget *parent)
    : QMainWindow(parent)
{
    if (!initializeDatabase()) {
        qDebug() << "Database initialization failed!";
        return;
    }
    setupUI();
    setTheme();
}

OwnerDashboard::~OwnerDashboard()
{
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void OwnerDashboard::doOwnerDashboard()
{
    setupUI();
    setTheme();
}

void OwnerDashboard::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    logoLayout = new QHBoxLayout();
    buttonLayout = new QVBoxLayout();

    logoLabel = createLogo();
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();

    titleLabel = new QLabel("ALSYED PAKWAN", this); // Use titleLabel
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 24, QFont::Bold);
    titleLabel->setFont(titleFont);

    welcomeLabel = new QLabel("OWNER DASHBOARD", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont welcomeFont("Arial", 18, QFont::Bold);
    welcomeLabel->setFont(welcomeFont);

    inventoryButton = new QPushButton("INVENTORY MANAGEMENT", this);
    salaryButton = new QPushButton("EMPLOYEE SALARY", this);
    salesmanButton = new QPushButton("SALESMAN DASHBOARD", this);
    logoutButton = new QPushButton("LOGOUT", this);

    inventoryButton->setMinimumSize(300, 80);
    salaryButton->setMinimumSize(300, 80);
    salesmanButton->setMinimumSize(300, 80);
    logoutButton->setMinimumSize(150, 50);

    QFont buttonFont("Arial", 14, QFont::Bold);
    inventoryButton->setFont(buttonFont);
    salaryButton->setFont(buttonFont);
    salesmanButton->setFont(buttonFont);
    logoutButton->setFont(buttonFont);

    buttonLayout->addStretch();
    buttonLayout->addWidget(titleLabel); // Use titleLabel
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(welcomeLabel);
    buttonLayout->addSpacing(50);
    buttonLayout->addWidget(inventoryButton, 0, Qt::AlignCenter);
    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(salaryButton, 0, Qt::AlignCenter);
    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(salesmanButton, 0, Qt::AlignCenter);
    buttonLayout->addSpacing(40);
    buttonLayout->addWidget(logoutButton, 0, Qt::AlignCenter);
    buttonLayout->addStretch();

    mainLayout->addLayout(logoLayout);
    mainLayout->addLayout(buttonLayout);

    connect(inventoryButton, &QPushButton::clicked, this, &OwnerDashboard::openInventory);
    connect(salaryButton, &QPushButton::clicked, this, &OwnerDashboard::openEmployeeSalary);
    connect(salesmanButton, &QPushButton::clicked, this, &OwnerDashboard::openSalesman);
    connect(logoutButton, &QPushButton::clicked, this, &OwnerDashboard::logout);

    setWindowTitle("Owner Dashboard - Alsyed Pakwan");
    resize(800, 600);
}

void OwnerDashboard::setTheme()
{
    setStyleSheet(
        "QMainWindow { background-color: #1a1a1a; }"
        "QLabel { color: white; }"
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    border: 2px solid #45a049;"
        "    border-radius: 15px;"
        "    color: white;"
        "    padding: 15px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
        );
}

QLabel* OwnerDashboard::createLogo()
{
    QLabel* logo = new QLabel(this);
    logo->setFixedSize(80, 80);
    logo->setStyleSheet(
        "QLabel {"
        "    background-color: #4CAF50;"
        "    border: 3px solid white;"
        "    border-radius: 40px;"
        "    color: white;"
        "    font-weight: bold;"
        "    font-size: 16px;"
        "}"
        );
    logo->setText("AP");
    logo->setAlignment(Qt::AlignCenter);
    return logo;
}

bool OwnerDashboard::initializeDatabase()
{
    static int connectionCount = 0;
    QString connectionName = QString("owner_dashboard_%1").arg(connectionCount++);
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("alsyed_pakwan.db");
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }
    return true;
}

void OwnerDashboard::openInventory()
{
    Inventory *inventory = new Inventory(this);
    inventory->show();
    this->hide();
}

void OwnerDashboard::openEmployeeSalary()
{
    EmployeeSalary *employeeSalary = new EmployeeSalary(this);
    employeeSalary->show();
    this->hide();
}

void OwnerDashboard::openSalesman()
{
    SalesmanDashboard *salesmanDashboard = new SalesmanDashboard(this);
    salesmanDashboard->show();
    this->hide();
}

void OwnerDashboard::logout()
{
    MainWindow *mainWindow = new MainWindow(this->parentWidget());
    mainWindow->show();
    this->close();
}
