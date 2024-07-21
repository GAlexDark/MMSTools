#include <QtTest>
#include <QFile>
#include <QByteArray>

#include "CSqliteDatabase.h"
#include "DBStrings.h"

// add necessary includes here

const int recordCount = 51;

const QString selectCount(QLatin1String("select count() from eventlog;"));
const QString selectAll(QLatin1String("select * from eventlog;"));

class CBasicDatabaseTest : public QObject
{
    Q_OBJECT

public:
    explicit CBasicDatabaseTest(QObject *parent = nullptr);
    ~CBasicDatabaseTest();

private:
    CSqliteDatabase m_fakeDB;

private slots:
    void initTestCase();
    void test_connectionName();
    void test_getDBinstance();
    void test_openDB();
    void test_createTable();
    void test_importData();
    void test_findInDBvar1();
    void test_findInDBvar2();
    void test_findInDBvar3();
    void test_getValues();
    void test_truncateTable();
    void test_InsertBindedValues();
};

void CBasicDatabaseTest::initTestCase()
{
    bool retVal = true;
    QFileInfo file(SRCDIR"data/fakedb.db");
    if (file.exists()) {
        retVal = QFile::remove(file.absoluteFilePath());
        qDebug() << retVal;
    }
    QVERIFY(retVal);
    retVal = m_fakeDB.init(SRCDIR"data/fakedb.db");
    QVERIFY(retVal);
}


CBasicDatabaseTest::CBasicDatabaseTest(QObject *parent)
    : QObject{parent}
{}

CBasicDatabaseTest::~CBasicDatabaseTest()
{
    m_fakeDB.close();
}

/*****************************************************
 *
 * Test cases
 *
 ****************************************************/

void CBasicDatabaseTest::test_connectionName()
{
    QVERIFY(!m_fakeDB.getConnectionName().isEmpty());
    QVERIFY(!m_fakeDB.getConnectionName().isNull());
}

void CBasicDatabaseTest::test_getDBinstance()
{
    QVERIFY(m_fakeDB.getDBinstance());
}

void CBasicDatabaseTest::test_openDB()
{
    bool retVal = m_fakeDB.open();
    QVERIFY(retVal);
}

void CBasicDatabaseTest::test_createTable()
{
    bool retVal = m_fakeDB.exec(eventlog::createTable);
    QVERIFY(retVal);
    retVal = m_fakeDB.exec(audittrail::createTable);
    QVERIFY(retVal);
}

void CBasicDatabaseTest::test_importData()
{
    QFile file(SRCDIR"data/fakedb.sql");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool retVal = file.open(QIODeviceBase::ReadOnly);
#else
    bool retVal = file.open(QIODevice::ReadOnly);
#endif
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    file.close();
    QVERIFY(!buf.isEmpty());
    retVal = m_fakeDB.exec(buf);
    QVERIFY(retVal);
    qDebug()  << "Ignore dublicates checking";
    retVal = m_fakeDB.exec(buf);
    QVERIFY(retVal);
}

void CBasicDatabaseTest::test_findInDBvar1()
{
    dataList_t res = m_fakeDB.findInDB(selectCount, false);
    int columnCount = res.at(0).at(0).toInt();
    qDebug() << columnCount;
    QCOMPARE(columnCount, recordCount);

    dataList_t res1 = m_fakeDB.findInDB(selectCount, true);
    columnCount = res1.at(1).at(0).toInt();
    qDebug() << columnCount;
    QCOMPARE(columnCount, recordCount);
}

void CBasicDatabaseTest::test_findInDBvar2()
{
    dataList_t retVal = m_fakeDB.findInDB(selectAll, false);
    QCOMPARE(retVal.count(), recordCount);

    dataList_t retVal1 = m_fakeDB.findInDB(selectAll, true);
    QCOMPARE(retVal1.count(), recordCount + 1);
}

void CBasicDatabaseTest::test_findInDBvar3()
{
    int count = 0;
    bool retVal = m_fakeDB.exec(selectAll);
    QVERIFY(retVal);
    while (m_fakeDB.isNext()) {
        count++;
    }
    QCOMPARE(count, recordCount);
}

