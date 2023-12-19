#include <QtTest>
#include <QFile>
#include <QByteArray>

#include "CBasicDBClass.h"

// add necessary includes here

const QString createEventLogTable = QStringLiteral("CREATE TABLE IF NOT EXISTS [eventlog] (username TEXT, \
                                    timestampISO8601 TEXT NOT NULL, requestid TEXT NOT NULL, \
                                    type TEXT, details TEXT, username1 TEXT, authtype TEXT, externalip TEXT, internalip TEXT, timestamp DATETIME, \
                                    PRIMARY KEY (timestampISO8601, requestid));");

const QString insertOriginalData = QStringLiteral("INSERT OR IGNORE INTO [eventlog] (username, timestampISO8601, \
                                   requestid, type, details, timestamp, username1, authtype, externalip, \
                                   internalip) VALUES (:username, :timestampISO8601, :requestid, :type, \
                                   :details, :timestamp, :username1, :authtype, :externalip, :internalip)");

const int recordCount = 51;

class CBasicDBClassTest : public QObject
{
    Q_OBJECT

public:
    CBasicDBClassTest();
    ~CBasicDBClassTest();

private:
    CBasicDBClass m_fakeDB;

    void initTestCase();

private slots:
    void test_connectionName();
    void test_getDBinstance();

