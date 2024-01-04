#ifndef KIOSKSETTINGS_H
#define KIOSKSETTINGS_H

#include <QString>
#include <QVariant>
#include <QSettings>

class QPayKioskSettings : public QObject
{
    Q_OBJECT

public:
    QPayKioskSettings();
    ~QPayKioskSettings();

public:
    bool init(const QString& appPath, const QString &fileName);
    QVariant getMain(const QString& keyName) const;
    void setMain(const QString& group, const QString& keyName, QVariant value);

    QString defaultClientLang();

protected:
    virtual void createDefault(const QString& iniPath) =0;

private:
    QSettings* m_settings;
    QString    m_fileName;
};

#endif // KIOSKSETTINGS_H
