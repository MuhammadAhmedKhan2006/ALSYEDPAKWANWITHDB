#ifndef SALESMANDASHBOARD_H
#define SALESMANDASHBOARD_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QSqlDatabase>
#include <QList>

struct MenuItem {
    QString name;
    double price;
    QString category;
};

struct OrderItem {
    QString name;
    int quantity;
    double total;
};

class SalesmanDashboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit SalesmanDashboard(QWidget *parent = nullptr);
    ~SalesmanDashboard() override;

private slots:
    void addToOrder();
    void removeFromOrder();
    void clearOrder();
    void generateBill();
    void printBill();
    void generateDailySummary();
    void logout();
    void onOrderItemClicked(QListWidgetItem *item);
    void updateReceivedAmount(const QString &text);
    void showAboutDialog();
    void addMenuItem();
    void editMenuItem();

private:
    void setupUI();
    void setTheme();
    QLabel* createLogo();
    bool initializeDatabase();
    bool saveOrderToDatabase(const QList<OrderItem>& order);
    bool saveDailySummary();
    void loadMenuItems();
    void updateBillPreview();
    QString generateSummaryText();

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *logoLayout;
    QHBoxLayout *contentLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QHBoxLayout *buttonLayout;
    QHBoxLayout *orderButtonLayout;
    QHBoxLayout *paymentLayout;
    QHBoxLayout *menuInputLayout;
    QHBoxLayout *menuEditLayout;
    QMenuBar *menuBar;
    QMenu *helpMenu;
    QAction *aboutAction;
    QLabel *logoLabel;
    QLabel *titleLabel;
    QLabel *menuLabel;
    QLabel *orderLabel;
    QLabel *customerLabel;
    QLabel *receivedLabel;
    QLabel *changeLabel;
    QLabel *developerLabel;
    QLabel *totalLabel;
    QPushButton *logoutButton;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *clearButton;
    QPushButton *generateBillButton;
    QPushButton *printButton;
    QPushButton *dailySummaryButton;
    QPushButton *addMenuItemButton;
    QPushButton *editMenuItemButton;
    QSpinBox *quantitySpinBox;
    QLineEdit *customerNameEdit;
    QLineEdit *receivedAmountEdit;
    QLineEdit *menuItemNameEdit;
    QLineEdit *menuItemPriceEdit;
    QLineEdit *menuItemEditNameEdit;
    QLineEdit *menuItemEditPriceEdit;
    QListWidget *orderList;
    QTextEdit *billPreview;
    QWidget *menuWidget;
    QGridLayout *menuGrid;
    QSqlDatabase db;
    QList<MenuItem> menuItems;
    QList<OrderItem> currentOrder;
    double orderTotal;
    double receivedAmount;
    double changeAmount;
    QString customerName;
    int selectedMenuItem;
    int selectedMenuItemForEdit;
    const QString developerName = "Your Name";
};

#endif // SALESMANDASHBOARD_H
