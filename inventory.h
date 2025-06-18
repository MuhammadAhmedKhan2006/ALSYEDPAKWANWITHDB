#ifndef INVENTORY_H
#define INVENTORY_H

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

struct InventoryItem {
    QString itemName;
    QString supplierName;
    double quantity;
    double totalAmount;
    double dueBalance;
    double paidAmount;
};

class Inventory : public QMainWindow
{
    Q_OBJECT
public:
    explicit Inventory(QWidget *parent = nullptr);
    ~Inventory() override;

private slots:
    void addItem();
    void addPayment();
    void addStock();
    void goBack();
    void onItemSelectionChanged();

private:
    void setupUI();
    void setTheme();
    QLabel* createLogo();
    bool initializeDatabase();
    bool saveInventoryItem(const InventoryItem& item);
    bool updateInventoryItem(const InventoryItem& item);
    bool deleteInventoryItem(int row);
    QList<InventoryItem> loadInventoryItemsFromDatabase();
    void loadInitialData();
    void refreshTable();
    void clearForm(); // Added declaration

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *logoLayout;
    QHBoxLayout *formLayout;
    QVBoxLayout *leftFormLayout;
    QVBoxLayout *rightFormLayout;
    QHBoxLayout *buttonLayout;
    QLabel *logoLabel;
    QLabel *titleLabel;
    QTableWidget *inventoryTable;
    QLineEdit *itemNameEdit;
    QLineEdit *supplierNameEdit;
    QDoubleSpinBox *quantitySpinBox;
    QDoubleSpinBox *totalAmountSpinBox;
    QDoubleSpinBox *paymentSpinBox;
    QDoubleSpinBox *stockSpinBox;
    QPushButton *addItemButton;
    QPushButton *addPaymentButton;
    QPushButton *addStockButton;
    QPushButton *backButton;
    QList<InventoryItem> inventoryItems;
    int selectedRow;
    QSqlDatabase db;
};

#endif // INVENTORY_H
