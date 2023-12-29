#ifndef CCONSOLEOUTPUT_H
#define CCONSOLEOUTPUT_H

#include <QTextStream>

#ifdef Q_OS_WIN32
  #include <windows.h>
#endif

class CConsoleOutput : public QObject
{
    Q_OBJECT
public:
    CConsoleOutput()
    {
#ifdef Q_OS_WIN32
        // ref: https://github.com/tsnsoft/CppConsoleUnicode
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
#endif
    }
    virtual ~CConsoleOutput() {}
    void
    outToConsole(const QString &msg)
    {
        if (!msg.isEmpty()) {
            QTextStream cout(stdout);
            cout << msg << '\n';
            cout.flush();
        }
    }
public slots:
    void printToConsole(const QString &msg)
    {
        outToConsole(msg);
    }
};


#endif // CCONSOLEOUTPUT_H
