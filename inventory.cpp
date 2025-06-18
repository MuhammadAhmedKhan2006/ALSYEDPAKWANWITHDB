#include "inventory.h"
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

Inventory::Inventory(QWidget *parent)
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

Inventory::~Inventory()
{
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void Inventory::setupUI()
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

    titleLabel = new QLabel("INVENTORY MANAGEMENT", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 20, QFont::Bold);
    titleLabel->setFont(titleFont);

    inventoryTable = new QTableWidget(0, 6, this);
    QStringList headers = {"Item Name", "Supplier", "Quantity", "Total Amount", "Paid Amount", "Due Balance"};
    inventoryTable->setHorizontalHeaderLabels(headers);
    inventoryTable->horizontalHeader()->setStretchLastSection(true);
    inventoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    inventoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    inventoryTable->setMinimumHeight(300);

    itemNameEdit = new QLineEdit(this);
    supplierNameEdit = new QLineEdit(this);
    quantitySpinBox = new QDoubleSpinBox(this);
    totalAmountSpinBox = new QDoubleSpinBox(this);
    paymentSpinBox = new QDoubleSpinBox(this);
    stockSpinBox = new QDoubleSpinBox(this);

    itemNameEdit->setPlaceholderText("Item Name");
    supplierNameEdit->setPlaceholderText("Supplier Name");
    quantitySpinBox->setRange(0.0, 1000000.0);
    totalAmountSpinBox->setRange(0.0, 1000000.0);
    paymentSpinBox->setRange(0.0, 1000000.0);
    stockSpinBox->setRange(0.0, 1000000.0);

    addItemButton = new QPushButton("ADD ITEM", this);
    addPaymentButton = new QPushButton("ADD PAYMENT", this);
    addStockButton = new QPushButton("ADD STOCK", this);
    backButton = new QPushButton("BACK", this);

    leftFormLayout->addWidget(new QLabel("Item Name:"));
    leftFormLayout->addWidget(itemNameEdit);
    leftFormLayout->addWidget(new QLabel("Supplier Name:"));
    leftFormLayout->addWidget(supplierNameEdit);
    leftFormLayout->addWidget(new QLabel("Quantity:"));
    leftFormLayout->addWidget(quantitySpinBox);
    leftFormLayout->addWidget(new QLabel("Total Amount:"));
    leftFormLayout->addWidget(totalAmountSpinBox);

    rightFormLayout->addWidget(new QLabel("Payment Amount:"));
    rightFormLayout->addWidget(paymentSpinBox);
    rightFormLayout->addWidget(new QLabel("Add Stock:"));
    rightFormLayout->addWidget(stockSpinBox);
    rightFormLayout->addStretch();

    buttonLayout->addWidget(addItemButton);
    buttonLayout->addWidget(addPaymentButton);
    buttonLayout->addWidget(addStockButton);
    buttonLayout->addWidget(backButton);

    formLayout->addLayout(leftFormLayout);
    formLayout->addLayout(rightFormLayout);

    mainLayout->addLayout(logoLayout);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(inventoryTable);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(addItemButton, &QPushButton::clicked, this, &Inventory::addItem);
    connect(addPaymentButton, &QPushButton::clicked, this, &Inventory::addPayment);
    connect(addStockButton, &QPushButton::clicked, this, &Inventory::addStock);
    connect(backButton, &QPushButton::clicked, this, &Inventory::goBack);
    connect(inventoryTable, &QTableWidget::itemSelectionChanged, this, &Inventory::onItemSelectionChanged);

    setWindowTitle("Inventory - Alsyed Pakwan");
    resize(800, 600);
}

void Inventory::setTheme()
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

QLabel* Inventory::createLogo()
{
    QLabel* logo = new QLabel(this);
    logo->setFixedSize(60, 60);
    logo->setStyleSheet("QLabel { background-color: #4CAF50; border: 3px solid white; border-radius: 30px; color: white; font-weight: bold; font-size: 14px; }");
    logo->setText("AP");
    logo->setAlignment(Qt::AlignCenter);
    return logo;
}

bool Inventory::initializeDatabase()
{
    static int connectionCount = 0;
    QString connectionName = QString("inventory_%1").arg(connectionCount++);
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("alsyed_pakwan.db");
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS inventory_items ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "item_name TEXT NOT NULL, "
                    "supplier_name TEXT, "
                    "quantity REAL, "
                    "total_amount REAL, "
                    "paid_amount REAL, "
                    "due_balance REAL)")) {
        qDebug() << "Create table error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Inventory::saveInventoryItem(const InventoryItem& item)
{
    db.transaction();
    QSqlQuery query(db);
    query.prepare("INSERT INTO inventory_items (item_name, supplier_name, quantity, total_amount, paid_amount, due_balance) "
                  "VALUES (:item_name, :supplier_name, :quantity, :total_amount, :paid_amount, :due_balance)");
    query.bindValue(":item_name", item.itemName);
    query.bindValue(":supplier_name", item.supplierName);
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":total_amount", item.totalAmount);
    query.bindValue(":paid_amount", item.paidAmount);
    query.bindValue(":due_balance", item.dueBalance);
    bool success = query.exec();
    db.commit();
    return success;
}

bool Inventory::updateInventoryItem(const InventoryItem& item)
{
    if (selectedRow < 0) return false;
    db.transaction();
    QSqlQuery query(db);
    query.prepare("UPDATE inventory_items SET item_name = :item_name, supplier_name = :supplier_name, "
                  "quantity = :quantity, total_amount = :total_amount, paid_amount = :paid_amount, "
                  "due_balance = :due_balance WHERE id = (SELECT id FROM inventory_items LIMIT 1 OFFSET :offset)");
    query.bindValue(":item_name", item.itemName);
    query.bindValue(":supplier_name", item.supplierName);
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":total_amount", item.totalAmount);
    query.bindValue(":paid_amount", item.paidAmount);
    query.bindValue(":due_balance", item.dueBalance);
    query.bindValue(":offset", selectedRow);
    bool success = query.exec();
    db.commit();
    return success;
}

