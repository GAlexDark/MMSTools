#include "OptionsDialog.h"
#include "qpushbutton.h"
#include "ui_OptionsDialog.h"
#include <QRegularExpression>
#include <QMessageBox>

#include "elcUtils.h"

void
OptionsDialog::clearLists()
{
    m_includeUsersList.clear();
    m_excludeUsersList.clear();
}

OptionsDialog::OptionsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/data-transformation.png"));

    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Apply"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    bool retVal = connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(doOkClicked()));
    Q_ASSERT_X (retVal, "OptionsDialog::OptionsDialog", "doOkClicked() connection is not established");
    retVal = connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(doCancelClicked()));
    Q_ASSERT_X (retVal, "OptionsDialog::OptionsDialog", "doCancelClicked() connection is not established");

    clearLists();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void
OptionsDialog::doOkClicked()
{
    QString buf = ui->edIncludedUsers->text().trimmed();
    if (!buf.isEmpty()) { //parse only the include users list, the exclude users list is ignored
        m_includeUsersList = elcUtils::parseValuesList(buf);
        buf.clear();
        for (qsizetype i = 0; i < m_includeUsersList.size(); ++i) {
            buf = m_includeUsersList.at(i);
            if (!elcUtils::sanitizeValue(buf)) {
                QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Invalid character in the value %1").arg(buf), QMessageBox::Ok);
                m_includeUsersList.clear();
                break;
            }
        }
    } else { // parse the exclude users list
        buf.clear();
        buf = ui->edExcludedUsers->text().trimmed();
        m_excludeUsersList = elcUtils::parseValuesList(buf);
        buf.clear();
        for (qsizetype i = 0; i < m_excludeUsersList.size(); ++i) {
            buf = m_excludeUsersList.at(i);
            if (!elcUtils::sanitizeValue(buf)) {
                QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Invalid character in the value %1").arg(buf), QMessageBox::Ok);
                m_excludeUsersList.clear();
                break;
            }
        }
    }
    close();
}

void
OptionsDialog::doCancelClicked()
{
    clearLists();
    close();
}
