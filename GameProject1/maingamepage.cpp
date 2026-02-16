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
void MainGamePage::rebuildScene()
{
    m_scene->clear();
    m_ui.clear();

    const auto& rows = gs.board.rows();
    if(rows.isEmpty()) return;

    // dynamic tile size
    QSizeF viewSize = m_view->viewport()->size();
    double spacing = 4.0;

    int maxCols=0;
    for(const auto& r: rows) maxCols = qMax(maxCols, r.size());

    double effCols = maxCols + 0.5;
    double availableW = viewSize.width() - 20;
    double availableH = viewSize.height() - 20;

    double tileX = (availableW - (effCols-1)*spacing)/effCols;
    double tileY = (availableH - (rows.size()-1)*spacing)/rows.size();
    double tile = qMin(tileX, tileY);
    if(tile>110) tile=110;
    if(tile<40) tile=40;

    QFont idFont("Arial", 12, QFont::Bold);
    QFont small("Arial", 8, QFont::Bold);

    for(int r=0;r<rows.size();++r){
        const auto& row = rows[r];
        if(row.isEmpty()) continue;

        bool isA = row[0].startsWith('A');
        double xOffset = isA ? 0.0 : (tile/2.0);
        double y = r*(tile+spacing);

        for(int c=0;c<row.size();++c){
            QString id = row[c];
            CellNode* n = gs.board.get(id);
            if(!n) continue;

            double x = xOffset + c*(tile+spacing);

            QColor base = baseColorForRowId(id);
            QColor col  = colorByShield(base, n->shield);

            QLinearGradient grad(0,0,0,tile);
            grad.setColorAt(0.0, col.lighter(115));
            grad.setColorAt(1.0, col.darker(115));

            auto* rect = m_scene->addRect(QRectF(x,y,tile,tile), QPen(Qt::white,2), QBrush(grad));
            rect->setData(0, id); // store id

            TileUI ui;
            ui.rect = rect;

            auto* idTxt = m_scene->addSimpleText(id, idFont);
            idTxt->setBrush(Qt::white);
            QRectF b = idTxt->boundingRect();
            idTxt->setPos(x + (tile-b.width())/2.0, y + 4);
            ui.idTxt = idTxt;

            auto* shieldTxt = m_scene->addSimpleText(QString::number(n->shield), small);
            shieldTxt->setBrush(Qt::black);
            QRectF sb = shieldTxt->boundingRect();
            shieldTxt->setPos(x + tile - sb.width() - 4, y + tile - sb.height() - 2);
            ui.shieldTxt = shieldTxt;

            ui.markTxt = m_scene->addSimpleText("", small);
            ui.markTxt->setBrush(Qt::white);
            ui.markTxt->setPos(x + 4, y + 2);

            ui.ctrlTxt = m_scene->addSimpleText("", small);
            ui.ctrlTxt->setBrush(Qt::white);
            ui.ctrlTxt->setPos(x + 4, y + tile - 14);

            ui.pieceTxt = m_scene->addSimpleText("", QFont("Arial", 11, QFont::Black));
            ui.pieceTxt->setBrush(Qt::black);
            ui.pieceTxt->setPos(x + 4, y + tile/2.0 - 6);

            m_ui.insert(id, ui);
        }
    }
    // click handling: use scene mousePress by checking items? easiest: install event filter not now
    // Instead: enable item selection and connect via view mousePress:
    // We'll do a simple approach: set rect as selectable and handle in view mousePress event is complex.
    // So: we add a transparent "text" click? We'll use QGraphicsRectItem's built-in mousePress by subclass normally.
    // For now we rely on selectionChanged + data(0). We'll emulate by enabling selectable.
    for(auto it=m_ui.begin(); it!=m_ui.end(); ++it){
        it.value().rect->setFlag(QGraphicsItem::ItemIsSelectable, true);
    }
    connect(m_scene, &QGraphicsScene::selectionChanged, this, [this](){
        auto items = m_scene->selectedItems();
        if(items.isEmpty()) return;
        auto* it = items.first();
        QString id = it->data(0).toString();
        // clear selection so next click works
        it->setSelected(false);
        onTileClicked(id);
    });

    QRectF bounds = m_scene->itemsBoundingRect().adjusted(-10,-10,10,10);
    m_scene->setSceneRect(bounds);

    updateOverlays();
}
QString MainGamePage::pieceLabel(Player owner, AgentType t) const
{
    if(owner==Player::None || t==AgentType::None) return "";
    QString shortT = agentToString(t).left(1).toUpper();
    return QString("%1:%2").arg(playerToChar(owner)).arg(shortT);
}
void MainGamePage::updateOverlays()
{
    // update top turn info
    m_turnInfo->setText(QString("Turn: %1 (%2) | Card: %3")
                            .arg(playerToChar(gs.currentPlayer))
                            .arg(playerName(gs.currentPlayer))
                            .arg(currentCardText()));

    for(auto it=m_ui.begin(); it!=m_ui.end(); ++it){
        const QString id = it.key();
        TileUI &ui = it.value();
        CellNode* n = gs.board.get(id);
        if(!n) continue;

        // mark
        QString m;
        if(n->markedA || n->markedB){
            m = "M:";
            if(n->markedA) m += "A";
            if(n->markedB) m += "B";
        }
        ui.markTxt->setText(m);

        // control
        QString c;
        if(n->controlOwner==Player::A) c="C:A";
        else if(n->controlOwner==Player::B) c="C:B";
        ui.ctrlTxt->setText(c);

        // piece
        ui.pieceTxt->setText(pieceLabel(n->pieceOwner, n->pieceType));
        if(n->pieceOwner==Player::A) ui.pieceTxt->setBrush(QColor("#e84118"));
        else if(n->pieceOwner==Player::B) ui.pieceTxt->setBrush(QColor("#0097e6"));
        else ui.pieceTxt->setBrush(Qt::black);
    }
}

