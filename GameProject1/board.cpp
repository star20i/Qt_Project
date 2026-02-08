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

bool Board::loadMapFile(const QString &filename)
{
    clear();

    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) return false;

    QTextStream in(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    // |A01:0|  (ID + shield 0..2)
    QRegularExpression re("\\|\\s*([A-Z]\\d{2,3})\\s*:\\s*([0-2])");

    while(!in.atEnd()){
        QString line = in.readLine();
        if(line.trimmed().isEmpty()) continue;

        QVector<QString> rowIds;
        auto it = re.globalMatch(line);
        while(it.hasNext()){
            auto m = it.next();
            QString id = m.captured(1);
            int sh = m.captured(2).toInt();

            if(!m_nodes.contains(id)){
                auto* n = new CellNode();
                n->id = id;
                n->shield = sh;
                m_nodes.insert(id, n);
            }
            rowIds.push_back(id);
        }
        if(!rowIds.isEmpty()) m_rows.push_back(rowIds);
    }

    return !m_rows.isEmpty();
}
