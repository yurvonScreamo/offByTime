#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QCommonStyle>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/timeIcon.png"));

    setButton();
    setTray();
    setTimer();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete trayIcon;
    delete tmr;
    delete timeToPower;
}

void MainWindow::setTray()
{
    trayIcon = new QSystemTrayIcon(this);

    QCommonStyle style;

    this->setWindowTitle("Выключатель");
    trayIcon->setIcon( QIcon(":/timeIcon.png"));
    trayIcon->setToolTip("Выключатель");

    QMenu* menu = new QMenu(this);
    QAction* viewWindow = new QAction(tr("Развернуть окно"), this);
    QAction* quitAction = new QAction(tr("Выход"), this);

    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    menu->addAction(viewWindow);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::setButton()
{
    actionRadio.addButton(ui->powerPc, 0);
    actionRadio.addButton(ui->resetPc, 1);
    actionRadio.addButton(ui->sleepPc, 2);

    ui->buttonCancel->hide();
}

void MainWindow::setTimer()
{

    ui->labelTimeNow->setText("Сейчас " + QDateTime::currentDateTime().toString());
    ui->dateTime->setDateTime(QDateTime::currentDateTime());

    tmr = new QTimer();

    tmr->setInterval(1000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime()));

    tmr->start();
}

void MainWindow::updateTime()
{
    ui->labelTimeNow->setText("Сейчас " + QDateTime::currentDateTime().toString());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(this->isVisible()){
        event->ignore();
        this->hide();
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger )
       {
            if(!this->isVisible()){
                this->show();
            } else {
                this->hide();
            }
        }
}

void MainWindow::on_buttonStart_clicked()
{

    if (!actionRadio.checkedButton())
    {
        ui->labelSave->setText("Выберите действие");
        return;
    }

    QDateTime dateTime = ui->dateTime->dateTime();

    qint64 time = dateTime.toMSecsSinceEpoch() -  QDateTime::currentMSecsSinceEpoch();

    if(time < 0)
    {
        ui->labelSave->setText("Извините, я не могу совершать действия в прошлом");
        return;
    }

    timeToPower = new QTimer(this);
    timeToPower->setSingleShot(true);

    switch (actionRadio.checkedId())
    {
    case 0: {
        connect(timeToPower,SIGNAL(timeout()), this, SLOT(powerPC()));
        break;
    }

    case 1: {
        connect(timeToPower,SIGNAL(timeout()), this, SLOT(resetPC()));
        break;
    }

    case 2: {
        connect(timeToPower,SIGNAL(timeout()), this, SLOT(sleepPC()));
        break;
    }
    }

    timeToPower->start(time);

    writeTimeToEnd();
    connect(tmr, SIGNAL(timeout()), this, SLOT(writeTimeToEnd()));

    lockInput();

}

void MainWindow::writeTimeToEnd()
{

    QDateTime dateTime = ui->dateTime->dateTime();

    qint64 time = dateTime.toSecsSinceEpoch() - dateTime.toSecsSinceEpoch() % 60  - QDateTime::currentSecsSinceEpoch();

    qint64 timeHour = time / 3600,
           timeMin = (time % 3600) / 60,
           timeSec = (time % 3600) % 60;

    switch (actionRadio.checkedId())
    {
    case 0: {
        ui->labelSave->setText("Ваш компьютер будет выключен через "
                               + QString::number(timeHour) + " часов "
                               + QString::number(timeMin) + " минут "
                               + QString::number(timeSec) + " секунд");
        break;
    }

    case 1: {
        ui->labelSave->setText("Ваш компьютер будет перезагружен через "
                               + QString::number(timeHour) + " часов "
                               + QString::number(timeMin) + " минут "
                               + QString::number(timeSec) + " секунд");
        break;
    }

    case 2: {
        ui->labelSave->setText("Ваш компьютер уйдет в режим сна через "
                               + QString::number(timeHour) + " часов "
                               + QString::number(timeMin) + " минут "
                               + QString::number(timeSec) + " секунд");
        break;
    }
    }
}

void MainWindow::lockInput()
{
    ui->buttonCancel->show();
    ui->buttonStart->setEnabled(false);

    ui->powerPc->setEnabled(false);
    ui->resetPc->setEnabled(false);
    ui->sleepPc->setEnabled(false);

    ui->dateTime->setEnabled(false);
}

void MainWindow::on_buttonCancel_clicked()
{
    ui->buttonCancel->hide();
    ui->labelSave->clear();

    ui->buttonStart->setEnabled(true);
    ui->powerPc->setEnabled(true);
    ui->resetPc->setEnabled(true);
    ui->sleepPc->setEnabled(true);
    ui->dateTime->setEnabled(true);

    disconnect(tmr, SIGNAL(timeout()), this, SLOT(writeTimeToEnd()));

    delete timeToPower;
}

void MainWindow::powerPC()
{
    QString command = "shutdown -s";
    QProcess process;
    #ifdef Q_OS_WIN
        process.start("cmd.exe /C " + command);
    #else
        process.start(command);
    #endif
    process.waitForFinished();
    process.waitForReadyRead();
}

void MainWindow::sleepPC()
{
    QString command = "rundll32 powrprof.dll,SetSuspendState 0,1,0";
    QProcess process;
    #ifdef Q_OS_WIN
        process.start("cmd.exe /C " + command);
    #else
        process.start(command);
    #endif
    process.waitForFinished();
    process.waitForReadyRead();
}

void MainWindow::resetPC()
{
    QString command = " shutdown /r";
    QProcess process;
    #ifdef Q_OS_WIN
        process.start("cmd.exe /C " + command);
    #else
        process.start(command);
    #endif
    process.waitForFinished();
    process.waitForReadyRead();
}
