#ifndef MAINGAMEPAGE_H
#define MAINGAMEPAGE_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QColor>

class QGraphicsView;
class QGraphicsScene;
class QLabel;
class QPushButton;
class QResizeEvent;
class QGraphicsRectItem;
class QFont;

struct TileCell
{
    QString id;
    int value;

    QColor baseColorForRow() const {
        QChar letter = id.isEmpty() ? QChar('A') : id.at(0).toUpper();
        return (letter == 'A') ? QColor("#f1c40f") : QColor("#9b59b6");
    }
};

class MainGamePage : public QWidget
{
    Q_OBJECT

public:
    explicit MainGamePage(const QString &player1,
                          const QString &player2,
                          int mapIndex,
                          QWidget *parent = nullptr);
    ~MainGamePage();

    void resizeEvent(QResizeEvent *event) override;

private:
    bool loadMap(int mapIndex);
    void buildBoard();

    bool computeTileMetrics(double &tileSize,
                            double &spacing,
                            int &rows,
                            int &maxCols) const;

    bool isARow(const QVector<TileCell> &row) const;
    double rowXOffset(bool isARow, double tileSize) const;

    QColor tileBaseColor(const TileCell &cell) const;
    QColor tileColorByValue(QColor base, int value) const;
    QBrush tileBrush(const TileCell &cell, double tileSize) const;

    QGraphicsRectItem* createTile(const TileCell &cell,
                                  double x, double y,
                                  double tileSize,
                                  const QFont &idFont,
                                  const QFont &valFont);

    void addTileTexts(QGraphicsRectItem *rect,
                      const TileCell &cell,
                      double x, double y,
                      double tileSize,
                      const QFont &idFont,
                      const QFont &valFont);

    void finalizeSceneBounds();

private:
    QString m_player1;
    QString m_player2;
    int m_mapIndex;

    QVector<QVector<TileCell>> m_board;

    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QLabel *m_labelInfo;
    QLabel *m_titleLabel;
    QPushButton *m_startButton;
};

#endif
