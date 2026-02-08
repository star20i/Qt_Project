#ifndef BOARD_H
#define BOARD_H

#include "cellnode.h"
#include <QHash>
#include <QVector>
#include <QString>

class Board {

public:
    ~Board();

    bool loadMapFile(const QString& filename);
    void buildAdjacency();

    CellNode* get(const QString& id) const { return m_nodes.value(id, nullptr); }
    const QHash<QString, CellNode*>& nodes() const { return m_nodes; }
    const QVector<QVector<QString>>& rows() const { return m_rows; }

private:
    QHash<QString, CellNode*> m_nodes;
    QVector<QVector<QString>> m_rows;

    void clear();
};

#endif
