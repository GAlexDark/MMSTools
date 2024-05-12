#include <QDateTime>
#include <QFile>
#include <QtTest>

#include "CEventLogParser.h"
#include "qtestcase.h"

// add necessary includes here

class CEventLogParserTest : public QObject {
  Q_OBJECT

public:
  CEventLogParserTest(QObject *parent = nullptr);
  ~CEventLogParserTest();

private:
  CEventLogParser m_parser;

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

private slots:
  void initTestCase();
  void initTestCaseEng();
  void test_successAuth_wALL_ipsUkrLang();
  void test_successAuth_wALL_ipsEngLang();
  void test_failedAuthUkrLang();
  void test_failedAuthEngLang();
  void test_successAuth_wINTERNAL_ips();
  void test_successAuth_wEXTERNAL_ips();
  void test_otherData1();
  void test_otherData2();
  void test_otherData3_exception();
  void test_DateTimeFormats();
};

CEventLogParserTest::CEventLogParserTest(QObject *parent) : QObject{parent} {}

CEventLogParserTest::~CEventLogParserTest() {}

void CEventLogParserTest::initTestCase() {
  // All fields are not empty
  QFile file(SRCDIR "data/testcase_success_wAll_ips.csv");
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
  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);
}

void CEventLogParserTest::initTestCaseEng() {
  // All fields are not empty
  QFile file(SRCDIR "data/testcase_success_wAll_ips_engLang.csv");
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
  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);
}

