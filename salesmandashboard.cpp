#include "salesmandashboard.h"
#include "mainwindow.h"
#include <QScrollArea>
#include <QDoubleValidator>
#include <QApplication>
#include <QFileInfo>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QTextDocument>
#include <QMessageBox>
#include <QFont>
#include <QtGlobal>
#include <QDir>
#include <QDateTime>

SalesmanDashboard::SalesmanDashboard(QWidget *parent)
    : QMainWindow(parent), orderTotal(0.0), receivedAmount(0.0), changeAmount(0.0), selectedMenuItem(-1), selectedMenuItemForEdit(-1)
{
    if (!initializeDatabase()) {
        QMessageBox::critical(this, "Error", "Failed to initialize database!");
        return;
    }
    setupUI();
    setTheme();
    loadMenuItems();
}

SalesmanDashboard::~SalesmanDashboard()
{
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

void SalesmanDashboard::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    logoLayout = new QHBoxLayout();
    contentLayout = new QHBoxLayout();
    leftLayout = new QVBoxLayout();
    rightLayout = new QVBoxLayout();
    buttonLayout = new QHBoxLayout();
    orderButtonLayout = new QHBoxLayout();
    paymentLayout = new QHBoxLayout();
    menuInputLayout = new QHBoxLayout();
    menuEditLayout = new QHBoxLayout();

    menuBar = new QMenuBar(this);
    helpMenu = new QMenu("Help", this);
    aboutAction = new QAction("About", this);
    helpMenu->addAction(aboutAction);
    menuBar->addMenu(helpMenu);
    setMenuBar(menuBar);

    logoLabel = createLogo();
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();
    logoutButton = new QPushButton("LOGOUT", this);
    logoutButton->setFixedSize(100, 40);
    QFont buttonFont("Arial", 12, QFont::Bold);
    logoutButton->setFont(buttonFont);
    logoLayout->addWidget(logoutButton);

    titleLabel = new QLabel("POINT OF SALE - ALSYED PAKWAN", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 20, QFont::Bold);
    titleLabel->setFont(titleFont);

    menuLabel = new QLabel("MENU ITEMS", this);
    menuLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

    menuWidget = new QWidget(this);
    menuGrid = new QGridLayout(menuWidget);
    QScrollArea *menuScroll = new QScrollArea(this);
    menuScroll->setWidget(menuWidget);
    menuScroll->setWidgetResizable(true);
    menuScroll->setMinimumSize(600, 400); // Increased size to show all items

    orderLabel = new QLabel("CURRENT ORDER", this);
    orderLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

    quantitySpinBox = new QSpinBox(this);
    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setMaximum(100);
    quantitySpinBox->setValue(1);
    quantitySpinBox->setFixedHeight(30);

    addButton = new QPushButton("ADD TO ORDER", this);
    removeButton = new QPushButton("REMOVE ITEM", this);
    clearButton = new QPushButton("CLEAR ORDER", this);

    addButton->setMinimumHeight(40);
    removeButton->setMinimumHeight(40);
    clearButton->setMinimumHeight(40);

    orderButtonLayout->addWidget(new QLabel("Quantity:"));
    orderButtonLayout->addWidget(quantitySpinBox);
    orderButtonLayout->addWidget(addButton);
    orderButtonLayout->addWidget(removeButton);
    orderButtonLayout->addWidget(clearButton);

    orderList = new QListWidget(this);
    orderList->setMinimumHeight(200);

    totalLabel = new QLabel("Total: Rs. 0.00", this);
    totalLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

    leftLayout->addWidget(menuLabel);
    leftLayout->addWidget(menuScroll);
    leftLayout->addWidget(orderLabel);
    leftLayout->addLayout(orderButtonLayout);
    leftLayout->addWidget(orderList);
    leftLayout->addWidget(totalLabel);

    // Add menu item input fields
    menuInputLayout->addWidget(new QLabel("Item Name:"));
    menuItemNameEdit = new QLineEdit(this);
    menuInputLayout->addWidget(menuItemNameEdit);
    menuInputLayout->addWidget(new QLabel("Price (Rs.):"));
    menuItemPriceEdit = new QLineEdit(this);
    menuItemPriceEdit->setValidator(new QDoubleValidator(0.0, 100000.0, 2, this));
    menuInputLayout->addWidget(menuItemPriceEdit);
    addMenuItemButton = new QPushButton("ADD MENU ITEM", this);
    addMenuItemButton->setMinimumHeight(40);
    menuInputLayout->addWidget(addMenuItemButton);
    leftLayout->addLayout(menuInputLayout);

    // Edit menu item fields
    menuEditLayout->addWidget(new QLabel("Edit Name:"));
    menuItemEditNameEdit = new QLineEdit(this);
    menuEditLayout->addWidget(menuItemEditNameEdit);
    menuEditLayout->addWidget(new QLabel("Edit Price (Rs.):"));
    menuItemEditPriceEdit = new QLineEdit(this);
    menuItemEditPriceEdit->setValidator(new QDoubleValidator(0.0, 100000.0, 2, this));
    menuEditLayout->addWidget(menuItemEditPriceEdit);
    editMenuItemButton = new QPushButton("EDIT MENU ITEM", this);
    editMenuItemButton->setMinimumHeight(40);
    menuEditLayout->addWidget(editMenuItemButton);
    leftLayout->addLayout(menuEditLayout);

    QLabel *billLabel = new QLabel("BILL PREVIEW", this);
    billLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

    customerLabel = new QLabel("Customer Name:", this);
    customerNameEdit = new QLineEdit(this);
    customerNameEdit->setPlaceholderText("Enter customer name");
    customerNameEdit->setFixedHeight(30);

    receivedLabel = new QLabel("Received: Rs. 0.00", this);
    changeLabel = new QLabel("Change: Rs. 0.00", this);
    receivedLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    changeLabel->setStyleSheet("font-size: 14px; font-weight: bold;");

    receivedAmountEdit = new QLineEdit(this);
    receivedAmountEdit->setPlaceholderText("Enter amount received");
    receivedAmountEdit->setFixedHeight(30);
    receivedAmountEdit->setValidator(new QDoubleValidator(0.0, 100000.0, 2, this));

    paymentLayout->addWidget(new QLabel("Amount Received:"));
    paymentLayout->addWidget(receivedAmountEdit);

    billPreview = new QTextEdit(this);
    billPreview->setReadOnly(true);
    billPreview->setMinimumWidth(300);

    generateBillButton = new QPushButton("GENERATE BILL", this);
    printButton = new QPushButton("PRINT BILL", this);
    dailySummaryButton = new QPushButton("DAILY SUMMARY", this);
    generateBillButton->setMinimumHeight(40);
    printButton->setMinimumHeight(40);
    dailySummaryButton->setMinimumHeight(40);

    buttonLayout->addWidget(generateBillButton);
    buttonLayout->addWidget(printButton);
    buttonLayout->addWidget(dailySummaryButton);
    buttonLayout->addStretch();

    rightLayout->addWidget(billLabel);
    rightLayout->addWidget(customerLabel);
    rightLayout->addWidget(customerNameEdit);
    rightLayout->addWidget(billPreview);
    rightLayout->addLayout(paymentLayout);
    rightLayout->addWidget(receivedLabel);
    rightLayout->addWidget(changeLabel);
    rightLayout->addLayout(buttonLayout);

    developerLabel = new QLabel(QString("Developed by %1").arg(developerName), this);
    developerLabel->setStyleSheet("font-size: 10px; color: #888888;");
    developerLabel->setAlignment(Qt::AlignRight);

    contentLayout->addLayout(leftLayout);
    contentLayout->addLayout(rightLayout);

    mainLayout->addLayout(logoLayout);
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(contentLayout);
    mainLayout->addWidget(developerLabel);

    connect(addButton, &QPushButton::clicked, this, &SalesmanDashboard::addToOrder);
    connect(removeButton, &QPushButton::clicked, this, &SalesmanDashboard::removeFromOrder);
    connect(clearButton, &QPushButton::clicked, this, &SalesmanDashboard::clearOrder);
    connect(generateBillButton, &QPushButton::clicked, this, &SalesmanDashboard::generateBill);
    connect(printButton, &QPushButton::clicked, this, &SalesmanDashboard::printBill);
    connect(dailySummaryButton, &QPushButton::clicked, this, &SalesmanDashboard::generateDailySummary);
    connect(logoutButton, &QPushButton::clicked, this, &SalesmanDashboard::logout);
    connect(orderList, &QListWidget::itemClicked, this, &SalesmanDashboard::onOrderItemClicked);
    connect(receivedAmountEdit, &QLineEdit::textChanged, this, &SalesmanDashboard::updateReceivedAmount);
    connect(aboutAction, &QAction::triggered, this, &SalesmanDashboard::showAboutDialog);
    connect(addMenuItemButton, &QPushButton::clicked, this, &SalesmanDashboard::addMenuItem);
    connect(editMenuItemButton, &QPushButton::clicked, this, &SalesmanDashboard::editMenuItem);

    QDir dir;
    if (!dir.exists("logs")) {
        dir.mkdir("logs");
    }

    setWindowTitle("Salesman Dashboard - Alsyed Pakwan");
    resize(1200, 800); // Increased window size to accommodate larger menu
}

