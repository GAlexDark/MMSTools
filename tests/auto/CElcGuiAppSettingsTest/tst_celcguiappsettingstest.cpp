#include <QCoreApplication>
#include <QtTest>

#include "CElcGuiAppSettings.h"

const QString fileName = QLatin1String("EventLogConverter.ini");
const QString filePath = QStringLiteral(SRCDIR"data");

class CElcGuiAppSettingsTest : public QObject
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
    void test_checkLastDir();
};

void
removeIniFile()
{
    QString fp = QDir(filePath).filePath(fileName);
    QFile file(fp);
    if (file.exists()) {
        bool retVal = file.remove();
        Q_ASSERT(retVal);
    }
}

void CElcGuiAppSettingsTest::initTestCase()
{
    removeIniFile();
}

void CElcGuiAppSettingsTest::cleanupTestCase()
{
    removeIniFile();
}

void CElcGuiAppSettingsTest::test_init()
{
    bool retVal = CElcGuiAppSettings::instance().init(filePath, fileName, false);
    QVERIFY(retVal);
}

void
CElcGuiAppSettingsTest::test_fileExists()
{
    QString fp = QDir(filePath).filePath(fileName);
    qDebug() << fp;
    QFile file(fp);
    bool retVal = file.exists();
    QVERIFY(retVal);
}

void
CElcGuiAppSettingsTest::test_getDBName()
{
    const CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    QString retVal = settings.getDbFileName();
    QCOMPARE(retVal, QString(TEST_SRCDIR"EventLogConverter.db"));
}

void
CElcGuiAppSettingsTest::test_checkLastDir()
{
    CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    settings.setMain(QLatin1String("HISTORY"), QLatin1String("last_dir"), filePath);
    QString lastDir = settings.getLastDir();
    QCOMPARE(lastDir, filePath);
}

QTEST_MAIN(CElcGuiAppSettingsTest)

#include "tst_celcguiappsettingstest.moc"
