#include <QtTest>

#include <QFile>
#include "CDataSourceList.h"

// add necessary includes here

const int filesCount = 16;

void
createTestFile(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QIODeviceBase::WriteOnly)) {
        f.write("test");
        f.close();
    }
}

class CDataSourceListTest : public QObject
{
    Q_OBJECT

public:
    CDataSourceListTest();
    ~CDataSourceListTest();

private:
    QString m_basePath,
            m_newDir;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_dataSourceListCount();
};

CDataSourceListTest::CDataSourceListTest(): m_basePath(SRCDIR"data"), m_newDir("source")
{}

CDataSourceListTest::~CDataSourceListTest() {}

void CDataSourceListTest::initTestCase()
{
    QDir dir(m_basePath);
    bool retVal = dir.mkdir(m_newDir);
    if (retVal) {
        QString tstPath = m_basePath + '/' + m_newDir + "/file";
        tstPath = QDir::fromNativeSeparators(tstPath);
        QString fileTempl = "%1%2.csv";
        for (int i = 0; i < filesCount; ++i) {
            createTestFile(fileTempl.arg(tstPath).arg(i));
        }
    }
    QVERIFY(retVal);
}

void CDataSourceListTest::cleanupTestCase()
{
    QString tstPath = m_basePath + '/' + m_newDir;
    QDir dir(tstPath);
    dir.removeRecursively();
}

void CDataSourceListTest::test_dataSourceListCount()
{
    QString tstPath = m_basePath + '/' + m_newDir;
    QStringList retVal = CDataSourceList::getDataSourceList(tstPath, QStringList() << "*.csv");
    QCOMPARE(retVal.size(), filesCount);
}

QTEST_APPLESS_MAIN(CDataSourceListTest)

#include "tst_cdatasourcelisttest.moc"
