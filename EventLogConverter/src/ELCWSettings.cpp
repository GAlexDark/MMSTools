#include "ELCWSettings.h"

static ELCWSettings g_elcwSettings;

ELCWSettings& ELCWSettings::instance()
{
    return g_elcwSettings;
}

void
ELCWSettings::createDefault(const QString& iniPath)
{
    ELCSettings::createDefault(iniPath);

    QSettings settings(iniPath, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("HISTORY"));
    settings.setValue(QStringLiteral("last_dir"), QStringLiteral(""));
    settings.endGroup();
}