void SalesmanDashboard::setTheme()
{
    setStyleSheet(
        "QMainWindow { background-color: #1a1a1a; }"
        "QLabel { color: white; }"
        "QLineEdit { background-color: #2d2d2d; border: 2px solid #4CAF50; border-radius: 5px; color: white; padding: 5px; }"
        "QSpinBox { background-color: #2d2d2d; border: 2px solid #4CAF50; border-radius: 5px; color: white; padding: 5px; }"
        "QPushButton { background-color: #4CAF50; border: 2px solid #45a049; border-radius: 8px; color: white; font-weight: bold; padding: 8px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }"
        "QListWidget { background-color: #2d2d2d; color: white; border: 2px solid #4CAF50; }"
        "QTextEdit { background-color: #2d2d2d; color: white; border: 2px solid #4CAF50; }"
        "QMenuBar { background-color: #2d2d2d; color: white; }"
        "QMenu { background-color: #2d2d2d; color: white; }"
        "QMenu::item:selected { background-color: #4CAF50; }"
        );
}

QLabel* SalesmanDashboard::createLogo()
{
    QLabel* logo = new QLabel(this);
    logo->setFixedSize(80, 80);
    logo->setStyleSheet(
        "QLabel {"
        "    background-color: #4CAF50;"
        "    border: 3px solid white;"
        "    border-radius: 40px;"
        "    color: white;"
        "    font-weight: bold;"
        "    font-size: 16px;"
        "}"
        );
    logo->setText("AP");
    logo->setAlignment(Qt::AlignCenter);
    return logo;
}

