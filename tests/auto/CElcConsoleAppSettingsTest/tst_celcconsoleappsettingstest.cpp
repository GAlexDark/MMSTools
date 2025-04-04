#include <QtTest>
#include <QFile>
#include <QDebug>

#include "CElcConsoleAppSettings.h"

inline const QString fileName = QLatin1String("EventLogConverterc.ini");
inline const QString filePath = QStringLiteral(SRCDIR"data");

class CElcConsoleAppSettingsTest : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_init();
    void test_fileExists();
    void test_getDBName();
};

void
removeIniFile()
{
    QString fp = QDir(filePath).filePath(fileName);
    QFile file(fp);
    if (file.exists()) {
        bool retVal = file.remove();
        Q_ASSERT(retVal);
        qDebug() << "file removed";
    }
}

void
CElcConsoleAppSettingsTest::initTestCase()
{
    removeIniFile();
}

void
CElcConsoleAppSettingsTest::cleanupTestCase()
{
    removeIniFile();
}

void
CElcConsoleAppSettingsTest::test_init()
{
    qDebug() << filePath;
    qDebug() << fileName;
    bool retVal = CElcConsoleAppSettings::instance().init(filePath, fileName, false);
    QVERIFY(retVal);
}

void
CElcConsoleAppSettingsTest::test_fileExists()
{
    QString fp = QDir(filePath).filePath(fileName);
    qDebug() << fp;
    QFile file(fp);
    bool retVal = file.exists();
    QVERIFY(retVal);
}

void
CElcConsoleAppSettingsTest::test_getDBName()
{
    const CElcConsoleAppSettings &settings = CElcConsoleAppSettings::instance();
    QString retVal = settings.getDbFileName();
    QCOMPARE(retVal, QString(TEST_SRCDIR"EventLogConverter.db"));
}

QTEST_APPLESS_MAIN(CElcConsoleAppSettingsTest)

#include "tst_celcconsoleappsettingstest.moc"
