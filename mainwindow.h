#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QButtonGroup>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent * event);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void setTray();
    void setButton();
    void setTimer();

    void updateTime();

    void on_buttonStart_clicked();

    void writeTimeToEnd();

    void lockInput();

    void powerPC();
    void sleepPC();
    void resetPC();

    void on_buttonCancel_clicked();

private:
    Ui::MainWindow *ui;

    QSystemTrayIcon * trayIcon;

    QTimer* tmr;

    QTimer* timeToPower;

    QButtonGroup actionRadio;

};

#endif // MAINWINDOW_H
