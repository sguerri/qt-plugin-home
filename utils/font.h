#ifndef PHOME_UTILS_FONT_H
#define PHOME_UTILS_FONT_H

#include <QFont>

namespace pluginHome::utils
{
    QFont smallerFont(const QFont& font, int diff = 1)
    {
        auto newFont = QFont(font);
        newFont.setPointSize(font.pointSize() - diff);
        return newFont;
    }

    QFont tallerFont(const QFont& font, int diff = 1)
    {
        auto newFont = QFont(font);
        newFont.setPointSize(font.pointSize() + diff);
        return newFont;
    }
}

#endif // PHOME_UTILS_FONT_H