void CBasicDatabaseTest::test_getValues()
{
    int count = 0;
    bool retVal = m_fakeDB.exec("select * from eventlog where rowid=1;");
    QVERIFY(retVal);

    while (m_fakeDB.isNext()) {
        QCOMPARE(m_fakeDB.geValue(0).toString(), QString("mr_data"));
        QCOMPARE(m_fakeDB.geValue(1).toString(), QString("2023-05-09T11:19:57.36Z"));
        QCOMPARE(m_fakeDB.geValue(2).toString(), QString("edbfa4ea24038861"));

        QCOMPARE(m_fakeDB.geValue(4).toString(), QString("username: mr_data,\r\n  type: PASSWORD,\r\n  ip address: 192.0.2.211, 10.10.10.10"));
        QCOMPARE(m_fakeDB.geValue(5).toString(), QString("mr_data"));
        QCOMPARE(m_fakeDB.geValue(6).toString(), QString("PASSWORD"));
        QCOMPARE(m_fakeDB.geValue(7).toString(), QString("192.0.2.211"));
        QCOMPARE(m_fakeDB.geValue(8).toString(), QString("10.10.10.10"));
        QCOMPARE(m_fakeDB.geValue(9).toString(), QString("2023-05-09T14:19:57.360"));
        count++;
    }
    QCOMPARE(count, 1);
}

void CBasicDatabaseTest::test_truncateTable()
{
    const QString insertString = QStringLiteral("INSERT INTO eventlog (username, timestampISO8601, requestid, type, details, username1, authtype, externalip, internalip, timestamp) VALUES ('mr_data', '2023-05-09T11:19:57.36Z', 'edbfa4ea24038861', 'Вхід користувача - успішно', 'username: mr_data,\r\n  type: PASSWORD,\r\n  ip address: 192.0.2.211, 10.10.10.10', 'mr_data', 'PASSWORD', '192.0.2.211', '10.10.10.10', '2023-05-09T14:19:57.360');");
    bool retVal = m_fakeDB.exec(insertString);
    QVERIFY(retVal);

    retVal = m_fakeDB.truncateTable("eventlog");
    QVERIFY(retVal);
    dataList_t res = m_fakeDB.findInDB(selectCount, false);
    int columnCount = res.at(0).at(0).toInt();
    QCOMPARE(columnCount, 0);
}

void CBasicDatabaseTest::test_InsertBindedValues()
{
    test_createTable();
    bool retVal = m_fakeDB.prepareRequest(eventlog::insertData);
    QVERIFY(retVal);

    mms::dataItem_t data;
    data[phUsername] = "mr_data";
    data[phTimestampISO8601] = "2023-05-09T11:19:57.36Z";
    data[phTimestamp] = "2023-05-09T14:19:57.360";
    data[phRequestID] = "edbfa4ea24038861";
    data[phType] = "Вхід користувача - успішно";
    data[phDetails] = "username: mr_data,\r\n  type: PASSWORD,\r\n  ip address: 192.0.2.211, 10.10.10.10";
    data[phUsername1] = "mr_data";
    data[phAuthType] = "PASSWORD";
    data[phExternalip] = "192.0.2.211";
    data[phInternalip] = "10.10.10.10";
    retVal = m_fakeDB.execRequest(&data);
    QVERIFY(retVal);

    int count = 0;
    retVal = m_fakeDB.exec(selectAll);
    QVERIFY(retVal);
    while (m_fakeDB.isNext()) {
        count++;
    }
    QCOMPARE(count, 1);

    data[phUsername] = "Ім'я користувача";
    data[phTimestampISO8601] = "Відмітка часу";
    data[phTimestamp] = QDateTime();
    data[phRequestID] = "ID запиту";
    data[phType] = "Тип";
    data[phDetails] = "Деталі";
    data[phUsername1] = QString();
    data[phAuthType] = QString();
    data[phExternalip] = QString();
    data[phInternalip] = QString();
    retVal = m_fakeDB.execRequest(&data);
    QVERIFY(retVal);

    count = 0;
    retVal = m_fakeDB.exec(selectAll);
    QVERIFY(retVal);
    while (m_fakeDB.isNext()) {
        count++;
    }
    QCOMPARE(count, 1);
}

QTEST_APPLESS_MAIN(CBasicDatabaseTest)

#include "tst_cbasicdatabase.moc"
