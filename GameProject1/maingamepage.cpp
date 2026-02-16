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
MainGamePage::~MainGamePage() {}

void MainGamePage::buildUI()
{
    m_view = new QGraphicsView(this);
    m_scene = new QGraphicsScene(this);
    m_view->setScene(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setStyleSheet("QGraphicsView{background:transparent;border:none;}");
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    m_view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    m_topInfo = new QLabel(this);
    m_topInfo->setAlignment(Qt::AlignCenter);
    m_topInfo->setStyleSheet("QLabel{color:white;font-weight:bold;background:#2c3e50;padding:6px;}");

    m_turnInfo = new QLabel(this);
    m_turnInfo->setAlignment(Qt::AlignCenter);
    m_turnInfo->setStyleSheet("QLabel{color:#00ffff;font-weight:bold;padding:4px;}");

    m_hint = new QLabel(this);
    m_hint->setAlignment(Qt::AlignCenter);
    m_hint->setStyleSheet("QLabel{color:black;padding:4px;}");

    m_btnMove = new QPushButton("Move", this);
    m_btnAttack = new QPushButton("Attack", this);
    m_btnSpecial = new QPushButton("Special", this);
    m_btnEndTurn = new QPushButton("End Turn", this);

    connect(m_btnMove, &QPushButton::clicked, this, &MainGamePage::beginMove);
    connect(m_btnAttack, &QPushButton::clicked, this, &MainGamePage::beginAttack);
    connect(m_btnSpecial, &QPushButton::clicked, this, &MainGamePage::beginSpecial);
    connect(m_btnEndTurn, &QPushButton::clicked, this, &MainGamePage::endTurn);

    auto* btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    btnRow->addWidget(m_btnMove);
    btnRow->addWidget(m_btnAttack);
    btnRow->addWidget(m_btnSpecial);
    btnRow->addWidget(m_btnEndTurn);
    btnRow->addStretch(1);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_topInfo);
    layout->addWidget(m_turnInfo);
    layout->addWidget(m_hint);
    layout->addLayout(btnRow);
    layout->addWidget(m_view);
    layout->setContentsMargins(8,8,8,8);
    setLayout(layout);
}
QString MainGamePage::mapFileName() const
{
    return fileNextToExe(QString("%1.txt").arg(m_mapNumber));  // m_mapNumber باید 1..5 باشد
}

QString MainGamePage::initFileName() const
{
    return fileNextToExe(QString("init%1.txt").arg(m_mapNumber));
}

QString MainGamePage::playerName(Player p) const
{
    if(p==Player::A) return m_p1;
    if(p==Player::B) return m_p2;
    return "-";
}

QString MainGamePage::currentCardText() const
{
    return agentToString(gs.currentCard);
}
void MainGamePage::loadAll()
{

    qDebug() << "EXE DIR =" << QCoreApplication::applicationDirPath();
    qDebug() << "MAP PATH =" << mapFileName();
    qDebug() << "INIT PATH=" << initFileName();
    qDebug() << "mapNumber =" << m_mapNumber;
    qDebug() << "MAP =" << mapFileName();
    qDebug() << "INIT=" << initFileName();

    // map + init + adjacency
    if(!gs.board.loadMapFile(mapFileName())){QMessageBox::critical(this, "Error", "Cannot open map file in working directory.");
        return;
    }
    if(!InitLoader::applyInitFile(gs.board, initFileName())){
        QMessageBox::critical(this, "Error", "Cannot open init file in working directory.");
        return;
    }
    gs.board.buildAdjacency();

    // background
    QLinearGradient bg(0,0,0,800);
    bg.setColorAt(0.0, QColor("#192a56"));
    bg.setColorAt(1.0, QColor("#273c75"));
    m_scene->setBackgroundBrush(bg);

    m_topInfo->setText(QString("Player A: %1 | Player B: %2 | Map: %3")
                           .arg(m_p1).arg(m_p2).arg(m_mapNumber));
}
void MainGamePage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    rebuildScene();
}
