#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <random>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QFont>
#include <QLabel>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnInventoryPage, &QPushButton::clicked, this, &MainWindow::on_btnInventoryPage_clicked);
    connect(ui->btnMainPage, &QPushButton::clicked, this, &MainWindow::on_btnMainPage_clicked);

    initialiseDatabase();





    setUpTimer();
    setRollBtnStylesheet();
    setCoinDisplayStylesheet();
    insertCurrencyToPlayerFinance(20);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initialiseDatabase(){
    // Connect to Database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("fumbler.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
                              QObject::tr("Unable to establish a database connection Click Cancel to exit."), QMessageBox::Cancel);
        return ;
    }else{
        qDebug() << "Connected to Database";
    }


    QSqlQuery query(db);
    query.exec("PRAGMA foreign_keys = ON;");

    query.exec("CREATE TABLE IF NOT EXISTS Items ("
               "itemID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "itemName TEXT UNIQUE, "
               "itemClass TEXT, "
               "itemRarity TEXT NOT NULL CHECK (itemRarity IN ('Common', 'Uncommon', 'Rare', 'Epic', 'FUMBLER')), "
               "itemDescription TEXT NOT NULL);");

    query.exec("CREATE TABLE IF NOT EXISTS PlayerInfo ("
               "playerID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "playerName TEXT NOT NULL UNIQUE, "
               "playerGender TEXT CHECK (playerGender IN ('Male', 'Female', 'Bisaya')), "
               "playerAge INTEGER NOT NULL);");

    query.exec("CREATE TABLE IF NOT EXISTS PlayerFinance ("
               "playerID INTEGER PRIMARY KEY, "
               "FC INTEGER, "
               "FOREIGN KEY (playerID) REFERENCES PlayerInfo(playerID));");

    query.exec("CREATE TABLE IF NOT EXISTS Inventory ("
               "inventoryID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "playerID INTEGER, "
               "itemID INTEGER, "
               "FOREIGN KEY (itemID) REFERENCES Items(itemID), "
               "FOREIGN KEY (playerID) REFERENCES PlayerInfo(playerID));");

    query.exec("INSERT INTO PlayerInfo (playerName, playerGender, playerAge) VALUES"
               "('Jibril', 'Bisaya', 19);");

    query.exec("INSERT INTO Items (itemName, itemClass, itemRarity, itemDescription) VALUES "
               // "('1 BL', 'Currency', 'Epic', 'How Much is a Bisaya Worth? 1 Bisaya Life that is!!!'), "
               "('10 FC', 'Currency', 'Common', 'The Fumbler King must Fall'), "
               "('5 FC', 'Currency', 'Common', 'The Amount of times he has talked to a girl'), "
               "('2 FC', 'Currency', 'Common', 'Twice has he Fumbled'), "
               "('1 FC', 'Currency', 'Common', 'How many times has the fumbler king fumbled?'), "
               "('Josue', 'Pet', 'Common', 'A Random Bisaya'), "
               "('White Shirt', 'Equipabble', 'Common', 'Nauubos ba ang white shirt ni gabby?'), "
               "('Ballpen ni Godoy', 'Item', 'Common', 'Di pa nasusuli ni Gabby'), "
               "('BSU Aircon', 'Item', 'Uncommon', 'is it Hot outside? The AC will help to make it hotter'), "
               "('Lolo John''s Skelan', 'Consumable', 'Uncommon', 'Arthritis? Gout? Mag Lolo John''s Skelan na!'), "
               "('Thirst Trap ni Aldrin', 'Item', 'Uncommon', 'Sino ba may gustong makakita?'), "
               "('Shampoo ni Melito', 'Consumable', 'Uncommon', 'Need niya ba?'), "
               "('Deniggafication Potion', 'Consumable', 'Uncommon', 'Losses your N word pass'), "
               "('Batombakal''s Submarine', 'Item', 'Uncommon', 'Baha? Need a ride? Batombakal''s submarine is here for you.'), "
               "('Lumang Laptop ni Vince', 'Item', 'Rare', 'Mas Mabilis pa magsulat sa Papel'), "
               "('Baha sa Hagonoy', 'Equipabble', 'Rare', 'Hindi Nawawala'), "
               "('Backpack ng Labhigh', 'Item', 'Rare', 'RICHMOND BAT KA MAY BAG NG LABHIGH!!!'), "
               "('Pants ni Josue', 'Equipabble', 'Rare', 'Stretchable kahit na fit size'), "
               "('Loafers ni Father', 'Equipabble', 'Rare', 'An ancient Bisaya shoe passed down in an unknown family for many generations.'), "
               "('Roblox Incident', 'Consumable', 'Epic', 'The Deus Ex Machina of a Bisaya when everything is against you even your ally, you shall win.'), "
               "('Nov 1 incident', 'Item', 'Epic', 'You''d rather not know'), "
               "('Chinese Flag', 'Consumable', 'Epic', 'Summons an Aldrin to your side'), "
               "('Richmond''s Hair', 'Equipabble', 'Epic', 'Can Change ones identity'), "
               "('RNG luck ni Vince', 'Consumable', 'FUMBLER', 'Gives infinite luck for 5 seconds'), "
               "('CP ni Josue', 'Item', 'FUMBLER', 'Anak ng Nokia 3310. Mas mahal pa repair sa phone'), "
               "('Aso ni Meldie', 'Pet', 'FUMBLER', 'Mas matalino pa sayo sa calculus'), "
               "('PC ni Blunder King', 'Item', 'FUMBLER', 'A PC that can defy Logic just to stay broken'), "
               "('Dragon ni Gabby', 'Consumable', 'FUMBLER', 'Once Consumed you will play like Chovy''s Asol');");


    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

}


