#ifndef PHOME_H
#define PHOME_H

#include <QObject>
#include <QApplication>

class PHome: public QObject
{
    Q_OBJECT

public:
    PHome();
    ~PHome() {}

    static QTranslator* Translator();
    static QString Theme();
};

#endif // PHOME_H