bool SalesmanDashboard::initializeDatabase()
{
    static int connectionCount = 0;
    QString connectionName = QString("salesman_dashboard_%1").arg(connectionCount++);
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("alsyed_pakwan.db");
    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS menu_items ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "price REAL NOT NULL, "
                    "category TEXT)")) {
        qDebug() << "Create menu table error:" << query.lastError().text();
        return false;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS orders ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "item_name TEXT NOT NULL, "
                    "quantity INTEGER NOT NULL, "
                    "total REAL NOT NULL, "
                    "order_date TEXT)")) {
        qDebug() << "Create orders table error:" << query.lastError().text();
        return false;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS daily_summary ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "date TEXT NOT NULL, "
                    "total_sales REAL NOT NULL)")) {
        qDebug() << "Create daily_summary table error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool SalesmanDashboard::saveOrderToDatabase(const QList<OrderItem>& order)
{
    db.transaction();
    QSqlQuery query(db);
    for (const OrderItem& item : order) {
        query.prepare("INSERT INTO orders (item_name, quantity, total, order_date) "
                      "VALUES (:item_name, :quantity, :total, :order_date)");
        query.bindValue(":item_name", item.name);
        query.bindValue(":quantity", item.quantity);
        query.bindValue(":total", item.total);
        query.bindValue(":order_date", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        if (!query.exec()) {
            qDebug() << "Save order error:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }
    db.commit();
    return true;
}

bool SalesmanDashboard::saveDailySummary()
{
    db.transaction();
    QSqlQuery query(db);
    query.prepare("INSERT INTO daily_summary (date, total_sales) VALUES (:date, :total_sales)");
    query.bindValue(":date", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    query.bindValue(":total_sales", orderTotal);
    bool success = query.exec();
    db.commit();
    return success;
}

void SalesmanDashboard::loadMenuItems()
{
    menuItems.clear();
    QSqlQuery query(db);
    if (query.exec("SELECT id, name, price, category FROM menu_items")) {
        int row = 0, col = 0;
        while (query.next()) {
            int id = query.value(0).toInt();
            MenuItem item;
            item.name = query.value(1).toString();
            item.price = query.value(2).toDouble();
            item.category = query.value(3).toString();
            menuItems.append(item);

            QPushButton *menuButton = new QPushButton(item.name + " (Rs. " + QString::number(item.price, 'f', 2) + ")", this);
            menuButton->setMinimumSize(150, 50);
            menuButton->setProperty("itemId", id);
            menuGrid->addWidget(menuButton, row, col);
            connect(menuButton, &QPushButton::clicked, [this, index = menuItems.size() - 1]() {
                selectedMenuItem = index;
                selectedMenuItemForEdit = index;
                if (index >= 0 && index < menuItems.size()) {
                    menuItemEditNameEdit->setText(menuItems[index].name);
                    menuItemEditPriceEdit->setText(QString::number(menuItems[index].price, 'f', 2));
                }
            });

            col++;
            if (col > 3) { // Increased to 4 columns for better visibility
                col = 0;
                row++;
            }
        }
    } else {
        qDebug() << "Load menu items error:" << query.lastError().text();
    }

    if (menuItems.isEmpty()) {
        QSqlQuery insertQuery(db);
        QStringList defaultItems = {
            "Chicken Biryani,250.00,Main Course",
            "Mutton Karahi,300.00,Main Course",
            "Beef Pulao,200.00,Main Course",
            "Naan,20.00,Bread",
            "Roti,15.00,Bread",
            "Raita,50.00,Side",
            "Salad,40.00,Side",
            "Kheer,80.00,Dessert",
            "Falooda,100.00,Dessert",
            "Chicken Tikka,150.00,Grill"
        };
        int row = 0, col = 0;
        for (const QString& itemData : defaultItems) {
            QStringList parts = itemData.split(",");
            if (parts.size() == 3) {
                insertQuery.prepare("INSERT INTO menu_items (name, price, category) VALUES (:name, :price, :category)");
                insertQuery.bindValue(":name", parts[0]);
                insertQuery.bindValue(":price", parts[1].toDouble());
                insertQuery.bindValue(":category", parts[2]);
                if (!insertQuery.exec()) {
                    qDebug() << "Insert menu item error:" << insertQuery.lastError().text();
                } else {
                    MenuItem item;
                    item.name = parts[0];
                    item.price = parts[1].toDouble();
                    item.category = parts[2];
                    menuItems.append(item);

                    QPushButton *menuButton = new QPushButton(item.name + " (Rs. " + QString::number(item.price, 'f', 2) + ")", this);
                    menuButton->setMinimumSize(150, 50);
                    menuButton->setProperty("itemId", insertQuery.lastInsertId().toInt());
                    menuGrid->addWidget(menuButton, row, col);
                    connect(menuButton, &QPushButton::clicked, [this, index = menuItems.size() - 1]() {
                        selectedMenuItem = index;
                        selectedMenuItemForEdit = index;
                        if (index >= 0 && index < menuItems.size()) {
                            menuItemEditNameEdit->setText(menuItems[index].name);
                            menuItemEditPriceEdit->setText(QString::number(menuItems[index].price, 'f', 2));
                        }
                    });

                    col++;
                    if (col > 3) {
                        col = 0;
                        row++;
                    }
                }
            }
        }
    }
}

void SalesmanDashboard::addMenuItem()
{
    QString name = menuItemNameEdit->text().trimmed();
    bool ok;
    double price = menuItemPriceEdit->text().toDouble(&ok);
    if (name.isEmpty() || !ok || price <= 0) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid name and price!");
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO menu_items (name, price, category) VALUES (:name, :price, :category)");
    query.bindValue(":name", name);
    query.bindValue(":price", price);
    query.bindValue(":category", "Custom");
    if (query.exec()) {
        MenuItem item;
        item.name = name;
        item.price = price;
        item.category = "Custom";
        menuItems.append(item);

        int row = menuGrid->rowCount();
        int col = 0;
        QPushButton *menuButton = new QPushButton(name + " (Rs. " + QString::number(price, 'f', 2) + ")", this);
        menuButton->setMinimumSize(150, 50);
        menuButton->setProperty("itemId", query.lastInsertId().toInt());
        menuGrid->addWidget(menuButton, row, col);
        connect(menuButton, &QPushButton::clicked, [this, index = menuItems.size() - 1]() {
            selectedMenuItem = index;
            selectedMenuItemForEdit = index;
            if (index >= 0 && index < menuItems.size()) {
                menuItemEditNameEdit->setText(menuItems[index].name);
                menuItemEditPriceEdit->setText(QString::number(menuItems[index].price, 'f', 2));
            }
        });

        menuItemNameEdit->clear();
        menuItemPriceEdit->clear();
        QMessageBox::information(this, "Success", "Menu item added!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to add menu item!");
    }
}

void SalesmanDashboard::editMenuItem()
{
    if (selectedMenuItemForEdit < 0 || selectedMenuItemForEdit >= menuItems.size()) {
        QMessageBox::warning(this, "Edit Error", "Please select a menu item to edit!");
        return;
    }

    QString newName = menuItemEditNameEdit->text().trimmed();
    bool ok;
    double newPrice = menuItemEditPriceEdit->text().toDouble(&ok);
    if (newName.isEmpty() || !ok || newPrice <= 0) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid name and price!");
        return;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE menu_items SET name = :name, price = :price WHERE id = (SELECT id FROM menu_items WHERE name = :oldName LIMIT 1)");
    query.bindValue(":name", newName);
    query.bindValue(":price", newPrice);
    query.bindValue(":oldName", menuItems[selectedMenuItemForEdit].name);
    if (query.exec()) {
        menuItems[selectedMenuItemForEdit].name = newName;
        menuItems[selectedMenuItemForEdit].price = newPrice;

        // Update the button text
        for (int i = 0; i < menuWidget->children().size(); ++i) {
            QPushButton *button = qobject_cast<QPushButton*>(menuWidget->children()[i]);
            if (button && button->text().startsWith(menuItems[selectedMenuItemForEdit].name)) {
                button->setText(newName + " (Rs. " + QString::number(newPrice, 'f', 2) + ")");
                break;
            }
        }

        selectedMenuItem = selectedMenuItemForEdit; // Keep selection
        menuItemEditNameEdit->clear();
        menuItemEditPriceEdit->clear();
        selectedMenuItemForEdit = -1;
        QMessageBox::information(this, "Success", "Menu item updated!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to update menu item!");
    }
}

void SalesmanDashboard::updateBillPreview()
{
    QString billText = "BILL\n";
    billText += "------------------------\n";
    billText += "Customer: " + customerNameEdit->text() + "\n";
    billText += "Date: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n";
    billText += "------------------------\n";
    for (const OrderItem& item : currentOrder) {
        billText += QString("%1 x %2 = Rs. %3\n").arg(item.quantity).arg(item.name).arg(item.total, 0, 'f', 2);
    }
    billText += "------------------------\n";
    billText += "Total: Rs. " + QString::number(orderTotal, 'f', 2) + "\n";
    billText += "Received: Rs. " + QString::number(receivedAmount, 'f', 2) + "\n";
    billText += "Change: Rs. " + QString::number(changeAmount, 'f', 2) + "\n";
    billText += "------------------------\n";
    billPreview->setText(billText);
}

QString SalesmanDashboard::generateSummaryText()
{
    QString summary = "DAILY SUMMARY\n";
    summary += "------------------------\n";
    summary += "Date: " + QDateTime::currentDateTime().toString("yyyy-MM-dd") + "\n";
    summary += "Total Sales: Rs. " + QString::number(orderTotal, 'f', 2) + "\n";
    summary += "------------------------\n";
    return summary;
}

void SalesmanDashboard::addToOrder()
{
    if (selectedMenuItem < 0 || selectedMenuItem >= menuItems.size()) return;

    MenuItem item = menuItems[selectedMenuItem];
    int quantity = quantitySpinBox->value();
    double total = item.price * quantity;

    OrderItem orderItem;
    orderItem.name = item.name;
    orderItem.quantity = quantity;
    orderItem.total = total;
    currentOrder.append(orderItem);

    orderTotal += total;
    orderList->addItem(QString("%1 x %2 = Rs. %3").arg(quantity).arg(item.name).arg(total, 0, 'f', 2));
    totalLabel->setText("Total: Rs. " + QString::number(orderTotal, 'f', 2));
    updateBillPreview();
}

void SalesmanDashboard::removeFromOrder()
{
    int row = orderList->currentRow();
    if (row < 0 || row >= currentOrder.size()) return;

    orderTotal -= currentOrder[row].total;
    currentOrder.removeAt(row);
    delete orderList->takeItem(row);
    totalLabel->setText("Total: Rs. " + QString::number(orderTotal, 'f', 2));
    updateBillPreview();
}

void SalesmanDashboard::clearOrder()
{
    currentOrder.clear();
    orderTotal = 0.0;
    receivedAmount = 0.0;
    changeAmount = 0.0;
    orderList->clear();
    totalLabel->setText("Total: Rs. 0.00");
    receivedLabel->setText("Received: Rs. 0.00");
    changeLabel->setText("Change: Rs. 0.00");
    receivedAmountEdit->clear();
    updateBillPreview();
}

void SalesmanDashboard::generateBill()
{
    customerName = customerNameEdit->text().trimmed();
    if (customerName.isEmpty() || currentOrder.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter customer name and add items to order!");
        return;
    }
    if (saveOrderToDatabase(currentOrder)) {
        QMessageBox::information(this, "Success", "Bill generated and saved!");
        updateBillPreview();
    } else {
        QMessageBox::critical(this, "Error", "Failed to save bill!");
    }
}

void SalesmanDashboard::printBill()
{
    if (currentOrder.isEmpty()) {
        QMessageBox::warning(this, "Print Error", "No order to print!");
        return;
    }
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QPrintDialog::Accepted) {
        QTextDocument *doc = new QTextDocument();
        doc->setHtml(billPreview->toHtml());
        doc->print(&printer);
        delete doc;
    }
}

void SalesmanDashboard::generateDailySummary()
{
    if (saveDailySummary()) {
        QMessageBox::information(this, "Success", generateSummaryText());
    } else {
        QMessageBox::critical(this, "Error", "Failed to save daily summary!");
    }
}

void SalesmanDashboard::logout()
{
    MainWindow *mainWindow = new MainWindow(this->parentWidget());
    mainWindow->show();
    this->close();
}

void SalesmanDashboard::onOrderItemClicked(QListWidgetItem *item)
{
    int row = orderList->row(item);
    if (row >= 0 && row < currentOrder.size()) {
        selectedMenuItem = -1; // Reset selection
    }
}

void SalesmanDashboard::updateReceivedAmount(const QString &text)
{
    bool ok;
    receivedAmount = text.toDouble(&ok);
    if (ok) {
        changeAmount = receivedAmount - orderTotal;
        receivedLabel->setText("Received: Rs. " + QString::number(receivedAmount, 'f', 2));
        changeLabel->setText("Change: Rs. " + QString::number(changeAmount, 'f', 2));
    } else {
        receivedAmount = 0.0;
        changeAmount = 0.0 - orderTotal;
        receivedLabel->setText("Received: Rs. 0.00");
        changeLabel->setText("Change: Rs. " + QString::number(changeAmount, 'f', 2));
    }
    updateBillPreview();
}

void SalesmanDashboard::showAboutDialog()
{
    QMessageBox::about(this, "About", "Salesman Dashboard\nDeveloped by " + developerName + "\nVersion 1.0");
}