QString getDataFromDatabase(int inputNumber) {
    QString result = "";

    // Prepare the query
    QSqlQuery query;
    query.prepare("SELECT itemName FROM Items WHERE itemID = :id");
    query.bindValue(":id", inputNumber);  // Bind the input number

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return "Query execution failed";
    }

    // Retrieve the result if any
    if (query.next()) {
        QString itemName = query.value(0).toString();


        // Build the result string
        result = "Item Name: " + itemName;
        qDebug() << "Item Name: " + itemName;

    } else {
        result = "No data found for the given ID.";
    }

    return result;
}

void MainWindow::insertItemToInventory(int itemID){
    QSqlQuery query;
    query.prepare("INSERT INTO Inventory (playerID, itemID) VALUES (:playerID, :itemID)");

    // Bind the values to the placeholders
    query.bindValue(":playerID", 1);
    query.bindValue(":itemID", itemID);

    if (!query.exec()) {
        qDebug() << "Error inserting item into inventory:" << query.lastError().text();
    } else {
        qDebug() << "Item inserted successfully into inventory.";
    }
}


void MainWindow::insertCurrencyToPlayerFinance(int coins){
    QSqlQuery query;
    query.prepare("SELECT FC FROM PlayerFinance WHERE playerID = :playerID");
    query.bindValue(":playerID", 1);

    if (!query.exec()) {
        qDebug() << "Error fetching Coins from Finance:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        // Row exists, update the FC value
        int currentCoins = query.value(0).toInt();
        int newCoins = currentCoins + coins;

        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE PlayerFinance SET FC = :FC WHERE playerID = :playerID");
        updateQuery.bindValue(":FC", newCoins);
        updateQuery.bindValue(":playerID", 1);  // Assuming playerID is 1

        if (!updateQuery.exec()) {
            qDebug() << "Error updating Coins in Finance:" << updateQuery.lastError().text();
        } else {
            qDebug() << "Coins Added successfully in Finance. New FC:" << newCoins;
            ui->textBrowserCoinsDisplay->setText(QString::number(newCoins));
        }
    } else {
        // Row does not exist, insert new row
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO PlayerFinance (playerID, FC) VALUES (:playerID, :FC)");
        insertQuery.bindValue(":playerID", 1);  // Assuming playerID is 1
        insertQuery.bindValue(":FC", coins);

        if (!insertQuery.exec()) {
            qDebug() << "Error inserting Coins into Finance:" << insertQuery.lastError().text();
        } else {
            qDebug() << "Coins inserted successfully into Finance.";
        }
    }
}


