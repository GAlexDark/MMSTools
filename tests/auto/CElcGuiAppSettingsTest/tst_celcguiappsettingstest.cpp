#include <QCoreApplication>
#include <QtTest>

#include "CElcGuiAppSettings.h"

// add necessary includes here

inline const QString fileName = QStringLiteral("EventLogConverter.ini");
inline const QString filePath = QStringLiteral(SRCDIR"data");

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
    QString retVal = settings.getMain(SettingsDbFileName).toString().trimmed();
    QCOMPARE(retVal, QString(TEST_SRCDIR"EventLogConverter.db"));
}

void
CElcGuiAppSettingsTest::test_checkLastDir()
{
    CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    settings.setMain(HistoryGroup, KeyLastDir, filePath);
    QString lastDir = settings.getMain(HistoryLastDir).toString().trimmed();
    QCOMPARE(lastDir, filePath);
}

QTEST_MAIN(CElcGuiAppSettingsTest)

#include "tst_celcguiappsettingstest.moc"
