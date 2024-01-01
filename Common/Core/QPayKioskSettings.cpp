#include "QPayKioskSettings.h"
#include <QDir>
#include <QFileInfo>
#include "Debug.h"

QPayKioskSettings::QPayKioskSettings() :
    m_settings(nullptr) {}

QPayKioskSettings::~QPayKioskSettings()
{
    delete m_settings;
}

bool
QPayKioskSettings::init(const QString& appPath, const QString &fileName)
{
    QString iniFileName = QDir(appPath).filePath(fileName);

    QFileInfo fileInfo(iniFileName);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        createDefault(iniFileName);
    }

    delete m_settings;

    bool retVal = true;
    try {
        m_settings = new QSettings(iniFileName, QSettings::IniFormat);
    } catch (const std::bad_alloc& ex) {
        __DEBUG( ex.what() )
        retVal = false;
    }
    return retVal;
}

QVariant
QPayKioskSettings::getMain(const QString& keyName) const
{
    return (m_settings) ? m_settings->value(keyName) : QVariant();
}

QString
QPayKioskSettings::defaultClientLang(){
    return getMain("user/default_lang").toString();
}

