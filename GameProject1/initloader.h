#ifndef INITLOADER_H
#define INITLOADER_H

#include "board.h"

class InitLoader {
public:
    static bool applyInitFile(Board& board, const QString& filename);
};

#endif // INITLOADER_H
