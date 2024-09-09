/****************************************************************************
*
* The Event Log Conversion GUI Utility Main file.
* Copyright (C) 2023-2024  Oleksii Gaienko
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QFile>
#include <QScopedPointer>

#include "MMSTypes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QScopedPointer<QLabel> m_state;
    QScopedPointer<QLabel> m_mode; // RDS | endpoint (single user)
    QString         m_lastDir;
    QString         m_dbName;
    QStringList     m_fileList;
    QByteArray      m_fdState;
    QString         m_errorString;

    bool            m_isButtonsDisabled = false;
    bool            m_hasHeaders = true;

    quint16         m_logId = 0;
    mms::ffs_t      m_ffs;

    void disableButtons();
    void enableButtons();

    void setStateText(const QString &state);
    void setModeText(const QString &mode);

    bool showReportOptionsDialog(const QStringList &logsList, quint16 &logID, QStringList &includeUsersList, QStringList &excludeUsersList);
    bool showReadFilesOptionsDialog(const QStringList &logsList, quint16 &logID, bool &hasHeaders);

private slots:
    //window menu
    void onAboutProgram();
    void onAboutQt();
    void onContact();

    void setInfoText(const QString &info);
    //
    void openFileClick();
    void convertEventLogClick();
    void clearDBclick();
    void generateReportClick();
};

#endif // MAINWINDOW_H
