#ifndef OWNERLOGIN_H
#define OWNERLOGIN_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QSqlDatabase>

class OwnerLogin : public QDialog
{
    Q_OBJECT
public:
    explicit OwnerLogin(QWidget *parent = nullptr);
    ~OwnerLogin();

private slots:
    void login();
    void goBack();

private:
    void setupUI();
    void setTheme();
    QLabel* createLogo();
    bool initializeDatabase();
    bool authenticateOwner(const QString& email, const QString& password);

    QVBoxLayout *mainLayout;
    QHBoxLayout *logoLayout;
    QVBoxLayout *formLayout;
    QLabel *logoLabel;
    QLabel *titleLabel;
    QLabel *emailLabel;
    QLineEdit *emailEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *backButton;
    QSqlDatabase db;
};

#endif // OWNERLOGIN_H
