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

#include "CLoadFilesOptionsDialog.h"
#include "ui_CLoadFilesOptionsDialog.h"
#include "qpushbutton.h"
#include <QMessageBox>

CLoadFilesOptionsDialog::CLoadFilesOptionsDialog(const QStringList &logsList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CLoadFilesOptionsDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/data-transformation.png"));

    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Apply"));

    bool retVal = connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(doOkClicked()));
    Q_ASSERT_X (retVal, "LoadFilesOptionsDialog::LoadFilesOptionsDialog", "doOkClicked() connection is not established");

    ui->comboBox->addItems(logsList);
    ui->comboBox->setCurrentIndex(0);
}

CLoadFilesOptionsDialog::~CLoadFilesOptionsDialog()
{
    delete ui;
}

bool
CLoadFilesOptionsDialog::getOptions(quint16 &logID, bool &hasHeaders)
{
    logID =  m_logID;
    hasHeaders = m_hasHeaders;
    return m_isOkClicked;
}

void
CLoadFilesOptionsDialog::doOkClicked()
{
    bool yesChecked = ui->rbYes->isChecked();
    bool noChecked = ui->rbNo->isChecked();
    bool isError = false;

    if (!yesChecked && !noChecked) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("The presence/absence of headers in the file(s) is not specified."), QMessageBox::Ok);
        isError = true;
    } else {
        int logID = ui->comboBox->currentIndex();
        m_logID = logID >= 0 ? logID : 0 ;
        if (m_logID == 0) {
            QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("The MMS log type is not selected."), QMessageBox::Ok);
            isError = true;
        }
    }

    if (!isError) {
        m_hasHeaders = yesChecked;
        m_isOkClicked = true;
        close();
    }
}
