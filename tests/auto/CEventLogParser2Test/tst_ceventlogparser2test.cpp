#include <QtTest>
#include <QFile>
#include <QDateTime>
#include <QByteArray>

#include "CEventLogParser2.h"

class CEventLogParser2Test : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

private:
    CEventLogParser2 m_parser;

    QString m_username;
    QString m_timestampISO8601;
    QString m_requestID;
    QString m_type;
    QString m_details;
    QString m_username1;
    QString m_authType;
    QString m_externalIP;
    QString m_internalIP;
    QDateTime m_timestampTZ;
    QString m_sessionID;

    void test_failedAuth(const QString &fileName);

    void test_successAuth_wALL_ips();

private slots:
    void initTestCase();
    void initTestCaseEng();
    void test_successAuth_wALL_ipsUkrLang();
    void test_successAuth_wALL_ipsEngLang();
    void test_failedAuthUkrLang();
    void test_failedAuthEngLang();
    void test_successAuth_wINTERNAL_ips();
    void test_successAuth_wEXTERNAL_ips();
};

void CEventLogParser2Test::initTestCase()
{
    // All fields are not empty
    QFile file(SRCDIR"data/testcase_success_wAll_ips.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
    bool retVal = file.open(QIODevice::ReadOnly);
#endif
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.init("10.");
    retVal = m_parser.parse(buf);
    QVERIFY(retVal);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_sessionID, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);
}

void CEventLogParser2Test::initTestCaseEng()
{
    // All fields are not empty
    QFile file(SRCDIR"data/testcase_success_wAll_ips_engLang.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
    bool retVal = file.open(QIODevice::ReadOnly);
#endif
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    m_parser.init("10.");
    retVal = m_parser.parse(buf);
    QVERIFY(retVal);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_sessionID, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);
}

void CEventLogParser2Test::test_successAuth_wALL_ips()
{
    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_timestampISO8601, QString("2023-05-09T11:19:57.36Z"));
    QCOMPARE(m_sessionID, QString("56153989-182b-4a36-9604-3f1ab4482c90"));
    QCOMPARE(m_requestID, QString("edbfa4ea24038861"));
    QCOMPARE(m_details, QString("username: mr_data,\n  type: PASSWORD,\n  ip address: 192.0.2.211, 10.10.10.10"));
    QCOMPARE(m_username1, QString("mr_data"));
    QCOMPARE(m_authType, QString("PASSWORD"));
    QCOMPARE(m_externalIP, QString("192.0.2.211"));
    QCOMPARE(m_internalIP, QString("10.10.10.10"));
    QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs), QString("2023-05-09T14:19:57.360"));
}

void CEventLogParser2Test::test_successAuth_wALL_ipsUkrLang() {
    qDebug() << "Testcase: Success auth with all IPs (internal + external)";

    initTestCase();
    test_successAuth_wALL_ips();

    QCOMPARE(m_type, QString("Вхід користувача - успішно"));
}

void CEventLogParser2Test::test_successAuth_wALL_ipsEngLang() {
    qDebug() << "Testcase: Success auth with all IPs (internal + external)";

    initTestCaseEng();
    test_successAuth_wALL_ips();

    QCOMPARE(m_type, QString("User login - successful"));
}

void CEventLogParser2Test::test_failedAuth(const QString &fileName)
{
    QFile file(fileName);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
    bool retVal = file.open(QIODevice::ReadOnly);
#endif
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    retVal = m_parser.parse(buf);
    QVERIFY(retVal);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_sessionID, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_timestampISO8601, QString("2025-11-08T13:18:04.857Z"));
    QCOMPARE(m_sessionID, QString());
    QCOMPARE(m_requestID, QString("690f430c9de75736a367002226fe6539"));
    QCOMPARE(m_details, QString("username: mr_data,\n type: PASSWORD,\n ip address: 10.10.10.10"));
    QCOMPARE(m_username1, QString("mr_data"));
    QCOMPARE(m_authType, QString("PASSWORD"));
    QCOMPARE(m_externalIP, QString());
    QCOMPARE(m_internalIP, QString("10.10.10.10"));
    QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs), QString("2025-11-08T15:18:04.857"));
}

void CEventLogParser2Test::test_failedAuthUkrLang() {
    qDebug() << "Testcase: Failed auth";

    initTestCase(); // for checking empty fields
    test_failedAuth(SRCDIR"data/testcase_failed.csv");

    QCOMPARE(m_type, QString("Вхід користувача - невдало"));
}

void CEventLogParser2Test::test_failedAuthEngLang() {
    qDebug() << "Testcase: Failed auth";

    initTestCase(); // for checking empty fields
    test_failedAuth(SRCDIR"data/testcase_failed_engLang.csv");

    QCOMPARE(m_type, QString("User login - unsuccessful"));
}

void CEventLogParser2Test::test_successAuth_wINTERNAL_ips() {
    qDebug() << "Testcase: Success auth with internal IPs only";

    initTestCase(); // for checking empty fields

    QFile file(SRCDIR"data/testcase_success_wInt_ips.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
    bool retVal = file.open(QIODevice::ReadOnly);
#endif
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    retVal = m_parser.parse(buf);
    QVERIFY(retVal);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_sessionID, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:06.777Z"));
    QCOMPARE(m_sessionID, QString("84035ed2-aa92-4fc2-8120-29046603be49"));
    QCOMPARE(m_requestID, QString("10ff82c8e706c291"));
    QCOMPARE(m_type, QString("Вхід користувача - успішно"));
    QCOMPARE(m_details, QString("username: mr_data,\n  type: PASSWORD,\n  ip address: 10.10.1.13, 10.10.10.10"));
    QCOMPARE(m_username1, QString("mr_data"));
    QCOMPARE(m_authType, QString("PASSWORD"));
    QCOMPARE(m_externalIP, QString());
    QCOMPARE(m_internalIP, QString("10.10.1.13, 10.10.10.10"));
    QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs), QString("2023-05-23T13:40:06.777"));
}

void CEventLogParser2Test::test_successAuth_wEXTERNAL_ips() {
    qDebug() << "Testcase: Success auth with external IPs only";

    initTestCase(); // for checking empty fields

    QFile file(SRCDIR"data/testcase_success_wExt_ips.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
    bool retVal = file.open(QIODevice::ReadOnly);
#endif
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    QVERIFY(!buf.isEmpty());
    file.close();

    retVal = m_parser.parse(buf);
    QVERIFY(retVal);
    m_parser.getParsedData(m_username, m_timestampISO8601, m_sessionID, m_requestID, m_type, m_details,
                           m_username1, m_authType, m_externalIP, m_internalIP, m_timestampTZ);

    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_timestampISO8601, QString("2022-10-17T06:46:45.443Z"));
    QCOMPARE(m_sessionID, QString("84035ed2-aa92-4fc2-8120-29046603be49"));
    QCOMPARE(m_requestID, QString("7eab70f76c744995"));
    QCOMPARE(m_type, QString("Вхід користувача - успішно"));
    QCOMPARE(m_details, QString("username: mr_data,\n  type: PASSWORD,\n  ip address: 192.0.2.118"));
    QCOMPARE(m_username1, QString("mr_data"));
    QCOMPARE(m_authType, QString("PASSWORD"));
    QCOMPARE(m_externalIP, QString("192.0.2.118"));
    QCOMPARE(m_internalIP, QString());
    QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs), QString("2022-10-17T09:46:45.443"));
}

QTEST_APPLESS_MAIN(CEventLogParser2Test)

#include "tst_ceventlogparser2test.moc"
