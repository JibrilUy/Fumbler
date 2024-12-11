#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include <QDebug>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVariant>
#include <QPixmap>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore>



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void displayNameAndSave(int itemID);
    void insertCurrencyToPlayerFinance(int coins);
    bool removeCurrencyToPlayerFinance(int coins);
    void insertItemToInventory(int itemID);
    void checkCoinValue(int coins);

    void displayItemsInIventory(QStringList &itemInventory);
    void changeCardImage(int itemID);
    void displayName(int itemID);

private slots:
    void on_btnRoll_clicked();
    void on_btnInventoryPage_clicked();
    void on_btnMainPage_clicked();

    void initialiseDatabase();

    QStringList getItemsFromInventory();


    void setCoinDisplayStylesheet();
    void setRollBtnStylesheet();

    void setUpTimer();
    void handleTimerEvent();
private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    QSqlDatabase db;

    QTimer *timer;

    int iterationCount;
    const int maxIterations = 9999;

};
#endif // MAINWINDOW_H
