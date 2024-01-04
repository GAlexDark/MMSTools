#include <QtTest>
#include <QFile>
#include <QDebug>

#include "CELCCSettings.h"

// add necessary includes here

class CELCCSettingsTest : public QObject
{
    Q_OBJECT
public:
    CELCCSettingsTest();
    ~CELCCSettingsTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_init();
    void test_fileExists();
    void test_getDBName();
    //void test_getMode();
};

CELCCSettingsTest::CELCCSettingsTest() {}

CELCCSettingsTest::~CELCCSettingsTest() {}

void
removeIniFile()
{
    QFile file(SRCDIR"data/EventLogConverter.ini");
    if (file.exists()) {
        file.remove();
    }
}

void
CELCCSettingsTest::initTestCase()
{
    removeIniFile();
}

void
CELCCSettingsTest::cleanupTestCase()
{
    removeIniFile();
}

void
CELCCSettingsTest::test_init()
{
    QString iniFile = QStringLiteral("EventLogConverter.ini");
    QString appPath = SRCDIR"data";
    bool retVal = CELCCSettings::instance().init(appPath, iniFile);
    QVERIFY(retVal);
}

void
CELCCSettingsTest::test_fileExists()
{
    QFile file(SRCDIR"data/EventLogConverter.ini");
    QVERIFY(file.exists());
}

void
CELCCSettingsTest::test_getDBName()
{
    CELCCSettings &settings = CELCCSettings::instance();
    QString retVal = settings.getMain("SETTINGS/db_file_name").toString().trimmed();
    QCOMPARE(retVal, QString(TEST_SRCDIR"EventLogConverter.db"));
}

QTEST_APPLESS_MAIN(CELCCSettingsTest)

#include "tst_celccsettings.moc"
