#include "board.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStringConverter>

Board::~Board() { clear(); }

void Board::clear()
{
    qDeleteAll(m_nodes);
    m_nodes.clear();
    m_rows.clear();
}