void MainGamePage::setMode(Mode m, const QString &hint)
{
    m_mode = m;
    m_hint->setText(hint);
    clearHighlights();
    m_selectedSource.clear();

    // Special button text to be explicit for presentation
    if(gs.currentCard==AgentType::Scout) m_btnSpecial->setText("Scout (Mark)");
    else if(gs.currentCard==AgentType::Sergeant) m_btnSpecial->setText("Sergeant (Control/Release)");
    else m_btnSpecial->setText("Special");
}

void MainGamePage::clearHighlights()
{
    for(auto it=m_ui.begin(); it!=m_ui.end(); ++it){
        auto &ui = it.value();
        ui.rect->setPen(QPen(Qt::white, 2));
        ui.highlighted = false;
    }
}
void MainGamePage::highlightIds(const QSet<QString> &ids)
{
    clearHighlights();
    for(const auto& id: ids){
        if(m_ui.contains(id)){
            auto &ui = m_ui[id];
            ui.rect->setPen(QPen(QColor("#00ffff"), 4));
            ui.highlighted = true;
        }
    }
}
void MainGamePage::beginMove()
{
    auto sources = gs.sourcesForCurrentCard();
    if(sources.isEmpty()){
        QMessageBox::information(this, "Move", "No piece of this card type on board.");
        return;
    }
    QSet<QString> ids;
    for(const auto& s: sources) ids.insert(s);

    setMode(Mode::PickSourceMove, "Pick your piece to MOVE.");
    highlightIds(ids);
}

void MainGamePage::beginAttack()
{
    auto sources = gs.sourcesForCurrentCard();
    if(sources.isEmpty()){
        QMessageBox::information(this, "Attack", "No attacker of this card type on board.");
        return;
    }
    QSet<QString> ids;
    for(const auto& s: sources) ids.insert(s);

    setMode(Mode::PickSourceAttack, "Pick your piece to ATTACK with.");
    highlightIds(ids);
}
void MainGamePage::beginSpecial()
{
    if(gs.currentCard==AgentType::Scout){
        // pick a scout (must be yours and scout)
        QSet<QString> ids;
        for(auto* n: gs.board.nodes()){
            if(n->pieceOwner==gs.currentPlayer && n->pieceType==AgentType::Scout)
                ids.insert(n->id);
        }
        if(ids.isEmpty()){
            QMessageBox::information(this, "Scout", "No Scout on board.");
            return;
        }
        setMode(Mode::PickScoutForMark, "Pick your SCOUT to mark its current cell (if unmarked).");
        highlightIds(ids);
        return;
    }

    if(gs.currentCard==AgentType::Sergeant){
        QSet<QString> ids;
        for(auto* n: gs.board.nodes()){
            if(n->pieceOwner==gs.currentPlayer && n->pieceType==AgentType::Sergeant)
                ids.insert(n->id);
        }
        if(ids.isEmpty()){
            QMessageBox::information(this, "Sergeant", "No Sergeant on board.");
            return;
        }
        setMode(Mode::PickSergeantForCtrlRel, "Pick your SERGEANT to Control/Release its current cell.");
        highlightIds(ids);
        return;
    }
    QMessageBox::information(this, "Special", "This card has no special action.");
}

void MainGamePage::endTurn()
{
    gs.endTurn();
    setMode(Mode::Idle, "Choose an action for the current card.");
    updateOverlays();
}
void MainGamePage::onTileClicked(const QString &id)
{
    CellNode* n = gs.board.get(id);
    if(!n) return;
