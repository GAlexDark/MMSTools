#ifndef CLOADFILESOPTIONSDIALOG_H
#define CLOADFILESOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class CLoadFilesOptionsDialog;
}

class CLoadFilesOptionsDialog : public QDialog {
  Q_OBJECT

public:
  explicit CLoadFilesOptionsDialog(const QStringList &logsList,
                                   QWidget *parent = nullptr);
  ~CLoadFilesOptionsDialog();

  bool getOptions(quint16 &logID, bool &hasHeaders);

private slots:
  void doOkClicked();

private:
  Ui::CLoadFilesOptionsDialog *ui;

  bool m_hasHeaders = false;
  bool m_isOkClicked = false;
  quint16 m_logID = 0;
};

#endif // CLOADFILESOPTIONSDIALOG_H
