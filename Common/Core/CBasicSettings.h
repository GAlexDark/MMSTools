/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2017-2024
*  Contact: galexsoftware@gmail.com
*
*  Common module
*  Common module
*
*  Module name: CBasicSettings.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The base class for the read and write settings from the ini file.
*
****************************************************************************/

#ifndef CBASICSETTINGS_H
#define CBASICSETTINGS_H

#include <QSettings>
#include <QString>
#include <QVariant>

class CBasicSettings
{
public:
    explicit CBasicSettings();
    ~CBasicSettings();

public:
    bool init(const QString& appPath, const QString &fileName, bool isTerminalMode);
    QVariant getMain(const QString& keyName) const;
    void setMain(const QString& group, const QString& keyName, const QVariant& value);
    bool isRdsEnabled() const { return m_isRdsEnabled; }

protected:
    virtual void createDefault(const QString& iniPath) =0;
    bool        m_isRdsEnabled;

private:
    QSettings*  m_settings;

};

#endif // CBASICSETTINGS_H
