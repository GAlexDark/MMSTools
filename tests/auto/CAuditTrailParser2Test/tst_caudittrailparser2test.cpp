#include <QtTest>
#include <QFile>
#include <QDateTime>
#include <QByteArray>

#include "CAuditTrailParser2.h"

class CAuditTrailParser2Test : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

private:
    CAuditTrailParser2 m_parser;

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
    QString     m_sessionId;

    void test_testdata1(const QString &fileName);

private slots:
    void initTestCase();
    void test_userLoginSuccess();
};

void
CAuditTrailParser2Test::initTestCase()
{
    m_parser.init("10.");
}

void
CAuditTrailParser2Test::test_userLoginSuccess()
{
    QFile file(SRCDIR"data/testcase_userLoginSuccess_CR003.csv");
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
                           m_internalip, m_externalip, m_sessionId);
    QCOMPARE(m_status, QString("Succeeded"));
    QCOMPARE(m_timestamp.toString(Qt::ISODateWithMs), QString("2025-11-09T13:20:29.228"));
    QCOMPARE(m_method, QString("com.smart.emwebgui.usecases.AuthenticationUseCases.isAlternateLogin"));
    QCOMPARE(m_username, QString("mr_data"));
    QCOMPARE(m_role, QString("Test Manager"));
    QCOMPARE(m_companyname, QString("TEST"));
    QCOMPARE(m_attributes, QString("(null)"));
    QCOMPARE(m_internalip, QString("10.10.10.10"));
    QCOMPARE(m_sessionId, QString("3b80177a-e867-4fa1-8221-856f6c739316"));
    QCOMPARE(m_username1, QString());
}

QTEST_APPLESS_MAIN(CAuditTrailParser2Test)

#include "tst_caudittrailparser2test.moc"
