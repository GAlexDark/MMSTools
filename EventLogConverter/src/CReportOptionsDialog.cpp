#include "CReportOptionsDialog.h"
#include "qpushbutton.h"
#include "ui_CReportOptionsDialog.h"
#include <QMessageBox>

#include "elcUtils.h"

void
CReportOptionsDialog::clearLists()
{
    m_includeUsersList.clear();
    m_excludeUsersList.clear();
}

CReportOptionsDialog::CReportOptionsDialog(const quint16 logID, const QStringList &logsList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CReportOptionsDialog), m_logID(logID)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/data-transformation.png"));

    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Apply"));

    bool retVal = connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(doOkClicked()));
    Q_ASSERT_X (retVal, "OptionsDialog::OptionsDialog", "doOkClicked() connection is not established");

    ui->comboBox->addItems(logsList);
    ui->comboBox->setCurrentIndex(m_logID);

    clearLists();
}

CReportOptionsDialog::~CReportOptionsDialog()
{
    delete ui;
}

bool
CReportOptionsDialog::getOptions(quint16 &logID, QStringList &includeUsersList, QStringList &excludeUsersList)
{
    logID = m_logID;
    includeUsersList = m_includeUsersList;
    excludeUsersList = m_excludeUsersList;
    return m_isOkClicked;
}

void
CReportOptionsDialog::doOkClicked()
{
    bool isError = false;

    int logID = ui->comboBox->currentIndex();
    m_logID = logID >= 0 ? logID : 0;
    if (m_logID == 0) {
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("The MMS report type is not selected."), QMessageBox::Ok);
        isError = true;
    } else {
        QString buf = ui->edIncludedUsers->text().trimmed();
        if (!buf.isEmpty()) { //parse only the include users list, the exclude users list is ignored
            m_includeUsersList = elcUtils::parseValuesList(buf);
            buf.clear();
            for (qsizetype i = 0; i < m_includeUsersList.size(); ++i) {
                buf = m_includeUsersList.at(i);
                if (!elcUtils::sanitizeValue(buf)) {
                    QMessageBox::warning(nullptr, QObject::tr("Warning"),
                                         QObject::tr("Invalid character in the value %1").arg(buf),
                                         QMessageBox::Ok);
                    m_includeUsersList.clear();
                    isError = true;
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
                    QMessageBox::warning(nullptr, QObject::tr("Warning"),
                                         QObject::tr("Invalid character in the value %1").arg(buf),
                                         QMessageBox::Ok);
                    m_excludeUsersList.clear();
                    isError = true;
                    break;
                }
            }
        }
    }

    if (!isError) {
        m_isOkClicked = true;
        close();
    }
}

void
CReportOptionsDialog::doCancelClicked()
{
    clearLists();
    m_logID = 0;
    m_isOkClicked = false;
    close();
}
