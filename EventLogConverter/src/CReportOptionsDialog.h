#ifndef CREPORTOPTIONSDIALOG_H
#define CREPORTOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class CReportOptionsDialog;
}

class CReportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CReportOptionsDialog(const quint16 logID, const QStringList &logsList, QWidget *parent = nullptr);
    ~CReportOptionsDialog();
    void getOptions(quint16 &logID, QStringList &includeUsersList, QStringList &excludeUsersList);

private slots:
    void doOkClicked();
    void doCancelClicked();

private:
    Ui::CReportOptionsDialog *ui;
    quint16 m_logID;

    QStringList m_includeUsersList;
    QStringList m_excludeUsersList;

    void clearLists();
};

#endif // CREPORTOPTIONSDIALOG_H
