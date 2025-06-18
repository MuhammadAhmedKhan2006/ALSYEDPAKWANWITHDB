#include "mainwindow.h"
#include "ownerlogin.h"
#include "salesmanlogin.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (!initializeDatabase()) {
        qDebug() << "Database initialization failed!";
        return;
    }
    setupUI();
    setTheme();
}

MainWindow::~MainWindow()
{
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    logoLayout = new QHBoxLayout();
    buttonLayout = new QVBoxLayout();

    logoLabel = createLogo();
    titleLabel = new QLabel("ALSYED PAKWAN", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 24, QFont::Bold);
    titleLabel->setFont(titleFont);

    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();

    ownerButton = new QPushButton("OWNER LOGIN", this);
    salesmanButton = new QPushButton("SALESMAN LOGIN", this);

    ownerButton->setMinimumSize(200, 60);
    salesmanButton->setMinimumSize(200, 60);

    QFont buttonFont("Arial", 14, QFont::Bold);
    ownerButton->setFont(buttonFont);
    salesmanButton->setFont(buttonFont);

    buttonLayout->addStretch();
    buttonLayout->addWidget(titleLabel);
    buttonLayout->addSpacing(50);
    buttonLayout->addWidget(ownerButton, 0, Qt::AlignCenter);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(salesmanButton, 0, Qt::AlignCenter);
    buttonLayout->addStretch();

    mainLayout->addLayout(logoLayout);
    mainLayout->addLayout(buttonLayout);

    connect(ownerButton, &QPushButton::clicked, this, &MainWindow::openOwnerLogin);
    connect(salesmanButton, &QPushButton::clicked, this, &MainWindow::openSalesmanLogin);

    setWindowTitle("Alsyed Pakwan - Restaurant Management System");
    resize(800, 600);
}

void MainWindow::setTheme()
{
    setStyleSheet(
        "QMainWindow { background-color: #1a1a1a; }"
        "QLabel { color: white; }"
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    border: 2px solid #45a049;"
        "    border-radius: 10px;"
        "    color: white;"
        "    padding: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b;"
        "}"
        );
}

QLabel* MainWindow::createLogo()
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

bool MainWindow::initializeDatabase()
{
    static int connectionCount = 0;
    QString connectionName = QString("main_window_%1").arg(connectionCount++);
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("alsyed_pakwan.db");
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "username TEXT NOT NULL UNIQUE,"
                    "password TEXT NOT NULL,"
                    "role TEXT NOT NULL)"
                    )) {
        qDebug() << "Create table error:" << query.lastError().text();
        return false;
    }
    return true;
}

void MainWindow::openOwnerLogin()
{
    OwnerLogin *ownerLogin = new OwnerLogin(this);
    ownerLogin->show();
    this->hide();
}

void MainWindow::openSalesmanLogin()
{
    SalesmanLogin *salesmanLogin = new SalesmanLogin(this);
    salesmanLogin->show();
    this->hide();
}
