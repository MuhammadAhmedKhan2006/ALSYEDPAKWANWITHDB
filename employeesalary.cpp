#include "employeesalary.h"
#include "mainwindow.h"
#include <QtWidgets/QHeaderView>
#include "ownerdashboard.h" // Added
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFont>
#include <QtGlobal>
#include <QMessageBox>

EmployeeSalary::EmployeeSalary(QWidget *parent)
    : QMainWindow(parent), selectedRow(-1)
{
    if (!initializeDatabase()) {
        qDebug() << "Database initialization failed!";
        QMessageBox::critical(this, "Error", "Failed to initialize database!");
        return;
    }
    setupUI();
    setTheme();
    loadInitialData();
}

EmployeeSalary::~EmployeeSalary()
{
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void EmployeeSalary::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    logoLayout = new QHBoxLayout();
    formLayout = new QHBoxLayout();
    leftFormLayout = new QVBoxLayout();
    rightFormLayout = new QVBoxLayout();
    buttonLayout = new QHBoxLayout();

    logoLabel = createLogo();
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();

    titleLabel = new QLabel("EMPLOYEE SALARY MANAGEMENT", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 20, QFont::Bold);
    titleLabel->setFont(titleFont);

    employeeTable = new QTableWidget(0, 5, this);
    QStringList headers = {"Name", "Position", "Monthly Salary", "Advance", "Remaining Salary"};
    employeeTable->setHorizontalHeaderLabels(headers);
    employeeTable->horizontalHeader()->setStretchLastSection(true);
    employeeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    employeeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    employeeTable->setMinimumHeight(300);

    nameEdit = new QLineEdit(this);
    positionEdit = new QLineEdit(this);
    salarySpinBox = new QDoubleSpinBox(this);
    advanceSpinBox = new QDoubleSpinBox(this);

    nameEdit->setPlaceholderText("Employee Name");
    positionEdit->setPlaceholderText("Position");
    salarySpinBox->setRange(0.0, 1000000.0);
    advanceSpinBox->setRange(0.0, 1000000.0);

    addEmployeeButton = new QPushButton("ADD EMPLOYEE", this);
    giveAdvanceButton = new QPushButton("GIVE ADVANCE", this);
    resetSalaryButton = new QPushButton("RESET SALARY", this);
    backButton = new QPushButton("BACK", this);

    leftFormLayout->addWidget(new QLabel("Name:"));
    leftFormLayout->addWidget(nameEdit);
    leftFormLayout->addWidget(new QLabel("Position:"));
    leftFormLayout->addWidget(positionEdit);
    leftFormLayout->addWidget(new QLabel("Monthly Salary:"));
    leftFormLayout->addWidget(salarySpinBox);

    rightFormLayout->addWidget(new QLabel("Advance Amount:"));
    rightFormLayout->addWidget(advanceSpinBox);
    rightFormLayout->addStretch();

    buttonLayout->addWidget(addEmployeeButton);
    buttonLayout->addWidget(giveAdvanceButton);
    buttonLayout->addWidget(resetSalaryButton);
    buttonLayout->addWidget(backButton);

    formLayout->addLayout(leftFormLayout);
    formLayout->addLayout(rightFormLayout);

    mainLayout->addLayout(logoLayout);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(employeeTable);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(addEmployeeButton, &QPushButton::clicked, this, &EmployeeSalary::addEmployee);
    connect(giveAdvanceButton, &QPushButton::clicked, this, &EmployeeSalary::giveAdvance);
    connect(resetSalaryButton, &QPushButton::clicked, this, &EmployeeSalary::resetSalary);
    connect(backButton, &QPushButton::clicked, this, &EmployeeSalary::goBack);
    connect(employeeTable, &QTableWidget::itemSelectionChanged, this, &EmployeeSalary::onEmployeeSelectionChanged);

    setWindowTitle("Employee Salary - Alsyed Pakwan");
    resize(800, 600);
}

void EmployeeSalary::setTheme()
{
    setStyleSheet(
        "QMainWindow { background-color: #1a1a1a; }"
        "QLabel { color: white; }"
        "QLineEdit { background-color: #2d2d2d; border: 2px solid #4CAF50; border-radius: 5px; color: white; padding: 5px; }"
        "QDoubleSpinBox { background-color: #2d2d2d; border: 2px solid #4CAF50; border-radius: 5px; color: white; padding: 5px; }"
        "QPushButton { background-color: #4CAF50; border: 2px solid #45a049; border-radius: 8px; color: white; font-weight: bold; padding: 8px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }"
        "QTableWidget { background-color: #2d2d2d; color: white; border: 2px solid #4CAF50; }"
        );
}

QLabel* EmployeeSalary::createLogo()
{
    QLabel* logo = new QLabel(this);
    logo->setFixedSize(60, 60);
    logo->setStyleSheet("QLabel { background-color: #4CAF50; border: 3px solid white; border-radius: 30px; color: white; font-weight: bold; font-size: 14px; }");
    logo->setText("AP");
    logo->setAlignment(Qt::AlignCenter);
    return logo;
}