bool MainWindow::removeCurrencyToPlayerFinance(int coins){
    QSqlQuery query;
    query.prepare("SELECT FC FROM PlayerFinance WHERE playerID = :playerID");
    query.bindValue(":playerID", 1);

    if (!query.exec()) {
        qDebug() << "Error fetching Coins from Finance:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        // Row exists, update the FC value
        int currentCoins = query.value(0).toInt();
        int newCoins = currentCoins - coins;

        if(currentCoins < coins){
            return false;
        }else{
            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE PlayerFinance SET FC = :FC WHERE playerID = :playerID");
            updateQuery.bindValue(":FC", newCoins);
            updateQuery.bindValue(":playerID", 1);

            if (!updateQuery.exec()) {
                qDebug() << "Error subtracting Coins in Finance:" << updateQuery.lastError().text();
                return false;
            } else {
                qDebug() << "Coins Subtracted Successfully, Remaining:" << newCoins;
                ui->textBrowserCoinsDisplay->setText(QString::number(newCoins));
                return true;
            }

        }

    } else {
        // Row does not exist, insert new row
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO PlayerFinance (playerID, FC) VALUES (:playerID, :FC)");
        insertQuery.bindValue(":playerID", 1);
        insertQuery.bindValue(":FC", coins);

        if (!insertQuery.exec()) {
            qDebug() << "Error inserting Coins into Finance:" << insertQuery.lastError().text();
        } else {
            qDebug() << "Coins inserted successfully into Finance.";
        }
    }
}


void MainWindow::checkCoinValue(int coins){
    int coinValue = 0;
    switch(coins){
    case 1:
        coinValue = 10;
        insertCurrencyToPlayerFinance(10);
            break;

    case 2:
        coinValue = 5;
        insertCurrencyToPlayerFinance(5);
        break;

    case 3:
        coinValue = 2;
        insertCurrencyToPlayerFinance(2);
        break;

    case 4:
        coinValue = 1;
        insertCurrencyToPlayerFinance(1);
        break;
    }
}

int randomNumberGenerator(int lowerLimit, int upperLimit){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(lowerLimit, upperLimit);
    int rngNumber = dis(gen);
    return rngNumber;
}

void MainWindow::displayNameAndSave(int itemID){

    // Get data from database
    QString result = getDataFromDatabase(itemID);
    // Update the label with the result
    ui->labelResult->setText(result);
    // Insert Data into Inventory
    insertItemToInventory(itemID);
}

void MainWindow::displayName(int itemID){

    // Get data from database
    QString result = getDataFromDatabase(itemID);
    // Update the label with the result
    ui->labelResult->setText(result);
}




QStringList MainWindow::getItemsFromInventory(){
    QStringList itemInventory;
    // Query the items in the inventory
    QSqlQuery query;
    query.prepare("SELECT itemID FROM Inventory");

    if (!query.exec()) {
        qDebug() << "Error fetching items: " << query.lastError().text();
    }

    // Loop through the items and add them to the layout
    while (query.next()) {
        int itemID = query.value(0).toInt();;

        QString itemName = getDataFromDatabase(itemID); // Get item name from the database
        if (!itemName.isEmpty()) {
            itemInventory.append(itemName); // Add the item name to the list
        } else {
            qDebug() << "Error fetching item name for itemID:" << itemID;
        }

    }
    return itemInventory;
}


void MainWindow::displayItemsInIventory(QStringList &itemInventory){

    // Clear the existing layout first
    QLayout* existingLayout = ui->scrollAreaInventoryPage->layout();
    if (existingLayout) {
        QLayoutItem* item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            delete item->widget(); // Delete each widget
            delete item;           // Delete layout item
        }
        delete existingLayout;      // Delete the old layout itself
    }

    // Now set the new layout
    QVBoxLayout *layout = new QVBoxLayout();

    // Loop through the QStringList and create labels for each item
    for (const QString &itemName : itemInventory) {
        // Debug the item name
        qDebug() << "Displaying item: " << itemName;

        // Create a QLabel for the item
        QLabel *nameLabel = new QLabel(itemName);
        nameLabel->setAlignment(Qt::AlignCenter); // Center the text in the label
        nameLabel->setStyleSheet("border: 1px solid #e7af40; padding: 5px; background-color: #f5f5f5;");

        // Add the label to the layout
        layout->addWidget(nameLabel);
    }

    // Set the layout to the scroll area
    QWidget *inventoryWidget = new QWidget();
    inventoryWidget->setLayout(layout);
    ui->scrollAreaInventoryPage->setWidget(inventoryWidget);


}


