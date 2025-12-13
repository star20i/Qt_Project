#include "splashscreen.h"
#include "ui_splashscreen.h"
#include "secondpage.h"
#include <QMessageBox>

SplashScreen::SplashScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SplashScreen)
{
    ui->setupUi(this);

    this->setWindowTitle("game");
    this->setFixedSize(800, 600);

    this->setStyleSheet(
        "SplashScreen {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                               stop:0 #1a1a2e, stop:1 #16213e);"
        "}"
        );

    ui->pushButtonStart->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #00b09b, stop:1 #96c93d);"
        "   color: white;"
        "   font-size: 28px;"
        "   font-weight: bold;"
        "   border: 4px solid #ffffff;"
        "   border-radius: 25px;"
        "   padding: 20px 40px;"
        "   min-width: 150px;"
        "   min-height: 60px;"
        "   box-shadow: 0 10px 20px rgba(0, 176, 155, 0.4);"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #00d4b1, stop:1 #a8e05c);"
        "   box-shadow: 0 12px 25px rgba(0, 176, 155, 0.6);"
        "   border: 4px solid #f0f0f0;"
        "}"
        "QPushButton:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #009982, stop:1 #7da835);"
        "   box-shadow: 0 5px 15px rgba(0, 176, 155, 0.4);"
        "   border: 4px solid #e0e0e0;"
        "}"
        );
}

SplashScreen::~SplashScreen()
{
    delete ui;
}

void SplashScreen::on_pushButtonStart_clicked()
{

    QMessageBox::information(this, "start", "welcome to the game!\n");

    SecondPage *secondPage = new SecondPage();
    secondPage->show();
    this->close();
}
