/****************************************************************************
*
* The load files options Dialog for the MMS Event Log Conversion GUI Utility.
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

#ifndef CLOADFILESOPTIONSDIALOG_H
#define CLOADFILESOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class CLoadFilesOptionsDialog;
}

class CLoadFilesOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CLoadFilesOptionsDialog(const QStringList &logsList, QWidget *parent = nullptr);
    ~CLoadFilesOptionsDialog();

    bool getOptions(quint16 &logID, bool &hasHeaders);

private slots:
    void doOkClicked();

private:
    Ui::CLoadFilesOptionsDialog *ui;

    bool    m_hasHeaders = false;
    bool    m_isOkClicked = false;
    quint16 m_logID = 0;
};

#endif // CLOADFILESOPTIONSDIALOG_H
