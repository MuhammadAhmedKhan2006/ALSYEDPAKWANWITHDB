#ifndef EMPLOYEESALARY_H
#define EMPLOYEESALARY_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QPushButton>
#include <QSqlDatabase>
#include <QList>

struct Employee {
    QString name;
    QString position;
    double monthlySalary;
    double advanceAmount;
    double remainingSalary;
};

class EmployeeSalary : public QMainWindow
{
    Q_OBJECT
public:
    explicit EmployeeSalary(QWidget *parent = nullptr);
    ~EmployeeSalary() override;

private slots:
    void addEmployee();
    void giveAdvance();
    void resetSalary();
    void goBack();
    void onEmployeeSelectionChanged();

private:
    void setupUI();
    void setTheme();
    QLabel* createLogo();
    bool initializeDatabase();
    bool saveEmployee(const Employee& employee);
    bool updateEmployee(const Employee& employee);
    bool deleteEmployee(int row);
    QList<Employee> loadEmployeesFromDatabase();
    void loadInitialData();
    void refreshTable();
    void clearForm(); // Declared here

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *logoLayout;
    QHBoxLayout *formLayout;
    QVBoxLayout *leftFormLayout;
    QVBoxLayout *rightFormLayout;
    QHBoxLayout *buttonLayout;
    QLabel *logoLabel;
    QLabel *titleLabel;
    QTableWidget *employeeTable;
    QLineEdit *nameEdit;
    QLineEdit *positionEdit;
    QDoubleSpinBox *salarySpinBox;
    QDoubleSpinBox *advanceSpinBox;
    QPushButton *addEmployeeButton;
    QPushButton *giveAdvanceButton;
    QPushButton *resetSalaryButton;
    QPushButton *backButton;
    QList<Employee> employees;
    int selectedRow;
    QSqlDatabase db;
};

#endif // EMPLOYEESALARY_H
