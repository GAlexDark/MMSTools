#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    ~OptionsDialog();

    QStringList getIncludeUsersList() const { return m_includeUsersList; }
    QStringList getExcludeUsersList() const { return m_excludeUsersList; }

private slots:
    void doOkClicked();
    void doCancelClicked();

private:
    Ui::OptionsDialog *ui;

    QStringList m_includeUsersList;
    QStringList m_excludeUsersList;

    bool sanitizeValue(const QString &value);
    void clearLists();
};

#endif // OPTIONSDIALOG_H