    void test_initDB();
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

void CBasicDBClassTest::initTestCase()
{
    bool retVal = true;
    QFileInfo file(SRCDIR"data/fakedb.db");
    if (file.exists()) {
        retVal = QFile::remove(file.absoluteFilePath());
        qDebug() << retVal;
    }
    QVERIFY(retVal);
}


CBasicDBClassTest::CBasicDBClassTest()
{
    initTestCase();
}

CBasicDBClassTest::~CBasicDBClassTest()
{
    m_fakeDB.close();
}

/*****************************************************
 *
 * Test cases
 *
 ****************************************************/

void CBasicDBClassTest::test_connectionName()
{
    QVERIFY(!m_fakeDB.getConnectionName().isEmpty());
    QVERIFY(!m_fakeDB.getConnectionName().isNull());
}

void CBasicDBClassTest::test_getDBinstance()
{
    QVERIFY(m_fakeDB.getDBinstance() != nullptr);
}

void CBasicDBClassTest::test_initDB()
{
    bool retVal = m_fakeDB.init("QSQLITE", SRCDIR"data/fakedb.db");
    QVERIFY(retVal);
}

void CBasicDBClassTest::test_openDB()
{
    bool retVal = m_fakeDB.open();
    QVERIFY(retVal);
}

void CBasicDBClassTest::test_createTable()
{
    bool retVal = m_fakeDB.exec(createEventLogTable);
    QVERIFY(retVal);
}

void CBasicDBClassTest::test_importData()
{
    QFile file(SRCDIR"data/fakedb.sql");
    bool retVal = file.open(QIODeviceBase::ReadOnly);
    QVERIFY(retVal);
    QByteArray buf = file.readAll();
    file.close();
    QVERIFY(!buf.isEmpty());
    retVal = m_fakeDB.exec(buf);
    QVERIFY(retVal);
}

void CBasicDBClassTest::test_findInDBvar1()
{
    TDataList res = m_fakeDB.findInDB("select count() from eventlog;", false);
    int columnCount = res.at(0).at(0).toInt();
    qDebug() << columnCount;
    QCOMPARE(columnCount, recordCount);

    TDataList res1 = m_fakeDB.findInDB("select count() from eventlog;", true);
    columnCount = res1.at(1).at(0).toInt();
    qDebug() << columnCount;
    QCOMPARE(columnCount, recordCount);
}

void CBasicDBClassTest::test_findInDBvar2()
{
    TDataList retVal = m_fakeDB.findInDB("select * from eventlog;", false);
    QCOMPARE(retVal.count(), recordCount);

    TDataList retVal1 = m_fakeDB.findInDB("select * from eventlog;", true);
    QCOMPARE(retVal1.count(), recordCount + 1);
}

void CBasicDBClassTest::test_findInDBvar3()
{
    int count = 0;
    bool retVal = m_fakeDB.exec("select * from eventlog;");
    QVERIFY(retVal);
    while (m_fakeDB.isNext()) {
        count++;
    }
    QCOMPARE(count, recordCount);
}

void CBasicDBClassTest::test_getValues()
{
    int count = 0;
    bool retVal = m_fakeDB.exec("select * from eventlog where rowid=1;");
    QVERIFY(retVal);

    while (m_fakeDB.isNext()) {
        QCOMPARE(m_fakeDB.geValue(0).toString(), QString("mr_data"));
        QCOMPARE(m_fakeDB.geValue(1).toString(), QString("2023-05-09T11:19:57.36Z"));
        QCOMPARE(m_fakeDB.geValue(2).toString(), QString("edbfa4ea24038861"));

        QCOMPARE(m_fakeDB.geValue(4).toString(), QString("username: mr_data,@N@  type: PASSWORD,@N@  ip address: 192.0.2.211, 10.10.10.10"));
        QCOMPARE(m_fakeDB.geValue(5).toString(), QString("mr_data"));
        QCOMPARE(m_fakeDB.geValue(6).toString(), QString("PASSWORD"));
        QCOMPARE(m_fakeDB.geValue(7).toString(), QString("192.0.2.211"));
        QCOMPARE(m_fakeDB.geValue(8).toString(), QString("10.10.10.10"));
        QCOMPARE(m_fakeDB.geValue(9).toString(), QString("2023-05-09T14:19:57.360"));
        count++;
    }
    QCOMPARE(count, 1);
}

void CBasicDBClassTest::test_truncateTable()
{
    QFile file(SRCDIR"data/fakedb.db");
    qint64 oldSize = file.size();
    qDebug() << "Old size: " << oldSize;

    bool retVal = m_fakeDB.truncateTable("eventlog");
    QVERIFY(retVal);
    TDataList res = m_fakeDB.findInDB("SELECT name FROM sqlite_master WHERE type='table' AND name='eventlog';", false);
    QVERIFY(res.isEmpty());

    qint64 newSize = file.size();
    qDebug() << "New size: " << newSize;

    QVERIFY(oldSize != newSize);
}

void CBasicDBClassTest::test_InsertBindedValues()
{
    test_createTable();
    bool retVal = m_fakeDB.prepareRequest(insertOriginalData);
    QVERIFY(retVal);

    TDataItem data;
    data[QStringLiteral(":username")] = "mr_data";
    data[QStringLiteral(":timestampISO8601")] = "2023-05-09T11:19:57.36Z";
    data[QStringLiteral(":timestamp")] = "2023-05-09T14:19:57.360";
    data[QStringLiteral(":requestid")] = "edbfa4ea24038861";
    data[QStringLiteral(":type")] = "Вхід користувача - успішно";
    data[QStringLiteral(":details")] = "username: mr_data,@N@  type: PASSWORD,@N@  ip address: 192.0.2.211, 10.10.10.10";
    data[QStringLiteral(":username1")] = "mr_data";
    data[QStringLiteral(":authtype")] = "PASSWORD";
    data[QStringLiteral(":externalip")] = "192.0.2.211";
    data[QStringLiteral(":internalip")] = "10.10.10.10";
    retVal = m_fakeDB.execRequest(data);
    QVERIFY(retVal);

    int count = 0;
    retVal = m_fakeDB.exec("select * from eventlog;");
    QVERIFY(retVal);
    while (m_fakeDB.isNext()) {
        count++;
    }
    QCOMPARE(count, 1);
}

QTEST_APPLESS_MAIN(CBasicDBClassTest)

#include "tst_cbasicdbclass.moc"
