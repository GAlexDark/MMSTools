#include <QCoreApplication>
#include <QtTest>

#include "CElcGuiAppSettings.h"

// add necessary includes here

const QString fileName = QLatin1String("EventLogConverter.ini");
const QString filePath = QStringLiteral(SRCDIR"data");

class CElcGuiAppSettingsTest : public QObject
{
    Q_OBJECT

public:
    CElcGuiAppSettingsTest();
    ~CElcGuiAppSettingsTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_init();
    void test_fileExists();
    void test_getDBName();
    void test_checkLastDir();
};

CElcGuiAppSettingsTest::CElcGuiAppSettingsTest() {}

CElcGuiAppSettingsTest::~CElcGuiAppSettingsTest() {}

void
removeIniFile()
{
    QString fp = QDir(filePath).filePath(fileName);
    QFile file(fp);
    if (file.exists()) {
        file.remove();
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
    CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    QString retVal = settings.getMain(QLatin1String("SETTINGS/db_file_name")).toString().trimmed();
    QCOMPARE(retVal, QString(TEST_SRCDIR"EventLogConverter.db"));
}

void
CElcGuiAppSettingsTest::test_checkLastDir()
{
    CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    settings.setMain(QLatin1String("HISTORY"), QLatin1String("last_dir"), filePath);
    QString lastDir = settings.getMain(QLatin1String("HISTORY/last_dir")).toString().trimmed();
    QCOMPARE(lastDir, filePath);
}

QTEST_MAIN(CElcGuiAppSettingsTest)

#include "tst_celcguiappsettingstest.moc"
