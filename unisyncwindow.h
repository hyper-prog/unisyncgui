/* **********************************************************
    UniSyncGui - Graphical User interface for UNISYNC
     http://hyperprog.com

    (C) 2014-2019 Peter Deak (hyper80@gmail.com)

    License: GPLv2  http://www.gnu.org/licenses/gpl-2.0.html
************************************************************* */
#ifndef UNISYNCWINDOW_H
#define UNISYNCWINDOW_H

#include <QtCore>
#include <QWidget>
#include <QProcess>

#define VERSION "1.0"

#ifdef _WIN32
#define UNISYNC_PROGRAMNAME "unisync.exe"
#else
#define UNISYNC_PROGRAMNAME "unisync"
#endif

class HConsolePanel;
namespace Ui {
class UniSyncWindow;
}

class UniSyncWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit UniSyncWindow(QWidget *parent = 0);
    ~UniSyncWindow();

    int changeFunc(void);
    int composeParameters(void);
    QString pExclGet(QString what);

public slots:
    int run(void);
    int errorOccured( QProcess::ProcessError error);
    int dataArrived(void);
    int usStarted(void);
    int usFinished(int exitCode, QProcess::ExitStatus exitStatus);

    int somethingChanged(void);
    int pFuncSelected(int i);

    int pScat(void);
    int pSsrc(void);
    int pSdst(void);
    int pSupd(void);

    int pExclF(void);
    int pExclD(void);
    int pExclP(void);
    int pDelExclude(void);

private:
    HConsolePanel *cPanel;
    QTimer  timer;
    QString command;
    QStringList args;
    QProcess *unisync;
    Ui::UniSyncWindow *ui;

    QList<QString> excl_f;
    QList<QString> excl_d;
    QList<QString> excl_p;
};

#endif // UNISYNCWINDOW_H
