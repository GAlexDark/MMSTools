#include <QCoreApplication>
#include <QtTest>

#include "CSingleApplication.h"

// add necessary includes here

class CSingleApplicationTest : public QObject
{
    Q_OBJECT

public:
    explicit CSingleApplicationTest(QObject *parent = nullptr);

private slots:
    void test_checkSecondStart() const;
    void test_applicationPid() const;
};

CSingleApplicationTest::CSingleApplicationTest(QObject *parent)
    : QObject{parent}
{}

void CSingleApplicationTest::test_checkSecondStart() const
{
    qDebug() << "Start first app";
    CSingleApplication sa1("elc_tst_instance");
    bool retVal = sa1.isRunning();
    QVERIFY(!retVal);

    qDebug() << "Start secont app";
    CSingleApplication sa2("elc_tst_instance");
    retVal = sa2.isRunning();
    QVERIFY(retVal);
}

void CSingleApplicationTest::test_applicationPid() const
{
    QVERIFY(CSingleApplication::applicationPid() > 0);
}

QTEST_MAIN(CSingleApplicationTest)

#include "tst_csingleapplicationtest.moc"
