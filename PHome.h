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

    static void LoadTranslation(QApplication* app, QLocale locale);
    static QString Theme();
};

#endif // PHOME_H
