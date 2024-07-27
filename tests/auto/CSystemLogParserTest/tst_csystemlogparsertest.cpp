#include <QtTest>
#include <QFile>
#include <QDateTime>
#include <QByteArray>

#include "CSystemLogParser.h"

class CSystemLogParserTest : public QObject
{
    Q_OBJECT

public:
    explicit CSystemLogParserTest(QObject *parent = nullptr);

private:
    CSystemLogParser m_parser;

    QString m_severity;
    QDateTime m_timestamp;
    QString m_message;
    QString m_username;
    QString m_username1;
    QString m_role;
    QString m_companyname;


private slots:
    void initTestCase();
    void test_userLoginSuccess();
    void test_userRoleCompany();
    void test_other1();
    void test_other2();
};

CSystemLogParserTest::CSystemLogParserTest(QObject *parent)
    : QObject{parent}
{}

void
CSystemLogParserTest::initTestCase()
{
    m_parser.init("10.");
}

void
CSystemLogParserTest::test_userLoginSuccess()
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
    m_parser.getParsedData(m_severity, m_timestamp, m_message, m_username, m_username1,
                           m_role, m_companyname);

    QCOMPARE(m_severity, QString("INFO"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-07-14T16:43:09"));
    QCOMPARE(m_message, QString("User \"mr_data\" has logged in successfully"));
    QCOMPARE(m_username, QString());
    QCOMPARE(m_username1, QString("mr_data"));
    QCOMPARE(m_role, QString());
    QCOMPARE(m_companyname, QString());
}

void
CSystemLogParserTest::test_userRoleCompany()
{
    QFile file(SRCDIR"data/testcase_userRoleCompany.csv");
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
    m_parser.getParsedData(m_severity, m_timestamp, m_message, m_username, m_username1,
                           m_role, m_companyname);

    QCOMPARE(m_severity, QString("INFO"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-07-14T16:43:09"));
    QCOMPARE(m_message, QString("User \"mr_data\" occupied the role \"Data holder\" in the company \"TEST\"."));
    QCOMPARE(m_username, QString());
    QCOMPARE(m_username1, QString("mr_data"));
    QCOMPARE(m_role, QString("Data holder"));
    QCOMPARE(m_companyname, QString("TEST"));
}

void
CSystemLogParserTest::test_other1()
{
    QFile file(SRCDIR"data/testcase_other1.csv");
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
    m_parser.getParsedData(m_severity, m_timestamp, m_message, m_username, m_username1,
                           m_role, m_companyname);

    QCOMPARE(m_severity, QString("INFO"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-07-14T16:43:20"));
    QCOMPARE(m_message, QString("file processing finished. file: long_long_filename.xml)"));
    QCOMPARE(m_username, QString());
    QCOMPARE(m_username1, QString());
    QCOMPARE(m_role, QString());
    QCOMPARE(m_companyname, QString());
}

void
CSystemLogParserTest::test_other2()
{
    QFile file(SRCDIR"data/testcase_other2.csv");
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
    m_parser.getParsedData(m_severity, m_timestamp, m_message, m_username, m_username1,
                           m_role, m_companyname);

    QCOMPARE(m_severity, QString("INFO"));
    QCOMPARE(m_timestamp.toString(Qt::ISODate), QString("2024-07-14T16:43:17"));
    QCOMPARE(m_message, QString("file processing started ... (file: long_long_filename.xml)"));
    QCOMPARE(m_username, QString());
    QCOMPARE(m_username1, QString());
    QCOMPARE(m_role, QString());
    QCOMPARE(m_companyname, QString());
}

QTEST_APPLESS_MAIN(CSystemLogParserTest)

#include "tst_csystemlogparsertest.moc"
