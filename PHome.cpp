#include "PHome.h"

#include <QFile>
#include <QTextStream>
#include <QTranslator>

QTranslator* PHome::Translator()
{
    Q_INIT_RESOURCE(pluginHome);

    QTranslator* appTranslator = new QTranslator();
    if (appTranslator->load(QLocale(), QLatin1String("pluginHome"), QLatin1String("_"), QLatin1String(":/i18n"))) {
        return appTranslator;
    } else {
        // TODO Error message
    }
    return nullptr;
}

QString PHome::Theme()
{
    Q_INIT_RESOURCE(pluginHome);

    QFile fTheme(":/theme/pluginHome.qss");
    fTheme.open(QFile::ReadOnly);
    QTextStream inTheme(&fTheme);
    QString theme = inTheme.readAll();
    fTheme.close();

    return theme;
}
