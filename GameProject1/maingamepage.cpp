#include "maingamepage.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QResizeEvent>
#include <QPainter>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QLinearGradient>
#include <QDebug>

static QColor baseColorForRowId(const QString& id){
    if(id.startsWith('A')) return QColor("#f1c40f");
    return QColor("#9b59b6");
}

static QColor colorByShield(QColor base, int shield){
    if(shield==1) return base.darker(115);
    if(shield==2) return base.darker(135);
    return base;
}
static QString fileNextToExe(const QString& name)
{
    return QDir(QCoreApplication::applicationDirPath()).filePath(name);
}

static QString exeDirPath()
{
    return QCoreApplication::applicationDirPath();
}

static QString pickExisting(const QString& a, const QString& b)
{
    if (QFileInfo::exists(a)) return a;
    return b; // اگر a نبود، b رو برمی‌گردونه (بعداً error می‌گیریم اگر این هم نبود)
}
MainGamePage::MainGamePage(const QString &player1,
                           const QString &player2,
                           int mapNumber,
                           QWidget *parent)
    : QWidget(parent)
    , m_p1(player1)
    , m_p2(player2)
    , m_mapNumber(mapNumber + 1)
{
    setWindowTitle("UNDAUNTED - Phase 2");
    setWindowState(Qt::WindowMaximized);

    buildUI();
    loadAll();
    rebuildScene();
    gs.initNewGame();
    setMode(Mode::Idle, "Choose an action for the current card.");
}