void MainWindow::on_btnRoll_clicked() {
    // Random Number Generator


    if(removeCurrencyToPlayerFinance(5)){
        int rarityNumber = randomNumberGenerator(1, 100);
        qDebug() << "Roll Number: " << rarityNumber;

        if (rarityNumber <= 35) {
            qDebug() << "Rarity Value: Common";  // 50% chance for Common
            int itemRoll = randomNumberGenerator(1, 4);
            changeCardImage(itemRoll);
            qDebug() << "Item Name Roll: "<< itemRoll;
            checkCoinValue(itemRoll);
            displayName(itemRoll);
        } else if(rarityNumber <= 15) {
            qDebug() << "Rarity Value: Common";  // 50% chance for Common
            int itemRoll = randomNumberGenerator(5, 7);
            changeCardImage(itemRoll);
            qDebug() << "Item Name Roll: "<< itemRoll;
            displayNameAndSave(itemRoll);
        } else if (rarityNumber <= 80) {
            qDebug() << "Rarity Value: Uncommon";  // 30% chance for Uncommon
            int itemRoll = randomNumberGenerator(8, 13);
            changeCardImage(itemRoll);
            qDebug() << "Item Name Roll: "<< itemRoll;
            displayNameAndSave(itemRoll);
        } else if (rarityNumber <= 95) {
            qDebug() << "Rarity Value: Rare";  // 15% chance for Rare
            int itemRoll = randomNumberGenerator(14, 17);
            changeCardImage(itemRoll);
            qDebug() << "Item Name Roll: "<< itemRoll;
            displayNameAndSave(itemRoll);
        } else if (rarityNumber <= 99) {
            qDebug() << "Rarity Value: Epic";  // 4% chance for Epic
            int itemRoll = randomNumberGenerator(18, 21);
            changeCardImage(itemRoll);
            qDebug() << "Item Name Roll: "<< itemRoll;
            displayNameAndSave(itemRoll);
        } else {
            qDebug() << "Rarity Value: Fumbler";  // 1% chance for Fumbler
            int itemRoll = randomNumberGenerator(22, 26);
            changeCardImage(itemRoll);
            qDebug() << "Item Name Roll: "<< itemRoll;
            displayNameAndSave(itemRoll);
        }

    }else{
        qDebug() << "No Fumbler Coins";
    }

}



void MainWindow::on_btnInventoryPage_clicked(){
    ui->stackedWidget->setCurrentIndex(1);
    QStringList inventoryList = getItemsFromInventory();
    displayItemsInIventory(inventoryList);
}


void MainWindow::on_btnMainPage_clicked(){
    ui->stackedWidget->setCurrentIndex(0);

}

void MainWindow::changeCardImage(int itemID){
    QString itemIDString = QString::number(itemID);

    QString resourcePath = ":/cards/" + itemIDString + ".png";

    // Check if the resource exists
    if (!QFile::exists(resourcePath)) {
        qDebug() << "Resource file not found at:" << resourcePath;
    } else {
        qDebug() << "Resource file found at:" << resourcePath;

        // Attempt to load the image
        QPixmap testImage(resourcePath);
        if (testImage.isNull()) {
            qDebug() << "Failed to load the image from resource!";
        } else {
            qDebug() << "Image loaded successfully!";
            ui->labelCardImage->setPixmap(testImage);
            ui->labelCardImage->setScaledContents(true); // Optional: ensure image scales with label
        }
    }

}

