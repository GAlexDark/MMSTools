#ifndef CREPORTOPTIONSDIALOG_H
#define CREPORTOPTIONSDIALOG_H

#include <QDialog>
#include <QRegularExpressionValidator>
#include <QScopedPointer>

namespace Ui {
class CReportOptionsDialog;
}

class CReportOptionsDialog : public QDialog {
  Q_OBJECT

public:
  explicit CReportOptionsDialog(const quint16 logID,
                                const QStringList &logsList,
                                QWidget *parent = nullptr);
  ~CReportOptionsDialog();
  bool getOptions(quint16 &logID, QStringList &includeUsersList,
                  QStringList &excludeUsersList);

private slots:
  void doOkClicked();
  void doCancelClicked();

private:
  Ui::CReportOptionsDialog *ui;
  QScopedPointer<QRegularExpressionValidator> m_rev;
  quint16 m_logID = 0;
  bool m_isOkClicked = false;

  QStringList m_includeUsersList;
  QStringList m_excludeUsersList;

  void clearLists();
};

#endif // CREPORTOPTIONSDIALOG_H
