#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QSqlDatabase>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void openOwnerLogin();
    void openSalesmanLogin();

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
    QLabel *titleLabel;
    QPushButton *ownerButton;
    QPushButton *salesmanButton;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