void MainWindow::setCoinDisplayStylesheet(){

    QTextBrowser *textBrowserCoins = ui->textBrowserCoinsDisplay;

    textBrowserCoins->setStyleSheet(
        "QTextBrowser {"
        "   border: 2px solid #e7af40;"
        "   border-radius: 5px;"
        "   background: qlineargradient("
        "   spread: pad, "
        "   x1: 0, y1: 0, x2: 1, y2: 1, "
        "   stop: 0 #fedd7a, "
        "   stop: 1 #fec949); "
        "   padding: 5px;"
        "   color: #FFFFFF;"
        "   font-size: 24px;"
        "   font-weight: bold;"

        "}"
        "QScrollBar:vertical, QScrollBar:horizontal {"
        "   width: 0px;"  // Hides the scrollbar
        "   height: 0px;"
        "}"
        );

    // Create a timer for the pulsing effect
    QTimer *textBrowserTimer = new QTimer(this);

    // Define gradient states using hex codes
    QString textBrowserGradientStart = "background: qlineargradient("
                                       "spread: pad, "
                                       "x1: 0, y1: 0, x2: 1, y2: 1, "
                                       "stop: 0 #fec949, "
                                       "stop: 1 #fedd7a);";

    QString textBrowserGradientEnd = "background: qlineargradient("
                                     "spread: pad, "
                                     "x1: 0, y1: 0, x2: 1, y2: 1, "
                                     "stop: 0 #fedd7a, "
                                     "stop: 1 #fec949);";

    // Connect the timer to update the QTextBrowser's style sheet
    connect(textBrowserTimer, &QTimer::timeout, [textBrowserCoins, textBrowserGradientStart, textBrowserGradientEnd]() {
        static bool toggle = false; // Toggle state
        QString textBrowserGradient = toggle ? textBrowserGradientStart : textBrowserGradientEnd;
        textBrowserCoins->setStyleSheet(
            "QTextBrowser {"
            + textBrowserGradient +
            "border: 2px solid #e7af40;"  // Border color
            "border-radius: 5px;"
            "color: #FFFFFF;"
            "font-size: 24px;"
            "font-weight: bold;"
            "padding: 5px;"
            "}"
            );
        toggle = !toggle; // Switch state
    });

    // Start the timer with a 500 ms interval
    textBrowserTimer->start(500);

}

void MainWindow::setRollBtnStylesheet(){
    QPushButton *btnRoll = ui->btnRoll;
    QFont btnRollFont = btnRoll->font();
    btnRollFont.setPointSize(18);
    btnRoll->setFont(btnRollFont);
    // Set the initial style sheet
    btnRoll->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient("
        "        spread: pad, "
        "        x1: 0, y1: 0, x2: 1, y2: 1, "
        "        stop: 0 #aca3da, "
        "        stop: 1 #a0c1ca); "
        "    color: white;"
        "    border-radius: 10px;"
        "    padding: 10px;"
        "}"
        );

    // Create a timer for the pulsing effect
    QTimer *timer = new QTimer(this);

    // Define gradient states using hex codes
    QString gradientStart = "background: qlineargradient("
                            "spread: pad, "
                            "x1: 0, y1: 0, x2: 1, y2: 1, "
                            "stop: 0 #aca3da, "
                            "stop: 1 #a0c1ca);";

    QString gradientEnd = "background: qlineargradient("
                          "spread: pad, "
                          "x1: 0, y1: 0, x2: 1, y2: 1, "
                          "stop: 0 #a0c1ca, "
                          "stop: 1 #aca3da);";

    // Connect the timer to update the button's style sheet
    connect(timer, &QTimer::timeout, [btnRoll, gradientStart, gradientEnd]() {
        static bool toggle = false; // Toggle state
        QString newGradient = toggle ? gradientStart : gradientEnd;
        btnRoll->setStyleSheet(
            "QPushButton {"
            + newGradient +
            "border: 2px solid white;"  // Green border color
            "border-radius: 5px;"
            "color: white;"
            "border-radius: 10px;"
            "padding: 10px;"
            "}"
            "QPushButton:hover {"
            "border: 2px solid #ecc01f;"
            "}"

            );
        toggle = !toggle; // Switch state
    });

    // Start the timer with a 500 ms interval
    timer->start(500);

}

void MainWindow::setUpTimer() {
    // Initialize the iteration count
    iterationCount = 0;

    // Create a new QTimer
    timer = new QTimer(this);

    // Connect the timer's timeout signal to your slot
    connect(timer, &QTimer::timeout, this, &MainWindow::handleTimerEvent);

    timer->start(30000);

    qDebug() << "Timer started. It will run for" << maxIterations << "minutes.";
}

void MainWindow::handleTimerEvent() {
    // Increment the iteration count
    iterationCount++;

    // Send the debug message
    insertCurrencyToPlayerFinance(5);

    // Stop the timer if the maximum number of iterations is reached
    if (iterationCount >= maxIterations) {
        timer->stop();
        qDebug() << "Timer stopped after" << maxIterations << "iterations.";
    }
}
