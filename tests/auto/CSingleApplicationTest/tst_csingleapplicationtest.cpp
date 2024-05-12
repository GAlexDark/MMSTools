#include <QCoreApplication>
#include <QtTest>

#include "CSingleApplication.h"

// add necessary includes here

class CSingleApplicationTest : public QObject {
  Q_OBJECT

public:
  CSingleApplicationTest(QObject *parent = nullptr);
  ~CSingleApplicationTest();

private slots:
  void test_checkSecondStart();
  void test_applicationPid();
};

CSingleApplicationTest::CSingleApplicationTest(QObject *parent)
    : QObject{parent} {}

CSingleApplicationTest::~CSingleApplicationTest() {}

void CSingleApplicationTest::test_checkSecondStart() {
  qDebug() << "Start first app";
  CSingleApplication sa1("elc_tst_instance");
  bool retVal = sa1.isRunning();
  QVERIFY(!retVal);

  qDebug() << "Start secont app";
  CSingleApplication sa2("elc_tst_instance");
  retVal = sa2.isRunning();
  QVERIFY(retVal);
}

void CSingleApplicationTest::test_applicationPid() {
  QVERIFY(CSingleApplication::applicationPid() > 0);
}

QTEST_MAIN(CSingleApplicationTest)

#include "tst_csingleapplicationtest.moc"
