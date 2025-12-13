#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class SplashScreen;
}
QT_END_NAMESPACE

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen();

private:
    Ui::SplashScreen *ui;
};
#endif // SPLASHSCREEN_H
