/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2017-2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: CBasicSettings.cpp
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for the read and write settings from the ini-file.
*
****************************************************************************/

#include "CBasicSettings.h"
#include <QDir>
#include <QFileInfo>

CBasicSettings::~CBasicSettings()
{
    if (m_settings) {
        delete m_settings;
        m_settings = nullptr;
    }
}

bool
CBasicSettings::init(const QString& appPath, const QString &fileName, bool isTerminalMode)
{
    m_isRdsEnabled = isTerminalMode;
    QString iniFileName = QDir(appPath).filePath(fileName);

    QFileInfo fileInfo(iniFileName);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        createDefault(iniFileName);
    }

    delete m_settings;

    bool retVal = true;
    try {
        m_settings = new QSettings(iniFileName, QSettings::IniFormat);
        Q_CHECK_PTR(m_settings);
    } catch (const std::bad_alloc& ex) {
        retVal = false;
    }

    return retVal;
}

QVariant
CBasicSettings::getMain(const QString& keyName) const
{
    return m_settings ? m_settings->value(keyName) : QVariant();
}

void
CBasicSettings::setMain(const QString& group, const QString& keyName, const QVariant& value)
{
    if (m_settings) {
        m_settings->beginGroup(group);
        m_settings->setValue(keyName, value);
        m_settings->endGroup();
    }
}
