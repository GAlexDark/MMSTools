#include <QCoreApplication>
#include <QtTest>

#include "CELCWSettings.h"

// add necessary includes here

class CELCWSettingsTest : public QObject
{
    Q_OBJECT

public:
    CELCWSettingsTest();
    ~CELCWSettingsTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_init();
    void test_fileExists();
    void test_getDBName();
    void test_checkLastDir();
};

CELCWSettingsTest::CELCWSettingsTest() {}

CELCWSettingsTest::~CELCWSettingsTest() {}

void
removeIniFile()
{
    QFile file(SRCDIR"data/EventLogConverter.ini");
    if (file.exists()) {
        file.remove();
    }
}

void CELCWSettingsTest::initTestCase()
{
    removeIniFile();
}

void CELCWSettingsTest::cleanupTestCase()
{
    removeIniFile();
}

void CELCWSettingsTest::test_init()
{
    QString iniFile = QStringLiteral("EventLogConverter.ini");
    QString appPath = SRCDIR"data";
    bool retVal = ELCWSettings::instance().init(appPath, iniFile);
    QVERIFY(retVal);
}

void
CELCWSettingsTest::test_fileExists()
{
    QFile file(SRCDIR"data/EventLogConverter.ini");
    QVERIFY(file.exists());
}

void
CELCWSettingsTest::test_getDBName()
{
    ELCWSettings &settings = ELCWSettings::instance();
    QString retVal = settings.getMain("SETTINGS/db_file_name").toString().trimmed();
    QCOMPARE(retVal, QString(TEST_SRCDIR"EventLogConverter.db"));
}

void
CELCWSettingsTest::test_checkLastDir()
{
    ELCWSettings &settings = ELCWSettings::instance();
    QString lastDirData = QStringLiteral(SRCDIR"data");
    settings.setMain("HISTORY", "last_dir", lastDirData);
    QString lastDir = settings.getMain("HISTORY/last_dir").toString().trimmed();
    QCOMPARE(lastDir, lastDirData);
}

QTEST_MAIN(CELCWSettingsTest)

#include "tst_celcwsettingstest.moc"
