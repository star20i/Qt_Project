#include "maingamepage.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QMessageBox>
#include <QPainter>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QStringList>
#include <QStringConverter>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QPushButton>
#include <QList>

MainGamePage::MainGamePage(const QString &player1,
                           const QString &player2,
                           int mapIndex,
                           QWidget *parent)
    :QWidget(parent)
    ,m_player1(player1)
    ,m_player2(player2)
    ,m_mapIndex(mapIndex)
    ,m_view(new QGraphicsView(this))
    ,m_scene(new QGraphicsScene(this))
    ,m_labelInfo(new QLabel(this))
    ,m_titleLabel(new QLabel(this))
    ,m_startButton(new QPushButton(this))
{
    setWindowTitle("UNDAUNTED - Main Game");

    setWindowState(Qt::WindowMaximized);

    QLinearGradient bgGrad(0, 0, 0, 800);
    bgGrad.setColorAt(0.0, QColor("#192a56"));
    bgGrad.setColorAt(1.0, QColor("#273c75"));
    m_scene->setBackgroundBrush(bgGrad);

    m_titleLabel->setText("UNDAUNTED");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    QFont warFont("Stencil", 40, QFont::Black);
    warFont.setLetterSpacing(QFont::AbsoluteSpacing, 5);
    m_titleLabel->setFont(warFont);

    m_titleLabel->setStyleSheet(
        "QLabel {"
        "color: #f5f6fa;"
        "background: transparent;"
        "padding: 10px;"
        "}"
        );

    auto *titleEffect = new QGraphicsDropShadowEffect(this);
    titleEffect->setBlurRadius(30);
    titleEffect->setOffset(0, 0);
    titleEffect->setColor(QColor(0, 0, 0, 200));
    m_titleLabel->setGraphicsEffect(titleEffect);

    m_labelInfo->setText(
        QString("Player 1: %1 | Player 2: %2 | Map: %3")
            .arg(m_player1)
            .arg(m_player2)
            .arg(m_mapIndex + 1)
        );
    m_labelInfo->setAlignment(Qt::AlignCenter);
    m_labelInfo->setStyleSheet(
        "QLabel {"
        "color: white;"
        "font-size: 14px;"
        "font-weight: bold;"
        "background-color: #2c3e50;"
        "padding: 6px;"
        "}"
        );

    m_startButton->setText("START GAME");
    m_startButton->setCursor(Qt::PointingHandCursor);
    m_startButton->setFixedSize(220, 52);
    m_startButton->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #00b09b, stop:1 #96c93d);"
        "color: white;"
        "font-size: 22px;"
        "font-weight: bold;"
        "border: 3px solid #ffffff;"
        "border-radius: 12px;"
        "padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #00d4b1, stop:1 #a8e05c);"
        "border: 3px solid #f0f0f0;"
        "}"
        "QPushButton:pressed {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #009982, stop:1 #7da835);"
        "border: 3px solid #e0e0e0;"
        "}"
        );

    connect(m_startButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Start", "Game Started!");
    });

    m_view->setScene(m_scene);
    m_view->setAlignment(Qt::AlignCenter);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setStyleSheet(
        "QGraphicsView {"
        "background-color: transparent;"
        "border: none;"
        "}"
        );

    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    m_view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_labelInfo);
    layout->addWidget(m_startButton);
    layout->addWidget(m_view);

    layout->setAlignment(m_startButton, Qt::AlignCenter);
    layout->setSpacing(4);
    layout->setContentsMargins(8, 8, 8, 8);

    layout->setStretch(0, 0);
    layout->setStretch(1, 0);
    layout->setStretch(2, 0);
    layout->setStretch(3, 1);

    setLayout(layout);

    this->setStyleSheet("background-color: #1e272e;");

    if (loadMap(m_mapIndex)) {
        buildBoard();
    } else {
        QMessageBox::critical(this, "Error", "Failed to load map file.");
    }
}

MainGamePage::~MainGamePage(){}

void MainGamePage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    buildBoard();
}

