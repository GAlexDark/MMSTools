#include <QtTest>
#include <QFile>
#include <QDebug>

#include "CELCCSettings.h"

// add necessary includes here

const QString fileName = QStringLiteral("EventLogConverterc.ini");
const QString filePath = QStringLiteral(SRCDIR"data");

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
    QString fp = QDir(filePath).filePath(fileName);
    QFile file(fp);
    if (file.exists()) {
        file.remove();
        qDebug() << "file removed";
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
    qDebug() << filePath;
    qDebug() << fileName;
    bool retVal = CELCCSettings::instance().init(filePath, fileName, false);
    QVERIFY(retVal);
}

void
CELCCSettingsTest::test_fileExists()
{
    QString fp = QDir(filePath).filePath(fileName);
    qDebug() << fp;
    QFile file(fp);
    bool retVal = file.exists();
    QVERIFY(retVal);
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
