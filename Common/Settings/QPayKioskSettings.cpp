/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2017
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: QPayKioskSettings.cpp
*  Author(s):
*  Reviewer(s):
*
*  Abstract:
*     Basic class for reading ini-settings
*
****************************************************************************/

#include "QPayKioskSettings.h"
#include <QDir>
#include <QFileInfo>

QPayKioskSettings::QPayKioskSettings() :
    m_settings(NULL) {}

QPayKioskSettings::~QPayKioskSettings()
{
    if (m_settings)
        delete m_settings;
}

bool
QPayKioskSettings::init(const QString& appPath, const QString &fileName)
{
    QString iniFileName = QDir(appPath).filePath(fileName);

    QFileInfo fileInfo(iniFileName);
    if (!fileInfo.exists() || !fileInfo.isFile())
        createDefault(iniFileName);

    if (m_settings)
        delete m_settings;
    m_settings = new QSettings(iniFileName, QSettings::IniFormat);

    return true;
}

QVariant
QPayKioskSettings::getMain(const QString& keyName) const
{
    if (!m_settings)
        return QVariant();
    return m_settings->value(keyName);
}

QString
QPayKioskSettings::defaultClientLang(){
    return getMain("user/default_lang").toString();
}

