#include <QtTest>
#include <QFile>
#include <QDateTime>
#include <QByteArray>

#include "CAuditTrailParser.h"

class CAuditTrailParserTest : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

private:
    CAuditTrailParser m_parser;

    QDateTime   m_timestamp;

    QString     m_status;
    QString     m_method;
    QString     m_username;
    QString     m_role;
    QString     m_companyname;
    QString     m_attributes;
    QString     m_username1;

    QString     m_internalip;
    QString     m_externalip;

private slots:
    void initTestCase();
    void test_parsePerson();
    void test_userLoginSuccess();
    void test_dispatchBySystem();
    void test_doFileUpload();
    void test_listTransactions();
};

void
CAuditTrailParserTest::initTestCase()
{
    m_parser.init("10.");
}

void
CAuditTrailParserTest::test_parsePerson()
{
    QFile file(SRCDIR"data/testcase_parsePerson.csv");
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
    m_parser.getParsedData(m_status, m_timestamp, m_method, m_username, m_role, m_companyname, m_attributes, m_username1,
                        m_internalip, m_externalip);

    QCOMPARE(m_status, QString("Succeeded"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-03-22T18:07:27"));
    QCOMPARE(m_method, QString("listAuctions"));
    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_role, QString("Test Manager"));
    QCOMPARE(m_companyname, QString("TEST"));
    QCOMPARE(m_attributes, QString("com.smart.auction.uc.AuctionListFilter@10000001;Person [pk=1000001, alias=mr_data];1000001"));
    QCOMPARE(m_username1, QString("mr_data"));
    QCOMPARE(m_internalip, QString("10.10.10.10"));
    QCOMPARE(m_externalip, QString());
}

void
CAuditTrailParserTest::test_userLoginSuccess()
{
    QFile file(SRCDIR"data/testcase_userLoginSuccess.csv");
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
    m_parser.getParsedData(m_status, m_timestamp, m_method, m_username, m_role, m_companyname, m_attributes, m_username1,
                           m_internalip, m_externalip);

    QCOMPARE(m_status, QString("Succeeded"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-03-22T18:07:49"));
    QCOMPARE(m_method, QString("com.smart.emwebgui.usecases.AuthenticationUseCases.isAlternateLogin"));
    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_role, QString("Test Manager"));
    QCOMPARE(m_companyname, QString("TEST"));
    QCOMPARE(m_attributes, QString("(null)"));
    QCOMPARE(m_username1, QString());
    QCOMPARE(m_internalip, QString("10.10.10.10"));
    QCOMPARE(m_externalip, QString());
}

void
CAuditTrailParserTest::test_dispatchBySystem()
{
    QFile file(SRCDIR"data/testcase_dispatchBySystem.csv");
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
    m_parser.getParsedData(m_status, m_timestamp, m_method, m_username, m_role, m_companyname, m_attributes, m_username1,
                           m_internalip, m_externalip);

    QCOMPARE(m_status, QString("Succeeded"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-03-22T18:07:16"));
    QCOMPARE(m_method, QString("com.smart.eventmonitor.planned.ISEMPlannedEventMonitorUseCases.dispatchBySystem"));
    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_role, QString("Test Manager"));
    QCOMPARE(m_companyname, QString("TEST"));
    QCOMPARE(m_attributes, QString("UID: 1000001"));
    QCOMPARE(m_username1, QString());
    QCOMPARE(m_internalip, QString("10.10.10.10"));
    QCOMPARE(m_externalip, QString());
}

void
CAuditTrailParserTest::test_doFileUpload()
{
    QFile file(SRCDIR"data/testcase_doFileUpload.csv");
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
    m_parser.getParsedData(m_status, m_timestamp, m_method, m_username, m_role, m_companyname, m_attributes, m_username1,
                           m_internalip, m_externalip);

    QCOMPARE(m_status, QString("Succeeded"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-03-22T18:07:24"));
    QCOMPARE(m_method, QString("doFileUpload"));
    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_role, QString("Test Manager"));
    QCOMPARE(m_companyname, QString("TEST"));
    QCOMPARE(m_attributes, QString("very_very_long_filename.xml;java.io.ByteArrayInputStream@5007a705;uk;[ReturnMessageBean(0,web.ess.answer,A01,null,null)]"));
    QCOMPARE(m_username1, QString());
    QCOMPARE(m_internalip, QString("10.10.10.10"));
    QCOMPARE(m_externalip, QString());
}

void
CAuditTrailParserTest::test_listTransactions()
{
    QFile file(SRCDIR"data/testcase_listTransactions.csv");
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
    m_parser.getParsedData(m_status, m_timestamp, m_method, m_username, m_role, m_companyname, m_attributes, m_username1,
                           m_internalip, m_externalip);

    QCOMPARE(m_status, QString("Succeeded"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-03-22T18:07:47"));
    QCOMPARE(m_method, QString("listTransactions"));
    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_role, QString("Test Manager"));
    QCOMPARE(m_companyname, QString("TEST"));
    QCOMPARE(m_attributes, QString("BalanceGroup [pk=1000001, alias=TEST_UA-IPS];Sat Mar 23 10:04:00 EET 2024;null;null;null"));
    QCOMPARE(m_username1, QString());
    QCOMPARE(m_internalip, QString("10.10.10.10"));
    QCOMPARE(m_externalip, QString());
}

QTEST_APPLESS_MAIN(CAuditTrailParserTest)

#include "tst_caudittrailparsertest.moc"
