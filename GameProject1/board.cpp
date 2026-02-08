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

void Board::buildAdjacency()
{
    for(auto* n: m_nodes) n->neighbors.clear();

    const int R = m_rows.size();
    auto rowIsA = [&](int r)->bool{
        if(m_rows[r].isEmpty()) return true;
        return m_rows[r][0].startsWith('A');
    };

    auto addEdge = [&](const QString& from, const QString& to){
        CellNode* a = get(from);
        CellNode* b = get(to);
        if(!a || !b) return;
        if(!a->neighbors.contains(b)) a->neighbors.push_back(b);
        if(!b->neighbors.contains(a)) b->neighbors.push_back(a);
    };

    for(int r=0;r<R;++r){
        bool isA = rowIsA(r);
        const auto &row = m_rows[r];

        for(int c=0;c<row.size();++c){
            if(c-1>=0) addEdge(row[c], row[c-1]);
            if(c+1<row.size()) addEdge(row[c], row[c+1]);

            auto addIf = [&](int rr, int cc){
                if(rr<0||rr>=R) return;
                const auto &rrw = m_rows[rr];
                if(cc<0||cc>=rrw.size()) return;
                addEdge(row[c], rrw[cc]);
            };

            if(isA){
                addIf(r-1, c-1); addIf(r-1, c);
                addIf(r+1, c-1); addIf(r+1, c);
            }else{
                addIf(r-1, c);   addIf(r-1, c+1);
                addIf(r+1, c);   addIf(r+1, c+1);
            }
        }
    }
}
