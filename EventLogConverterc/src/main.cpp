#include <QCoreApplication>
#include <QTextStream>
#ifdef Q_OS_WIN32
  #include <windows.h>
#endif
#include "Debug.h"
#include "CSVLoader.h"
#include "CReportBuilder.h"


QTextStream cout(stdout);

void
OutToConsole(const QString &msg)
{
    if (!msg.isEmpty()) {
        cout << msg << '\n';
        cout.flush();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString fileName, dbName, reportName;
    if (argc == 4) {
        fileName = argv[1];
        __DEBUG( fileName )
        dbName = argv[2];
        __DEBUG( dbName )
        reportName = argv[3];
        __DEBUG( reportName )
    } else {
        return 1;
    }

    QStringList files;
    files << fileName;

#ifdef Q_OS_WIN32
    // ref: https://github.com/tsnsoft/CppConsoleUnicode
    SetConsoleCP(65001); // Принудительная установка консоли ввода на UTF-8
    SetConsoleOutputCP(65001); // Принудительная установка консоли вывода на UTF-8
#endif

    OutToConsole("EventLog Converter console start");

    CSVThreadLoader loader;
    bool retVal = loader.init(dbName, true, "\r\n");
    if (retVal) {
        loader.setFileName(files);
        loader.start();

        OutToConsole("wait...");
        loader.wait();
        retVal = loader.getStatus();
    }

    if (retVal) {
        OutToConsole("File reading finished");
    } else {
        OutToConsole("Error reading file");
        OutToConsole(loader.errorString());
    }

    if (retVal) {
        OutToConsole("Generate report");
        CSVThreadReportBuilder report;
        if (report.init(dbName, reportName)) {
            report.start();

            OutToConsole("wait...");
            report.wait();
            retVal = report.getStatus();
        }

        if (retVal) {
            OutToConsole("Report generating finished");
        } else {
            OutToConsole("Error generate report");
            OutToConsole(report.errorString());
        }
    }


#ifndef QT_DEBUG
    system("pause");
#endif
    return 0;
    //return a.exec();
}
//=============================================================

