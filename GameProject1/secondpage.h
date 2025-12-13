#ifndef SECONDPAGE_H
#define SECONDPAGE_H

#include <QWidget>
#include <QString>

class QLineEdit;
class QLabel;
class QPushButton;

class SecondPage : public QWidget
{
    Q_OBJECT

public:
    explicit SecondPage(QWidget *parent = nullptr);
    ~SecondPage();

private slots:
    void onSelectMapClicked();

private:
    bool validatePlayerName(const QString &name, QString &errorMessage);

    QLineEdit *lineEditPlayer1;
    QLineEdit *lineEditPlayer2;
    QLabel *labelTitle;
    QLabel *labelPlayer1;
    QLabel *labelPlayer2;
    QLabel *labelError;
    QPushButton *pushButtonSelectMap;
};
#endif
