#ifndef SALESMANLOGIN_H
#define SALESMANLOGIN_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QSqlDatabase>

class SalesmanLogin : public QDialog
{
    Q_OBJECT
public:
    explicit SalesmanLogin(QWidget *parent = nullptr);
    ~SalesmanLogin() override;

private slots:
    void login();
    void goBack();

private:
    void setupUI();
    void setTheme();
    QLabel* createLogo();
    bool initializeDatabase();
    bool authenticateSalesman(const QString& email, const QString& password);

    QVBoxLayout *mainLayout;
    QHBoxLayout *logoLayout;
    QVBoxLayout *formLayout;
    QLabel *logoLabel;
    QLabel *titleLabel;
    QLabel *emailLabel;    // Added
    QLabel *passwordLabel; // Added
    QLineEdit *emailEdit;  // Added
    QLineEdit *passwordEdit; // Added
    QPushButton *loginButton;
    QPushButton *backButton;
    QSqlDatabase db;
};

#endif // SALESMANLOGIN_H
