#ifndef MAINGAMEPAGE_H
#define MAINGAMEPAGE_H

#include <QWidget>
#include <QString>
#include <QVector>

class QGraphicsView;
class QGraphicsScene;
class QLabel;
class QPushButton;
class QResizeEvent;

struct TileCell
{
    QString id;
    int value;
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