bool MainGamePage::loadMap(int mapIndex)
{
    QStringList fileNames = { "1.txt", "2.txt", "3.txt", "4.txt", "5.txt" };

    if (mapIndex < 0 || mapIndex >= fileNames.size())
        return false;

    QFile file(fileNames[mapIndex]);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    m_board.clear();

    QRegularExpression re("\\|\\s*([A-Z]\\d{2,3})\\s*:\\s*([0-2])");

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty())
            continue;

        QVector<TileCell> row;
        QRegularExpressionMatchIterator it = re.globalMatch(line);

        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();

            TileCell cell;
            cell.id    = m.captured(1);
            cell.value = m.captured(2).toInt();

            row.append(cell);
        }

        if (!row.isEmpty())
            m_board.append(row);
    }

    return !m_board.isEmpty();
}

void MainGamePage::buildBoard()
{
    m_scene->clear();

    if (m_board.isEmpty())
        return;

    QSizeF viewSize = m_view->viewport()->size();
    if (viewSize.width() <= 50 || viewSize.height() <= 50)
        return;
    const double spacing = 4.0;

    int rows = m_board.size();
    int maxCols = 0;
    for (const auto &row : m_board)
        if (row.size() > maxCols)
            maxCols = row.size();

    double effectiveCols = maxCols + 0.5;

    double availableW = viewSize.width()  - 20;
    double availableH = viewSize.height() - 20;

    double tileSizeX = (availableW - (effectiveCols - 1) * spacing) / effectiveCols;
    double tileSizeY = (availableH - (rows - 1) * spacing) / rows;

    double tileSize = qMin(tileSizeX, tileSizeY);

    if (tileSize > 110) tileSize = 110;
    if (tileSize < 40)  tileSize = 40;

    QFont idFont("Arial", 12, QFont::Bold);
    QFont valFont("Arial", 9, QFont::Bold);

    for (int r = 0; r < m_board.size(); ++r) {
        const QVector<TileCell> &row = m_board[r];
        if (row.isEmpty())
            continue;

        double y = r * (tileSize + spacing);

        QChar letter = row[0].id.at(0).toUpper();
        bool isARow = (letter == 'A');

        QColor colorA("#f1c40f");
        QColor colorB("#9b59b6");

        double xOffset = isARow ? 0.0 : (tileSize / 2.0);

        for (int c = 0; c < row.size(); ++c) {
            const TileCell &cell = row[c];

            QColor base = isARow ? colorA : colorB;

            if (cell.value == 1)
                base = base.darker(115);
            else if (cell.value == 2)
                base = base.darker(135);

            QLinearGradient grad(0, 0, 0, tileSize);
            grad.setColorAt(0.0, base.lighter(115));
            grad.setColorAt(1.0, base.darker(115));

            double x = xOffset + c * (tileSize + spacing);

            QGraphicsRectItem *rect = m_scene->addRect(
                x,y,tileSize,tileSize,QPen(Qt::white, 2),QBrush(grad)
                );

            rect->setData(0, cell.id);
            rect->setData(1, cell.value);

            QGraphicsSimpleTextItem *idTxt =m_scene->addSimpleText(cell.id, idFont);idTxt->setBrush(Qt::white);

            QRectF rRect = rect->rect();
            QRectF tRect = idTxt->boundingRect();

            double idX = x + (rRect.width()  - tRect.width())  / 2.0;
            double idY = y + (rRect.height() - tRect.height()) / 2.0 - 10.0;
            idTxt->setPos(idX, idY);

            QGraphicsSimpleTextItem *valTxt =m_scene->addSimpleText(QString::number(cell.value), valFont);
            valTxt->setBrush(Qt::black);

            QRectF vRect = valTxt->boundingRect();
            double vX = x + rRect.width()  - vRect.width()  - 4.0;
            double vY = y + rRect.height() - vRect.height() - 2.0;
            valTxt->setPos(vX, vY);
        }
    }

    QRectF bounds = m_scene->itemsBoundingRect().adjusted(-10, -10, 10, 10);
    m_scene->setSceneRect(bounds);
}
