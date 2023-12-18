#include <QtTest>
#include <QFile>
#include <QDebug>

#include "ELCSettings.h"

// add necessary includes here

class ELCSettingsTest : public QObject
{
    Q_OBJECT
public:
    ELCSettingsTest();
    ~ELCSettingsTest();

private slots:
    void test_init();
    void test_fileExists();
    void test_getDBName();
    void test_getMode();
};

ELCSettingsTest::ELCSettingsTest()
{
    QFile file(SRCDIR"data/EventLogConverter.ini");
    if (file.exists()) {
        file.remove();
    }
}

ELCSettingsTest::~ELCSettingsTest()
{
    QFile file(SRCDIR"data/EventLogConverter.ini");
    if (file.exists()) {
        file.remove();
    }
}

void
ELCSettingsTest::test_init()
{
    QString iniFile = QStringLiteral("EventLogConverter.ini");
    QString appPath = SRCDIR"data";
    bool retVal = ELCSettings::instance().init(appPath, iniFile);
    QVERIFY(retVal);
}

void
ELCSettingsTest::test_fileExists()
{
    QFile file(SRCDIR"data/EventLogConverter.ini");
    QVERIFY(file.exists());
}

void
ELCSettingsTest::test_getDBName()
{
    ELCSettings &settings = ELCSettings::instance();
    QString retVal = settings.getMain("SETTINGS/db_file_name").toString().trimmed();
    QCOMPARE(retVal, QString(TEST_SRCDIR"EventLogConverter.db"));
}

void
ELCSettingsTest::test_getMode()
{
    ELCSettings &settings = ELCSettings::instance();
    QString retVal = settings.getMain("SETTINGS/mode").toString().trimmed();
    QCOMPARE(retVal, QString("simple_report"));
}

QTEST_APPLESS_MAIN(ELCSettingsTest)

#include "tst_elcsettings.moc"
