#include <QCoreApplication>
#include <QtTest>

#include "CELCWSettings.h"

// add necessary includes here

const QString fileName = QStringLiteral("EventLogConverter.ini");
const QString filePath = QStringLiteral(SRCDIR"data");

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
    QString fp = QDir(filePath).filePath(fileName);
    QFile file(fp);
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
    bool retVal = ELCWSettings::instance().init(filePath, fileName, false);
    QVERIFY(retVal);
}

void
CELCWSettingsTest::test_fileExists()
{
    QString fp = QDir(filePath).filePath(fileName);
    qDebug() << fp;
    QFile file(fp);
    bool retVal = file.exists();
    QVERIFY(retVal);
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
