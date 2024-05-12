#include "CReportOptionsDialog.h"
#include "qpushbutton.h"
#include "ui_CReportOptionsDialog.h"
#include <QMessageBox>

#include "CElcGuiAppSettings.h"
#include "elcUtils.h"

void CReportOptionsDialog::clearLists() {
  m_includeUsersList.clear();
  m_excludeUsersList.clear();
}

CReportOptionsDialog::CReportOptionsDialog(const quint16 logID,
                                           const QStringList &logsList,
                                           QWidget *parent)
    : QDialog(parent), ui(new Ui::CReportOptionsDialog), m_logID(logID) {
  ui->setupUi(this);
  setWindowIcon(QIcon(":/img/data-transformation.png"));

  ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Apply"));

  bool retVal = connect(ui->buttonBox->button(QDialogButtonBox::Apply),
                        SIGNAL(clicked()), this, SLOT(doOkClicked()));
  Q_ASSERT_X(retVal, "OptionsDialog::OptionsDialog",
             "doOkClicked() connection is not established");

  ui->comboBox->addItems(logsList);
  ui->comboBox->setCurrentIndex(m_logID);

  const CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
  QString defChars = settings.getAllowedChars();
  QRegularExpression mask(QStringLiteral("^([%1,;]+)$").arg(defChars));
  m_rev.reset(new QRegularExpressionValidator(mask, this));
  ui->edIncludedUsers->setValidator(m_rev.data());
  ui->edExcludedUsers->setValidator(m_rev.data());
  clearLists();
}

CReportOptionsDialog::~CReportOptionsDialog() { delete ui; }

bool CReportOptionsDialog::getOptions(quint16 &logID,
                                      QStringList &includeUsersList,
                                      QStringList &excludeUsersList) {
  logID = m_logID;
  includeUsersList = m_includeUsersList;
  excludeUsersList = m_excludeUsersList;
  return m_isOkClicked;
}

void CReportOptionsDialog::doOkClicked() {
  bool isError = false;

  int logID = ui->comboBox->currentIndex();
  m_logID = logID >= 0 ? logID : 0;
  if (m_logID == 0) {
    QMessageBox::warning(nullptr, QObject::tr("Warning"),
                         QObject::tr("The MMS report type is not selected."),
                         QMessageBox::Ok);
    isError = true;
  } else {
    QString buf1 = ui->edIncludedUsers->text().trimmed();
    QString buf2 = ui->edExcludedUsers->text().trimmed();
    bool isIncluded = !buf1.isEmpty();
    bool isExcluded = !buf2.isEmpty();
    if (isIncluded && isExcluded) {
      QMessageBox::warning(nullptr, QObject::tr("Warning"),
                           QObject::tr("User exclusion and inclusion lists "
                                       "cannot be specified at the same time."),
                           QMessageBox::Ok);
      isError = true;
    } else {
      if (isIncluded) {
        m_includeUsersList = elcUtils::parseValuesList(buf1);
        m_excludeUsersList.clear();
      }
      if (isExcluded) {
        m_excludeUsersList = elcUtils::parseValuesList(buf2);
        m_includeUsersList.clear();
      }
    }
  }

  if (!isError) {
    m_isOkClicked = true;
    close();
  }
}

void CReportOptionsDialog::doCancelClicked() {
  clearLists();
  m_logID = 0;
  m_isOkClicked = false;
  close();
}
