#include <QtTest>
#include <QFile>
#include <QDateTime>

#include "CSVParser.h"
#include "qtestcase.h"

// add necessary includes here

class CSVParserTest : public QObject
{
    Q_OBJECT

public:
    CSVParserTest();
    ~CSVParserTest();

private:
    CSVParser m_parser;

    QString m_username,
            m_timestampISO8601,
            m_requestID,
            m_type,
            m_details,
            m_username1,
            m_authType,
            m_externalIP,
            m_internalIP;
    QDateTime m_timestampTZ;

    void initTestCase();

private slots:
//    void cleanupTestCase();
    void testSuccessAuth_wALL_ips();
    void testFailedAuth();
    void testSuccessAuth_wINTERNAL_ips();
    void testOtherData1();
    void testOtherdata2();
    void testOtherdata3_exception();
    void testDateTimeFormats();
};

CSVParserTest::CSVParserTest() {}

CSVParserTest::~CSVParserTest() {}

void CSVParserTest::initTestCase()
{
    // All fields are not empty
    QFile file(SRCDIR"data/testcase_success_wAll_ips.csv");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);
}

void CSVParserTest::testSuccessAuth_wALL_ips() {
    qDebug() << "Testcase: Success auth with all IPs (internal + external)";

    initTestCase();

    QCOMPARE(m_username, QString("galex"));
    QCOMPARE(m_timestampISO8601, QString("2023-05-09T11:19:57.36Z"));
    QCOMPARE(m_requestID, QString("edbfa4ea24038861"));
    QCOMPARE(m_type, QString("Вхід користувача - успішно"));
    QCOMPARE(m_details, QString("username: galex,@N@  type: PASSWORD,@N@  ip address: 192.0.2.211, 10.10.10.10"));
    QCOMPARE(m_username1, QString("galex"));
    QCOMPARE(m_authType, QString("PASSWORD"));
    QCOMPARE(m_externalIP, QString("192.0.2.211"));
    QCOMPARE(m_internalIP, QString("10.10.10.10"));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-09T14:19:57.360"));
}

void CSVParserTest::testFailedAuth() {
    qDebug() << "Testcase: Failed auth";

    initTestCase(); // for checking empty fields

    QFile file(SRCDIR"data/testcase_failed.csv");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString(""));
    QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:25:46.717Z"));
    QCOMPARE(m_requestID, QString("693d40a5f9e0e2ec"));
    QCOMPARE(m_type, QString("Вхід користувача - невдало"));
    QCOMPARE(m_details, QString("type: PASSWORD@N@  ip address: 10.10.10.10"));
    QCOMPARE(m_username1, QString(""));
    QCOMPARE(m_authType, QString("PASSWORD"));
    QCOMPARE(m_externalIP, QString(""));
    QCOMPARE(m_internalIP, QString("10.10.10.10"));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-23T13:25:46.717"));
}

void CSVParserTest::testSuccessAuth_wINTERNAL_ips() {
    qDebug() << "Testcase: Success auth with internal IPs only";

    initTestCase(); // for checking empty fields

    QFile file(SRCDIR"data/testcase_success_wInt_ips.csv");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString("galex"));
    QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:06.777Z"));
    QCOMPARE(m_requestID, QString("10ff82c8e706c291"));
    QCOMPARE(m_type, QString("Вхід користувача - успішно"));
    QCOMPARE(m_details, QString("username: galex,@N@  type: PASSWORD,@N@  ip address: 10.10.1.13, 10.10.10.10"));
    QCOMPARE(m_username1, QString("galex"));
    QCOMPARE(m_authType, QString("PASSWORD"));
    QCOMPARE(m_externalIP, QString(""));
    QCOMPARE(m_internalIP, QString("10.10.1.13, 10.10.10.10"));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-23T13:40:06.777"));
}

void CSVParserTest::testOtherData1()
{
    qDebug() << "Testcase: Other data #1";

    initTestCase(); // for checking empty fields

    QFile file(SRCDIR"data/testcase_otherdata1.csv");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString("galex"));
    QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:16.293Z"));
    QCOMPARE(m_requestID, QString("d88e4c72cf7f34ad"));
    QCOMPARE(m_type, QString("Зміна меню"));
    QCOMPARE(m_details, QString("Журнал подій"));
    QCOMPARE(m_username1, QString(""));
    QCOMPARE(m_authType, QString(""));
    QCOMPARE(m_externalIP, QString(""));
    QCOMPARE(m_internalIP, QString(""));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-23T13:40:16.293"));
}

void CSVParserTest::testOtherdata2()
{
    qDebug() << "Testcase: Other data #2";

    initTestCase(); // for checking empty fields

    QFile file(SRCDIR"data/testcase_otherdata2.csv");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString("galex"));
    QCOMPARE(m_timestampISO8601, QString("2023-05-31T10:19:09.84Z"));
    QCOMPARE(m_requestID, QString("9afa5af2e524f0ed"));
    QCOMPARE(m_type, QString("Financial Guarantee"));
    //the datails data is not interesting and very long
    QCOMPARE(m_username1, QString(""));
    QCOMPARE(m_authType, QString(""));
    QCOMPARE(m_externalIP, QString(""));
    QCOMPARE(m_internalIP, QString(""));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-31T13:19:09.840"));
}

void CSVParserTest::testOtherdata3_exception()
{
    qDebug() << "Testcase: Other data #3 - exception";

    initTestCase(); // for checking empty fields

    QFile file(SRCDIR"data/testcase_otherdata3_exception.csv");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString("galex"));
    QCOMPARE(m_timestampISO8601, QString("2023-05-31T09:56:50.55Z"));
    QCOMPARE(m_requestID, QString("a6145c4eb3322357"));
    QCOMPARE(m_type, QString("System error"));
    //the datails data is not interesting and very long
    QCOMPARE(m_username1, QString(""));
    QCOMPARE(m_authType, QString(""));
    QCOMPARE(m_externalIP, QString(""));
    QCOMPARE(m_internalIP, QString(""));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-31T12:56:50.550"));
}

void CSVParserTest::testDateTimeFormats()
{
    QFile file;
    qDebug() << "Testcase: Check parsing datetime (format24 yyyy-MM-ddTHH:mm:ss.zzzZ)";

    file.setFileName(SRCDIR"data/testcase_datetimeFormat24.csv");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:16.293Z"));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-23T13:40:16.293"));

    qDebug() << "Testcase: Check parsing datetime (format20 yyyy-MM-ddTHH:mm:ss.zzZ)";

    buf.clear();
    file.setFileName(SRCDIR"data/testcase_datetimeFormat20.csv");
    retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:13.21Z"));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-23T13:40:13.210"));

    qDebug() << "Testcase: Check parsing datetime (format22 yyyy-MM-ddTHH:mm:ss.zZ)";

    buf.clear();
    file.setFileName(SRCDIR"data/testcase_datetimeFormat22.csv");
    retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.parse(buf);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:39:33.2Z"));
    QCOMPARE(m_timestampTZ.toString("yyyy-MM-ddTHH:mm:ss.zzz"), QString("2023-05-23T13:39:33.200"));
}

QTEST_APPLESS_MAIN(CSVParserTest)

#include "tst_csvparser.moc"