bool Inventory::deleteInventoryItem(int row)
{
    if (row < 0) return false;
    db.transaction();
    QSqlQuery query(db);
    query.prepare("DELETE FROM inventory_items WHERE id = (SELECT id FROM inventory_items LIMIT 1 OFFSET :offset)");
    query.bindValue(":offset", row);
    bool success = query.exec();
    db.commit();
    return success;
}

QList<InventoryItem> Inventory::loadInventoryItemsFromDatabase()
{
    QList<InventoryItem> items;
    QSqlQuery query(db);
    if (query.exec("SELECT item_name, supplier_name, quantity, total_amount, paid_amount, due_balance FROM inventory_items")) {
        while (query.next()) {
            InventoryItem item;
            item.itemName = query.value(0).toString();
            item.supplierName = query.value(1).toString();
            item.quantity = query.value(2).toDouble();
            item.totalAmount = query.value(3).toDouble();
            item.paidAmount = query.value(4).toDouble();
            item.dueBalance = query.value(5).toDouble();
            items.append(item);
        }
    } else {
        qDebug() << "Load inventory items error:" << query.lastError().text();
    }
    return items;
}

void Inventory::loadInitialData()
{
    inventoryItems = loadInventoryItemsFromDatabase();
    refreshTable();
}

void Inventory::refreshTable()
{
    inventoryTable->setRowCount(inventoryItems.size());
    for (int row = 0; row < inventoryItems.size(); ++row) {
        InventoryItem item = inventoryItems[row];
        inventoryTable->setItem(row, 0, new QTableWidgetItem(item.itemName));
        inventoryTable->setItem(row, 1, new QTableWidgetItem(item.supplierName));
        inventoryTable->setItem(row, 2, new QTableWidgetItem(QString::number(item.quantity, 'f', 2)));
        inventoryTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.totalAmount, 'f', 2)));
        inventoryTable->setItem(row, 4, new QTableWidgetItem(QString::number(item.paidAmount, 'f', 2)));
        inventoryTable->setItem(row, 5, new QTableWidgetItem(QString::number(item.dueBalance, 'f', 2)));
    }
}

void Inventory::addItem()
{
    InventoryItem item;
    item.itemName = itemNameEdit->text().trimmed();
    item.supplierName = supplierNameEdit->text().trimmed();
    item.quantity = quantitySpinBox->value();
    item.totalAmount = totalAmountSpinBox->value();
    item.paidAmount = 0.0;
    item.dueBalance = item.totalAmount;

    if (item.itemName.isEmpty() || item.supplierName.isEmpty() || item.quantity <= 0 || item.totalAmount <= 0) {
        QMessageBox::warning(this, "Input Error", "Please fill all fields with valid values!");
        return;
    }

    if (saveInventoryItem(item)) {
        inventoryItems.append(item);
        refreshTable();
        clearForm();
        QMessageBox::information(this, "Success", "Item added successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to add item to database!");
    }
}

void Inventory::addPayment()
{
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an item to add payment!");
        return;
    }

    double payment = paymentSpinBox->value();
    if (payment <= 0 || payment > inventoryItems[selectedRow].dueBalance) {
        QMessageBox::warning(this, "Input Error", "Invalid payment amount!");
        return;
    }

    InventoryItem& item = inventoryItems[selectedRow];
    item.paidAmount += payment;
    item.dueBalance -= payment;

    if (updateInventoryItem(item)) {
        refreshTable();
        clearForm();
        QMessageBox::information(this, "Success", "Payment added successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to update payment!");
    }
}

void Inventory::addStock()
{
    if (selectedRow < 0) {
        QMessageBox::warning(this, "Selection Error", "Please select an item to add stock!");
        return;
    }

    double stock = stockSpinBox->value();
    if (stock <= 0) {
        QMessageBox::warning(this, "Input Error", "Invalid stock amount!");
        return;
    }

    InventoryItem& item = inventoryItems[selectedRow];
    item.quantity += stock;

    if (updateInventoryItem(item)) {
        refreshTable();
        clearForm();
        QMessageBox::information(this, "Success", "Stock added successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to update stock!");
    }
}

void Inventory::goBack()
{
    OwnerDashboard *dashboard = new OwnerDashboard(this->parentWidget()); // Corrected
    dashboard->show();
    this->close();
}

void Inventory::onItemSelectionChanged()
{
    selectedRow = inventoryTable->currentRow();
    if (selectedRow >= 0 && selectedRow < inventoryItems.size()) {
        InventoryItem item = inventoryItems[selectedRow];
        itemNameEdit->setText(item.itemName);
        supplierNameEdit->setText(item.supplierName);
        quantitySpinBox->setValue(item.quantity);
        totalAmountSpinBox->setValue(item.totalAmount);
        paymentSpinBox->setValue(0.0);
        stockSpinBox->setValue(0.0);
    } else {
        clearForm();
    }
}

void Inventory::clearForm()
{
    itemNameEdit->clear();
    supplierNameEdit->clear();
    quantitySpinBox->setValue(0.0);
    totalAmountSpinBox->setValue(0.0);
    paymentSpinBox->setValue(0.0);
    stockSpinBox->setValue(0.0);
    inventoryTable->clearSelection();
    selectedRow = -1;
}
