#include <QtTest>

// add necessary includes here

class CSVLoaderTest : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    ~CSVLoaderTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();
};

CSVLoaderTest::~CSVLoaderTest() {}

void CSVLoaderTest::initTestCase() {}

void CSVLoaderTest::cleanupTestCase() {}

void CSVLoaderTest::test_case1() {}

QTEST_APPLESS_MAIN(CSVLoaderTest)

#include "tst_csvloader.moc"
