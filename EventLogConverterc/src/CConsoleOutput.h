/****************************************************************************
*
*  Copyright (c) Oleksii Gaienko, 2024
*  Contact: galexsoftware@gmail.com
*
*  Event Log Conversion Utility
*  Event Log Conversion Console Utility
*
*  Module name: CConsoleOutput.h
*  Author(s): Oleksii Gaienko
*  Reviewer(s):
*
*  Abstract:
*     The class for output messages to the console
*
****************************************************************************/

#ifndef CCONSOLEOUTPUT_H
#define CCONSOLEOUTPUT_H

#include <QTextStream>
#include <QObject>

#ifdef Q_OS_WIN32
  #include <windows.h>
#endif

class CConsoleOutput : public QObject
{
    Q_OBJECT
public:
    explicit CConsoleOutput(QObject *parent = nullptr)
        : QObject{parent}
    {
#ifdef Q_OS_WIN32
        // ref: https://github.com/tsnsoft/CppConsoleUnicode
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
#endif
    }
    void
    outToConsole(const QString &msg) const
    {
        if (!msg.isEmpty()) {
            QTextStream cout(stdout);
            cout << msg << '\n';
            cout.flush();
        }
    }
public slots:
    void printToConsole(const QString &msg) const
    {
        outToConsole(msg);
    }
};


#endif // CCONSOLEOUTPUT_H
