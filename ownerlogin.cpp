#include "ownerlogin.h"
#include "ownerdashboard.h"
#include "mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QtGlobal>
#include <QMessageBox>

OwnerLogin::OwnerLogin(QWidget *parent)
    : QDialog(parent)
{
    if (!initializeDatabase()) {
        qDebug() << "Database initialization failed!";
        return;
    }
    setupUI();
    setTheme();
}

OwnerLogin::~OwnerLogin()
{
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void OwnerLogin::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    logoLayout = new QHBoxLayout();
    formLayout = new QVBoxLayout();

    logoLabel = createLogo();
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();

    titleLabel = new QLabel("OWNER LOGIN", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 20, QFont::Bold);
    titleLabel->setFont(titleFont);

    emailLabel = new QLabel("Email:", this);
    emailEdit = new QLineEdit(this);
    passwordLabel = new QLabel("Password:", this);
    passwordEdit = new QLineEdit(this);
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

    connect(loginButton, &QPushButton::clicked, this, &OwnerLogin::login);
    connect(backButton, &QPushButton::clicked, this, &OwnerLogin::goBack);

    setWindowTitle("Owner Login - Alsyed Pakwan");
    resize(400, 500);
}

void OwnerLogin::setTheme()
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

QLabel* OwnerLogin::createLogo()
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

bool OwnerLogin::initializeDatabase()
{
    static int connectionCount = 0;
    QString connectionName = QString("owner_login_%1").arg(connectionCount++);
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
    query.bindValue(":username", "owner@alsyedpakwan.com");
    query.bindValue(":password", "owner123");
    query.bindValue(":role", "owner");
    query.exec();
    return true;
}

bool OwnerLogin::authenticateOwner(const QString& email, const QString& password)
{
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND password = :password AND role = 'owner'");
    query.bindValue(":username", email);
    query.bindValue(":password", password);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

void OwnerLogin::login()
{
    QString email = emailEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter both email and password!");
        return;
    }

    if (authenticateOwner(email, password)) {
        OwnerDashboard *dashboard = new OwnerDashboard(this->parentWidget());
        dashboard->show();
        this->close();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid email or password!");
    }
}

void OwnerLogin::goBack()
{
    MainWindow *mainWindow = new MainWindow(this->parentWidget());
    mainWindow->show();
    this->close();
}
