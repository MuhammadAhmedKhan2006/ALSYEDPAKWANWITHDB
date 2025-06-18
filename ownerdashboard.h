#ifndef OWNERDASHBOARD_H
#define OWNERDASHBOARD_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QSqlDatabase>

class OwnerDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit OwnerDashboard(QWidget *parent = nullptr);
    ~OwnerDashboard();
    void doOwnerDashboard(); // Declared

private slots:
    void openInventory();
    void openEmployeeSalary();
    void openSalesman();
    void logout();

private:
    void setupUI();
    void setTheme();
    QLabel* createLogo();
    bool initializeDatabase();

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *logoLayout;
    QVBoxLayout *buttonLayout;
    QLabel *logoLabel;
    QLabel *titleLabel; // Correct member
    QLabel *welcomeLabel;
    QPushButton *inventoryButton;
    QPushButton *salaryButton;
    QPushButton *salesmanButton;
    QPushButton *logoutButton;
    QSqlDatabase db;
};

#endif // OWNERDASHBOARD_H