void CEventLogParserTest::test_successAuth_wALL_ipsUkrLang() {
  qDebug() << "Testcase: Success auth with all IPs (internal + external)";

  initTestCase();

  QCOMPARE(m_username, QString("mr_data"));
  QCOMPARE(m_timestampISO8601, QString("2023-05-09T11:19:57.36Z"));
  QCOMPARE(m_requestID, QString("edbfa4ea24038861"));
  QCOMPARE(m_type, QString("Вхід користувача - успішно"));
  QCOMPARE(m_details, QString("username: mr_data,\n  type: PASSWORD,\n  ip "
                              "address: 192.0.2.211, 10.10.10.10"));
  QCOMPARE(m_username1, QString("mr_data"));
  QCOMPARE(m_authType, QString("PASSWORD"));
  QCOMPARE(m_externalIP, QString("192.0.2.211"));
  QCOMPARE(m_internalIP, QString("10.10.10.10"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-09T14:19:57.360"));
}

void CEventLogParserTest::test_successAuth_wALL_ipsEngLang() {
  qDebug() << "Testcase: Success auth with all IPs (internal + external)";

  initTestCaseEng();

  QCOMPARE(m_username, QString("mr_data"));
  QCOMPARE(m_timestampISO8601, QString("2023-05-09T11:19:57.36Z"));
  QCOMPARE(m_requestID, QString("edbfa4ea24038861"));
  QCOMPARE(m_type, QString("User login - successful"));
  QCOMPARE(m_details, QString("username: mr_data,\n  type: PASSWORD,\n  ip "
                              "address: 192.0.2.211, 10.10.10.10"));
  QCOMPARE(m_username1, QString("mr_data"));
  QCOMPARE(m_authType, QString("PASSWORD"));
  QCOMPARE(m_externalIP, QString("192.0.2.211"));
  QCOMPARE(m_internalIP, QString("10.10.10.10"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-09T14:19:57.360"));
}

void CEventLogParserTest::test_failedAuthUkrLang() {
  qDebug() << "Testcase: Failed auth";

  initTestCase(); // for checking empty fields

  QFile file(SRCDIR "data/testcase_failed.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_username, QString());
  QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:25:46.717Z"));
  QCOMPARE(m_requestID, QString("693d40a5f9e0e2ec"));
  QCOMPARE(m_type, QString("Вхід користувача - невдало"));
  QCOMPARE(m_details, QString("type: PASSWORD\n  ip address: 10.10.10.10"));
  QCOMPARE(m_username1, QString());
  QCOMPARE(m_authType, QString("PASSWORD"));
  QCOMPARE(m_externalIP, QString());
  QCOMPARE(m_internalIP, QString("10.10.10.10"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-23T13:25:46.717"));
}

void CEventLogParserTest::test_failedAuthEngLang() {
  qDebug() << "Testcase: Failed auth";

  initTestCase(); // for checking empty fields

  QFile file(SRCDIR "data/testcase_failed_engLang.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_username, QString());
  QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:25:46.717Z"));
  QCOMPARE(m_requestID, QString("693d40a5f9e0e2ec"));
  QCOMPARE(m_type, QString("User login - unsuccessful"));
  QCOMPARE(m_details, QString("type: PASSWORD\n  ip address: 10.10.10.10"));
  QCOMPARE(m_username1, QString());
  QCOMPARE(m_authType, QString("PASSWORD"));
  QCOMPARE(m_externalIP, QString());
  QCOMPARE(m_internalIP, QString("10.10.10.10"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-23T13:25:46.717"));
}

void CEventLogParserTest::test_successAuth_wINTERNAL_ips() {
  qDebug() << "Testcase: Success auth with internal IPs only";

  initTestCase(); // for checking empty fields

  QFile file(SRCDIR "data/testcase_success_wInt_ips.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_username, QString("mr_data"));
  QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:06.777Z"));
  QCOMPARE(m_requestID, QString("10ff82c8e706c291"));
  QCOMPARE(m_type, QString("Вхід користувача - успішно"));
  QCOMPARE(m_details, QString("username: mr_data,\n  type: PASSWORD,\n  ip "
                              "address: 10.10.1.13, 10.10.10.10"));
  QCOMPARE(m_username1, QString("mr_data"));
  QCOMPARE(m_authType, QString("PASSWORD"));
  QCOMPARE(m_externalIP, QString());
  QCOMPARE(m_internalIP, QString("10.10.1.13, 10.10.10.10"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-23T13:40:06.777"));
}

void CEventLogParserTest::test_successAuth_wEXTERNAL_ips() {
  qDebug() << "Testcase: Success auth with external IPs only";

  initTestCase(); // for checking empty fields

  QFile file(SRCDIR "data/testcase_success_wExt_ips.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_username, QString("mr_data"));
  QCOMPARE(m_timestampISO8601, QString("2022-10-17T06:46:45.443Z"));
  QCOMPARE(m_requestID, QString("7eab70f76c744995"));
  QCOMPARE(m_type, QString("Вхід користувача - успішно"));
  QCOMPARE(
      m_details,
      QString(
          "username: mr_data,\n  type: PASSWORD,\n  ip address: 192.0.2.118"));
  QCOMPARE(m_username1, QString("mr_data"));
  QCOMPARE(m_authType, QString("PASSWORD"));
  QCOMPARE(m_externalIP, QString("192.0.2.118"));
  QCOMPARE(m_internalIP, QString());
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2022-10-17T09:46:45.443"));
}

void CEventLogParserTest::test_otherData1() {
  qDebug() << "Testcase: Other data #1";

  initTestCase(); // for checking empty fields

  QFile file(SRCDIR "data/testcase_otherdata1.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_username, QString("mr_data"));
  QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:16.293Z"));
  QCOMPARE(m_requestID, QString("d88e4c72cf7f34ad"));
  QCOMPARE(m_type, QString("Зміна меню"));
  QCOMPARE(m_details, QString("Журнал подій"));
  QCOMPARE(m_username1, QString());
  QCOMPARE(m_authType, QString());
  QCOMPARE(m_externalIP, QString());
  QCOMPARE(m_internalIP, QString());
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-23T13:40:16.293"));
}

void CEventLogParserTest::test_otherData2() {
  qDebug() << "Testcase: Other data #2";

  initTestCase(); // for checking empty fields

  QFile file(SRCDIR "data/testcase_otherdata2.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_username, QString("mr_data"));
  QCOMPARE(m_timestampISO8601, QString("2023-05-31T10:19:09.84Z"));
  QCOMPARE(m_requestID, QString("9afa5af2e524f0ed"));
  QCOMPARE(m_type, QString("Financial Guarantee"));
  // the datails data is not interesting and very long
  QCOMPARE(m_username1, QString());
  QCOMPARE(m_authType, QString());
  QCOMPARE(m_externalIP, QString());
  QCOMPARE(m_internalIP, QString());
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           "2023-05-31T13:19:09.840");
}

void CEventLogParserTest::test_otherData3_exception() {
  qDebug() << "Testcase: Other data #3 - exception";

  initTestCase(); // for checking empty fields

  QFile file(SRCDIR "data/testcase_otherdata3_exception.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_username, QString("mr_data"));
  QCOMPARE(m_timestampISO8601, QString("2023-05-31T09:56:50.55Z"));
  QCOMPARE(m_requestID, QString("a6145c4eb3322357"));
  QCOMPARE(m_type, QString("System error"));
  // the datails data is not interesting and very long
  QCOMPARE(m_username1, QString());
  QCOMPARE(m_authType, QString());
  QCOMPARE(m_externalIP, QString());
  QCOMPARE(m_internalIP, QString());
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-31T12:56:50.550"));
}

void CEventLogParserTest::test_DateTimeFormats() {
  QFile file;
  qDebug()
      << "Testcase: Check parsing datetime (format24 yyyy-MM-ddTHH:mm:ss.zzzZ)";

  file.setFileName(SRCDIR "data/testcase_datetimeFormat24.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
  bool retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  QByteArray buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:16.293Z"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-23T13:40:16.293"));

  qDebug()
      << "Testcase: Check parsing datetime (format23 yyyy-MM-ddTHH:mm:ss.zzZ)";

  buf.clear();
  file.setFileName(SRCDIR "data/testcase_datetimeFormat23.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  retVal = file.open(QIODeviceBase::ReadOnly);
#else
  retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:40:13.21Z"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-23T13:40:13.210"));

  qDebug()
      << "Testcase: Check parsing datetime (format22 yyyy-MM-ddTHH:mm:ss.zZ)";

  buf.clear();
  file.setFileName(SRCDIR "data/testcase_datetimeFormat22.csv");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  retVal = file.open(QIODeviceBase::ReadOnly);
#else
  retVal = file.open(QIODevice::ReadOnly);
#endif
  QVERIFY(retVal);
  buf = file.readAll();
  QVERIFY(!buf.isEmpty());
  file.close();

  m_parser.parse(buf);
  m_parser.getParsedData(m_username, m_timestampISO8601, m_requestID, m_type,
                         m_details, m_username1, m_authType, m_externalIP,
                         m_internalIP, m_timestampTZ);

  QCOMPARE(m_timestampISO8601, QString("2023-05-23T10:39:33.2Z"));
  QCOMPARE(m_timestampTZ.toString(Qt::ISODateWithMs),
           QString("2023-05-23T13:39:33.200"));
}

QTEST_APPLESS_MAIN(CEventLogParserTest)

#include "tst_ceventlogparsertest.moc"
