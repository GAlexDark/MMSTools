/****************************************************************************
*
* The class for output messages to the console
* Copyright (C) 2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#ifndef CCONSOLEOUTPUT_H
#define CCONSOLEOUTPUT_H

#include <QTextStream>
#include <QObject>
#include <QMutex>

#ifdef Q_OS_WIN32
  #include <windows.h>
#endif

class CConsoleOutput : public QObject
{
    Q_OBJECT
public:
#ifndef Q_OS_WIN32
    using QObject::QObject;
#else
    explicit CConsoleOutput(QObject *parent = nullptr)
        : QObject{parent}
    {
        // ref: https://github.com/tsnsoft/CppConsoleUnicode
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
    }
#endif

    void
    outToConsole(const QString &msg) const
    {
        if (!msg.isEmpty()) {
            QMutexLocker locker(&mutex);
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
private:
    mutable QMutex mutex;
};


#endif // CCONSOLEOUTPUT_H
