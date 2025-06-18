#include "salesmanlogin.h"
#include "salesmandashboard.h"
#include "mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QtGlobal>
#include <QMessageBox>

SalesmanLogin::SalesmanLogin(QWidget *parent)
    : QDialog(parent)
{
    if (!initializeDatabase()) {
        qDebug() << "Database initialization failed!";
        QMessageBox::critical(this, "Error", "Failed to initialize database!");
        return;
    }
    setupUI();
    setTheme();
}

SalesmanLogin::~SalesmanLogin()
{
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void SalesmanLogin::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    logoLayout = new QHBoxLayout();
    formLayout = new QVBoxLayout();

    logoLabel = createLogo();
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();

    titleLabel = new QLabel("SALESMAN LOGIN", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 20, QFont::Bold);
    titleLabel->setFont(titleFont);

    emailLabel = new QLabel("Email:", this);      // Initialize
    emailEdit = new QLineEdit(this);              // Initialize
    passwordLabel = new QLabel("Password:", this); // Initialize
    passwordEdit = new QLineEdit(this);           // Initialize
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("LOGIN", this);
    backButton = new QPushButton("BACK", this);

    emailEdit->setMinimumHeight(40);
    passwordEdit->setMinimumHeight(40);
    loginButton->setMinimumHeight(50);
    backButton->setMinimumHeight(40);

    QFont labelFont("Arial", 12, QFont::Bold);
    emailLabel->setFont(labelFont);
    passwordLabel->setFont(labelFont);

    formLayout->addStretch();
    formLayout->addWidget(titleLabel);
    formLayout->addSpacing(30);
    formLayout->addWidget(emailLabel);
    formLayout->addWidget(emailEdit);
    formLayout->addSpacing(15);
    formLayout->addWidget(passwordLabel);
    formLayout->addWidget(passwordEdit);
    formLayout->addSpacing(30);
    formLayout->addWidget(loginButton);
    formLayout->addSpacing(10);
    formLayout->addWidget(backButton);
    formLayout->addStretch();

    mainLayout->addLayout(logoLayout);
    mainLayout->addLayout(formLayout);

    connect(loginButton, &QPushButton::clicked, this, &SalesmanLogin::login);
    connect(backButton, &QPushButton::clicked, this, &SalesmanLogin::goBack);

    setWindowTitle("Salesman Login - Alsyed Pakwan");
    resize(400, 500);
}

void SalesmanLogin::setTheme()
{
    setStyleSheet(
        "QDialog { background-color: #1a1a1a; }"
        "QLabel { color: white; }"
        "QLineEdit {"
        "    background-color: #2d2d2d;"
        "    border: 2px solid #4CAF50;"
        "    border-radius: 5px;"
        "    color: white;"
        "    padding: 8px;"
        "    font-size: 14px;"
        "}"
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    border: 2px solid #45a049;"
        "    border-radius: 8px;"
        "    color: white;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
        );
}

QLabel* SalesmanLogin::createLogo()
{
    QLabel* logo = new QLabel(this);
    logo->setFixedSize(60, 60);
    logo->setStyleSheet(
        "QLabel {"
        "    background-color: #4CAF50;"
        "    border: 3px solid white;"
        "    border-radius: 30px;"
        "    color: white;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        );
    logo->setText("AP");
    logo->setAlignment(Qt::AlignCenter);
    return logo;
}

bool SalesmanLogin::initializeDatabase()
{
    static int connectionCount = 0;
    QString connectionName = QString("salesman_login_%1").arg(connectionCount++);
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("alsyed_pakwan.db");
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT NOT NULL UNIQUE, "
                    "password TEXT NOT NULL, "
                    "role TEXT NOT NULL)")) {
        qDebug() << "Create table error:" << query.lastError().text();
        return false;
    }

    query.prepare("INSERT OR IGNORE INTO users (username, password, role) VALUES (:username, :password, :role)");
    query.bindValue(":username", "salesman@alsyedpakwan.com");
    query.bindValue(":password", "sales123");
    query.bindValue(":role", "salesman");
    query.exec();
    return true;
}

bool SalesmanLogin::authenticateSalesman(const QString& email, const QString& password)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND password = :password AND role = 'salesman'");
    query.bindValue(":username", email);
    query.bindValue(":password", password);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

void SalesmanLogin::login()
{
    QString email = emailEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter both email and password!");
        return;
    }

    if (authenticateSalesman(email, password)) {
        SalesmanDashboard *dashboard = new SalesmanDashboard(this->parentWidget());
        dashboard->show();
        this->close();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid email or password!");
    }
}

void SalesmanLogin::goBack()
{
    MainWindow *mainWindow = new MainWindow(this->parentWidget());
    mainWindow->show();
    this->close();
}
