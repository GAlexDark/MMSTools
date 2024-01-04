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

#include <QString>
#include <QVariant>
#include <QSettings>

class CBasicSettings : public QObject
{
    Q_OBJECT

public:
    CBasicSettings();
    ~CBasicSettings();

public:
    bool init(const QString& appPath, const QString &fileName);
    QVariant getMain(const QString& keyName) const;
    void setMain(const QString& group, const QString& keyName, const QVariant& value);

protected:
    virtual void createDefault(const QString& iniPath) =0;

private:
    QSettings* m_settings;
    QString    m_fileName;
};

#endif // CBASICSETTINGS_H
