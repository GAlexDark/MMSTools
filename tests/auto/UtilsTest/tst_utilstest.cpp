#include <QtTest>
#include <QDebug>
#include <QFile>

#include "elcUtils.h"
#include "CTestClass.h"
// add necessary includes here

const int filesCount = 16;

void
createTestFile(const QString &fileName)
{
    QFile f(fileName);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (f.open(QIODeviceBase::WriteOnly)) {
#else
    if (f.open(QIODevice::WriteOnly)) {
#endif
        f.write("test");
        f.close();
    }
}

class UtilsTest : public QObject
{
    Q_OBJECT

public:
    explicit UtilsTest(QObject *parent = nullptr);

private:
    QString     m_basePath,
                m_newDir;
private slots:
    void test_parseValuesList1();
    void test_parseValuesList2();
    void test_expandEnvironmentStrings();
#ifdef Q_OS_WIN
    void test_getWindowsApiErrorMessage();
#endif

    void initTestCase();
    void cleanupTestCase();
    void test_dataSourceListCount();
    void test_sanitizeValue();
    void test_getMetaClassInfo();
};

UtilsTest::UtilsTest(QObject *parent)
    : QObject{parent}, m_basePath(SRCDIR"data"), m_newDir("source")
{}

void
UtilsTest::test_parseValuesList1()
{
    qDebug() << "elcUtils::parseValuesList(QString data)";

    QString data;
    data.clear();
    QStringList retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 0);

    data = "user1;user2;user3;user4";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = data + ';';
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = data + ";user3";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = "user1;user2;user3,user4";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 4);

    retVal.clear();
    data = "user1";
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 1);

    retVal.clear();
    data = data + ';';
    retVal = elcUtils::parseValuesList(data);
    QCOMPARE(retVal.size(), 1);
}

void
UtilsTest::test_parseValuesList2()
{
    qDebug() << "elcUtils::parseValuesList(QStringList data)";

    QStringList data;
    data.clear();
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 0);

    data.clear();
    data << "user1" << "user2" << "user3" << "user4";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1" << "user2" << "user3" << "" <<"user4" << "";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1" << "user2" << "user3" << "" <<"user4" << "" << "user2";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 1);
    QCOMPARE(data.at(0), "user1");

    data.clear();
    data << "user1;";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 1);
    QCOMPARE(data.at(0), "user1");

    data.clear();
    data << "user1;user2;user3;user4";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1;user2;user3;user4;";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1;user2;user3;user4;;user3";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1;user2,user3;user4;";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 4);

    data.clear();
    data << "user1";
    elcUtils::parseValuesList(data);
    QCOMPARE(data.size(), 1);
}

void
UtilsTest::test_expandEnvironmentStrings()
{
#ifdef Q_OS_WIN
    QString data = QStringLiteral("%PUBLIC%\\temp\\eventlogreader");
    elcUtils::expandEnvironmentStrings(data);
    qDebug() << data;
    QCOMPARE(data, "C:/Users/Public/temp/eventlogreader");

    data = QStringLiteral("%PUBLIC%/temp/eventlogreader");
    elcUtils::expandEnvironmentStrings(data);
    qDebug() << data;
    QCOMPARE(data, "C:/Users/Public/temp/eventlogreader");
#else
    QString data = QStringLiteral("$HOME\\temp\\eventlogreader");
#endif
}

#ifdef Q_OS_WIN
void
UtilsTest::test_getWindowsApiErrorMessage()
{

    QString retVal = elcUtils::getWindowsApiErrorMessage(0);
    qDebug() << retVal;
    QCOMPARE(retVal.length(), 29);
}
#endif

void
UtilsTest::initTestCase()
{
    QDir dir(m_basePath);
    bool retVal = dir.mkdir(m_newDir);
    if (retVal) {
        QString tstPath = m_basePath + QStringLiteral("/") + m_newDir + QStringLiteral("/file");
        tstPath = QDir::fromNativeSeparators(tstPath);
        QString fileTempl = QStringLiteral("%1%2.csv");
        for (int i = 0; i < filesCount; ++i) {
            createTestFile(fileTempl.arg(tstPath).arg(i));
        }
    }
    QVERIFY(retVal);
}

void
UtilsTest::cleanupTestCase()
{
    QString tstPath = m_basePath + QStringLiteral("/") + m_newDir;
    QDir dir(tstPath);
    dir.removeRecursively();
}

void
UtilsTest::test_dataSourceListCount()
{
    QString tstPath = m_basePath + QStringLiteral("/") + m_newDir;
    QStringList retVal = elcUtils::getDataSourceList(tstPath, QStringList() << "*.csv");
    QCOMPARE(retVal.size(), filesCount);
}

void
UtilsTest::test_sanitizeValue()
{
    QVERIFY(elcUtils::sanitizeValue(QLatin1String("^([0-9.]+)$"), "10.10.10.10"));
    QVERIFY(!elcUtils::sanitizeValue(QLatin1String("^([0-9.]+)$"), "l0.10.10.10"));

    const QStringList allowedValues = { "value1", "value2", "value3" };
    QString retVal = elcUtils::sanitizeValue("value2", allowedValues, "value1");
    QCOMPARE(retVal, "value2");

    retVal = elcUtils::sanitizeValue("va1ue2", allowedValues, "value1");
    QCOMPARE(retVal, "value1");
}

void
UtilsTest::test_getMetaClassInfo()
{
    CTestClass testClass;
    QString value;
    bool retVal = elcUtils::getMetaClassInfo(&testClass, "prorerty", value);
    QCOMPARE(value, "propertyvalue");
    QVERIFY(retVal);
}

QTEST_APPLESS_MAIN(UtilsTest)

#include "tst_utilstest.moc"
