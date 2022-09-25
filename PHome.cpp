#include "PHome.h"

#include <QFile>
#include <QTextStream>
#include <QTranslator>

void PHome::LoadTranslation(QApplication* app, QLocale locale)
{
    Q_INIT_RESOURCE(pluginHome);

    QTranslator* appTranslator = new QTranslator();
    if (appTranslator->load(locale, QLatin1String("pluginHome"), QLatin1String("_"), QLatin1String(":/i18n"))) {
        app->installTranslator(appTranslator);
    }
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
