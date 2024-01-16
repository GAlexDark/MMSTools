
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QFile>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel         *m_state;
    QString         m_lastDir;
    QString         m_dbName;
    QStringList     m_fileList;
    QByteArray      m_fdState;

    bool            m_isButtonsDisabled;
    bool            m_hasHeaders;


    void disableButtons();
    void enableButtons();

    void setStateText(const QString &state);

    bool showOptionsDialog( QStringList &includeUsersList, QStringList &excludeUsersList);

private slots:
    //window menu
    void onAboutProgram();
    void onAboutQt();
    void onContact();

    void setInfoText(const QString &info);
    //
    void openFileClick();
    void convertEventLogClick();
    void clearDBclick();
    void generateReportClick();
};

#endif // MAINWINDOW_H
