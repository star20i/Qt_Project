
#include "secondpage.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QRegularExpression>
#include <QStringList>
#include <QChar>

SecondPage::SecondPage(QWidget *parent) :
    QWidget(parent)
{
    labelTitle = new QLabel("ثبت نام بازیکنان");
    labelPlayer1 = new QLabel("بازیکن ۱:");
    labelPlayer2 = new QLabel("بازیکن ۲:");
    labelError = new QLabel();

    lineEditPlayer1 = new QLineEdit();
    lineEditPlayer2 = new QLineEdit();

    pushButtonSelectMap = new QPushButton("انتخاب نقشه");

    this->setWindowTitle("ثبت نام بازیکنان");
    this->setFixedSize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    this->setStyleSheet(
        "SecondPage {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 #2c3e50, stop:1 #34495e);"
        "}"
        );

    labelTitle->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        "padding: 10px;"
        "}"
        );
    labelTitle->setAlignment(Qt::AlignCenter);

    labelPlayer1->setStyleSheet("QLabel { color: white; font-size: 16px; }");
    labelPlayer2->setStyleSheet("QLabel { color: white; font-size: 16px; }");

    lineEditPlayer1->setStyleSheet(
        "QLineEdit {"
        "padding: 8px;"
        "font-size: 14px;"
        "border: 2px solid #3498db;"
        "border-radius: 5px;"
        "background-color: white;"
        "}"
        );

    lineEditPlayer2->setStyleSheet(
        "QLineEdit {"
        "padding: 8px;"
        "font-size: 14px;"
        "border: 2px solid #3498db;"
        "border-radius: 5px;"
        "background-color: white;"
        "}"
        );



    labelError->setStyleSheet("QLabel { color: red; font-size: 14px; }");
    labelError->setAlignment(Qt::AlignCenter);
    labelError->clear();

    mainLayout->addWidget(labelTitle);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(labelPlayer1);
    layout1->addWidget(lineEditPlayer1);
    mainLayout->addLayout(layout1);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(labelPlayer2);
    layout2->addWidget(lineEditPlayer2);
    mainLayout->addLayout(layout2);

    mainLayout->addWidget(labelError);
    mainLayout->addWidget(pushButtonSelectMap);

    connect(pushButtonSelectMap, &QPushButton::clicked, this, &SecondPage::onSelectMapClicked);
}

SecondPage::~SecondPage(){}

bool SecondPage::validatePlayerName(const QString &name, QString &errorMessage)
{
    const int MIN_LENGTH = 3;

    if (name.length() < MIN_LENGTH) {
        errorMessage = QString("نام باید حداقل %1 کاراکتر باشد").arg(MIN_LENGTH);
        return false;
    }

    if (name[0].isDigit() || !name[0].isLetter()) {
        errorMessage = "نام نباید با عدد یا کاراکتر خاص شروع شود";
        return false;
    }

    bool hasDigit = false;
    bool hasSpecial = false;
    bool hasLower = false;
    bool hasUpper = false;

    for (const QChar &ch : name) {
        if (ch.isDigit()) hasDigit = true;
        if (!ch.isLetterOrNumber() && !ch.isSpace()) hasSpecial = true;
        if (ch.isLower()) hasLower = true;
        if (ch.isUpper()) hasUpper = true;
    }
    if (!hasDigit) {
        errorMessage = "نام باید شامل حداقل یک عدد باشد";
        return false;
    }

    if (!hasSpecial) {
        errorMessage = "نام باید شامل حداقل یک کاراکتر خاص باشد";
        return false;
    }

    if (!hasLower || !hasUpper) {
        errorMessage = "نام باید شامل ترکیبی از حروف کوچک و بزرگ انگلیسی باشد";
        return false;
    }

    return true;
}

void SecondPage::onSelectMapClicked()
{
    QString player1 = lineEditPlayer1->text().trimmed();
    QString player2 = lineEditPlayer2->text().trimmed();

    if (player1.isEmpty() || player2.isEmpty()) {
        labelError->setText("لطفا نام هر دو بازیکن را وارد کنید");
        return;
    }

    QString error1;
    if (!validatePlayerName(player1, error1)) {
        labelError->setText("خطا در نام بازیکن ۱: " + error1);
        return;
    }

    QString error2;
    if (!validatePlayerName(player2, error2)) {
        labelError->setText("خطا در نام بازیکن ۲: " + error2);
        return;
    }

    if (player1 == player2) {
        labelError->setText("نام بازیکنان نمی‌تواند یکسان باشد");
        return;
    }

    labelError->clear();

    QDialog mapDialog(this);
    mapDialog.setWindowTitle("انتخاب نقشه");
    mapDialog.setFixedSize(400, 300);
    mapDialog.setStyleSheet(
        "QDialog {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 #1e3c72, stop:1 #2a5298);"
        "}"
        );

    QVBoxLayout *layout = new QVBoxLayout(&mapDialog);

    QLabel *titleLabel = new QLabel("لطفا یک نقشه انتخاب کنید:");
    titleLabel->setStyleSheet("QLabel { color: white; font-size: 18px; font-weight: bold; padding: 10px; }");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QStringList maps = {
        "map1",
        "map2",
        "map3",
        "map4",
        "map5"
    };

    QStringList descriptions = {
        "اندازه: 5x5 | Tileها: 23",
        "اندازه: 7x7 | Tileها: 46",
        "اندازه: 9x9 | Tileها: 77",
        "اندازه: 6x11 | Tileها: 61",
        "اندازه: 5x9 | Tileها: 41"
    };

    for (int i = 0; i < maps.size(); ++i) {
        QPushButton *mapButton = new QPushButton(maps[i] +"\n" + descriptions[i], &mapDialog);
        mapButton->setStyleSheet(
            "QPushButton {"
            "background-color: #3498db;"
            "color: white;"
            "padding: 5px;"
            "margin: 5px;"
            "border: 2px solid #2980b9;"
            "border-radius: 10px;"
            "}"
            "QPushButton:hover {"
            "background-color: #2980b9;"
            "}"
            );

        connect(mapButton, &QPushButton::clicked, [this, &mapDialog, player1, player2, i]() {
            QMessageBox::information(this, "انتخاب نقشه",QString("نقشه انتخاب شده: نقشه %1\nبازیکن ۱: %2\nبازیکن ۲: %3")
                                                                        .arg(i + 1).arg(player1).arg(player2));



        });

        layout->addWidget(mapButton);
    }

    QPushButton *cancelButton = new QPushButton("انصراف", &mapDialog);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "background-color: #e74c3c;"
        "color: white;"
        "padding: 5px;"
        "margin: 5px;"
        "border: 2px solid #c0392b;"
        "border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "background-color: #c0392b;"
        "}"
        );
    connect(cancelButton, &QPushButton::clicked, &mapDialog, &QDialog::reject);
    layout->addWidget(cancelButton);

    mapDialog.exec();
}