bool EmployeeSalary::initializeDatabase()
{
    static int connectionCount = 0;
    QString connectionName = QString("employee_salary_%1").arg(connectionCount++);
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("alsyed_pakwan.db");
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS employees ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "position TEXT, "
                    "monthly_salary REAL, "
                    "advance_amount REAL, "
                    "remaining_salary REAL)")) {
        qDebug() << "Create table error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool EmployeeSalary::saveEmployee(const Employee& employee)
{
    db.transaction();
    QSqlQuery query(db);
    query.prepare("INSERT INTO employees (name, position, monthly_salary, advance_amount, remaining_salary) "
                  "VALUES (:name, :position, :monthly_salary, :advance_amount, :remaining_salary)");
    query.bindValue(":name", employee.name);
    query.bindValue(":position", employee.position);
    query.bindValue(":monthly_salary", employee.monthlySalary);
    query.bindValue(":advance_amount", employee.advanceAmount);
    query.bindValue(":remaining_salary", employee.remainingSalary);
    bool success = query.exec();
    db.commit();
    return success;
}

bool EmployeeSalary::updateEmployee(const Employee& employee)
{
    if (selectedRow < 0) return false;
    db.transaction();
    QSqlQuery query(db);
    query.prepare("UPDATE employees SET name = :name, position = :position, "
                  "monthly_salary = :monthly_salary, advance_amount = :advance_amount, "
                  "remaining_salary = :remaining_salary WHERE id = (SELECT id FROM employees LIMIT 1 OFFSET :offset)");
    query.bindValue(":name", employee.name);
    query.bindValue(":position", employee.position);
    query.bindValue(":monthly_salary", employee.monthlySalary);
    query.bindValue(":advance_amount", employee.advanceAmount);
    query.bindValue(":remaining_salary", employee.remainingSalary);
    query.bindValue(":offset", selectedRow);
    bool success = query.exec();
    db.commit();
    return success;
}

bool EmployeeSalary::deleteEmployee(int row)
{
    if (row < 0) return false;
    db.transaction();
    QSqlQuery query(db);
    query.prepare("DELETE FROM employees WHERE id = (SELECT id FROM employees LIMIT 1 OFFSET :offset)");
    query.bindValue(":offset", row);
    bool success = query.exec();
    db.commit();
    return success;
}

QList<Employee> EmployeeSalary::loadEmployeesFromDatabase()
{
    QList<Employee> employees;
    QSqlQuery query(db);
    if (query.exec("SELECT name, position, monthly_salary, advance_amount, remaining_salary FROM employees")) {
        while (query.next()) {
            Employee employee;
            employee.name = query.value(0).toString();
            employee.position = query.value(1).toString();
            employee.monthlySalary = query.value(2).toDouble();
            employee.advanceAmount = query.value(3).toDouble();
            employee.remainingSalary = query.value(4).toDouble();
            employees.append(employee);
        }
    } else {
        qDebug() << "Load employees error:" << query.lastError().text();
    }
    return employees;
}

void EmployeeSalary::loadInitialData()
{
    employees = loadEmployeesFromDatabase();
    refreshTable();
}

void EmployeeSalary::refreshTable()
{
    employeeTable->setRowCount(employees.size());
    for (int row = 0; row < employees.size(); ++row) {
        Employee employee = employees[row];
        employeeTable->setItem(row, 0, new QTableWidgetItem(employee.name));
        employeeTable->setItem(row, 1, new QTableWidgetItem(employee.position));
        employeeTable->setItem(row, 2, new QTableWidgetItem(QString::number(employee.monthlySalary, 'f', 2)));
        employeeTable->setItem(row, 3, new QTableWidgetItem(QString::number(employee.advanceAmount, 'f', 2)));
        employeeTable->setItem(row, 4, new QTableWidgetItem(QString::number(employee.remainingSalary, 'f', 2)));
    }
}

void EmployeeSalary::addEmployee()
{
    Employee employee;
    employee.name = nameEdit->text().trimmed();
    employee.position = positionEdit->text().trimmed();
    employee.monthlySalary = salarySpinBox->value();
    employee.advanceAmount = 0.0;
    employee.remainingSalary = employee.monthlySalary;

    if (employee.name.isEmpty() || employee.position.isEmpty() || employee.monthlySalary <= 0) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields with valid values!");
        return;
    }

    if (saveEmployee(employee)) {
        employees.append(employee);
        refreshTable();
        clearForm();
        QMessageBox::information(this, "Success", "Employee added successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to add employee to database!");
    }
}

void EmployeeSalary::giveAdvance()
{
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an employee to give advance!");
        return;
    }

    double advance = advanceSpinBox->value();
    if (advance <= 0 || advance > employees[selectedRow].remainingSalary) {
        QMessageBox::warning(this, "Input Error", "Invalid advance amount!");
        return;
    }

    Employee& employee = employees[selectedRow];
    employee.advanceAmount += advance;
    employee.remainingSalary -= advance;

    if (updateEmployee(employee)) {
        refreshTable();
        clearForm();
        QMessageBox::information(this, "Success", "Advance given successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to update advance!");
    }
}

void EmployeeSalary::resetSalary()
{
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an employee to reset salary!");
        return;
    }

    Employee& employee = employees[selectedRow];
    employee.advanceAmount = 0.0;
    employee.remainingSalary = employee.monthlySalary;

    if (updateEmployee(employee)) {
        refreshTable();
        clearForm();
        QMessageBox::information(this, "Success", "Salary reset successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to reset salary!");
    }
}

void EmployeeSalary::goBack()
{
    OwnerDashboard *dashboard = new OwnerDashboard(this->parentWidget()); // Corrected
    dashboard->show();
    this->close();
}

void EmployeeSalary::onEmployeeSelectionChanged()
{
    selectedRow = employeeTable->currentRow();
    if (selectedRow >= 0 && selectedRow < employees.size()) {
        Employee employee = employees[selectedRow];
        nameEdit->setText(employee.name);
        positionEdit->setText(employee.position);
        salarySpinBox->setValue(employee.monthlySalary);
        advanceSpinBox->setValue(0.0);
    } else {
        clearForm();
    }
}

void EmployeeSalary::clearForm()
{
    nameEdit->clear();
    positionEdit->clear();
    salarySpinBox->setValue(0.0);
    advanceSpinBox->setValue(0.0);
    employeeTable->clearSelection();
    selectedRow = -1;
}
