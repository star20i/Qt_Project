#include "initloader.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStringConverter>

bool InitLoader::applyInitFile(Board &board, const QString &filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly|QIODevice::Text)) return false;

    QTextStream in(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    // CellID:Player,Thing   e.g. A03:A,scout  / A13:A,mark / A15:B,control 9
    QRegularExpression re("^\\s*([AB]\\d{2,3})\\s*:\\s*([AB])\\s*,\\s*([A-Za-z]+)\\s*$");

    while(!in.atEnd()){
        QString line = in.readLine().trimmed();
        if(line.isEmpty()) continue;

        auto m = re.match(line);
        if(!m.hasMatch()) continue;

        QString cellId = m.captured(1);
        Player p = playerFromChar(m.captured(2).at(0));
        QString kind = m.captured(3).trimmed();
        QString lower = kind.toLower();

        CellNode* node = board.get(cellId);
        if(!node) continue;

        if(lower=="mark"){
            if(p==Player::A) node->markedA = true;
            if(p==Player::B) node->markedB = true;
        }else if(lower=="control"){
            node->controlOwner = p;
        }else{
            node->pieceOwner = p;
            node->pieceType  = agentFromString(kind);
        }
    }

    return true;
}
