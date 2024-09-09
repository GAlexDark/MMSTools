/****************************************************************************
*
* The report options Dialog for the MMS Event Log Conversion GUI Utility.
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

#ifndef CREPORTOPTIONSDIALOG_H
#define CREPORTOPTIONSDIALOG_H

#include <QDialog>
#include <QRegularExpressionValidator>
#include <QScopedPointer>

namespace Ui {
class CReportOptionsDialog;
}

class CReportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CReportOptionsDialog(const quint16 logID, const QStringList &logsList, QWidget *parent = nullptr);
    ~CReportOptionsDialog();
    bool getOptions(quint16 &logID, QStringList &includeUsersList, QStringList &excludeUsersList);

private slots:
    void doOkClicked();
    void doCancelClicked();

private:
    Ui::CReportOptionsDialog *ui;
    QScopedPointer<QRegularExpressionValidator> m_rev;
    quint16 m_logID = 0;
    bool    m_isOkClicked = false;

    QStringList m_includeUsersList;
    QStringList m_excludeUsersList;

    void clearLists();
};

#endif // CREPORTOPTIONSDIALOG_H
