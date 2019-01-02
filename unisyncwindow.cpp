/* **********************************************************
    UniSyncGui - Graphical User interface for UNISYNC
     http://hyperprog.com

    (C) 2014-2019 Peter Deak (hyper80@gmail.com)

    License: GPLv2  http://www.gnu.org/licenses/gpl-2.0.html
************************************************************* */

#include <QtGui/QApplication>

#include <dconsole.h>

#include "unisyncwindow.h"
#include "ui_unisyncwindow.h"

UniSyncWindow::UniSyncWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UniSyncWindow)
{
    unisync = NULL;
    ui->setupUi(this);

    cPanel = new HConsolePanel(ui->workFrame);
    QHBoxLayout *lay = new QHBoxLayout(ui->workFrame);
    lay->setSpacing(1);
    lay->setMargin(1);
    lay->addWidget(cPanel);

    cPanel->setPromptString("");
    cPanel->setCommandLineCharacterEcho(true);
    cPanel->setTextTypeColor(1,QColor(83,125,250));
    cPanel->setTextTypeColor(2,QColor(250,50,50));
    cPanel->setTextTypeColor(3,QColor(255,179,10));
    cPanel->setMarginText("%00000s");
    cPanel->setColor("margin",QColor(160,160,160));
    cPanel->setColor("marginbg",QColor(30,30,30));
    connect(ui->buttonRun,SIGNAL(clicked()),this,SLOT(run()));
    connect(ui->cFunc,SIGNAL(activated(int)),this,SLOT(pFuncSelected(int)));

    connect(ui->rHashNo,SIGNAL(clicked()),this,SLOT(somethingChanged()));
    connect(ui->rHashMd5,SIGNAL(clicked()),this,SLOT(somethingChanged()));
    connect(ui->rHashSha2,SIGNAL(clicked()),this,SLOT(somethingChanged()));

    connect(ui->rVerb0,SIGNAL(clicked()),this,SLOT(somethingChanged()));
    connect(ui->rVerb1,SIGNAL(clicked()),this,SLOT(somethingChanged()));
    connect(ui->rVerb2,SIGNAL(clicked()),this,SLOT(somethingChanged()));

    connect(ui->cDontHash,SIGNAL(clicked()),this,SLOT(somethingChanged()));
    connect(ui->cModTimes,SIGNAL(clicked()),this,SLOT(somethingChanged()));
    connect(ui->cFixtime,SIGNAL(clicked()),this,SLOT(somethingChanged()));

    connect(ui->pCatalog,SIGNAL(clicked()),this,SLOT(pScat()));
    connect(ui->pSource,SIGNAL(clicked()),this,SLOT(pSsrc()));
    connect(ui->pDestination,SIGNAL(clicked()),this,SLOT(pSdst()));
    connect(ui->pUpdate,SIGNAL(clicked()),this,SLOT(pSupd()));

    connect(ui->pExclFile,SIGNAL(clicked()),this,SLOT(pExclF()));
    connect(ui->pExclDir,SIGNAL(clicked()),this,SLOT(pExclD()));
    connect(ui->pExclPath,SIGNAL(clicked()),this,SLOT(pExclP()));
    connect(ui->pDelEx,SIGNAL(clicked()),this,SLOT(pDelExclude()));

    excl_f.clear();
    excl_d.clear();
    excl_p.clear();
    changeFunc();
    timer.setInterval(100);
    timer.setSingleShot(false);

    connect(&timer,SIGNAL(timeout()),this,SLOT(dataArrived()));
    ui->labelAbout->setText(QString("UniSyncGui by Peter Deak (http://hyperprog.com/unisync) - Version: %1").arg(VERSION));
}

int UniSyncWindow::somethingChanged(void)
{
    composeParameters();
    return 0;
}

int UniSyncWindow::pScat(void)
{
    QString fn;
    if(command == "create")
        fn = QFileDialog::getSaveFileName(this,"Catalog file",QString(),"*.usc");
    if(command == "catdiff" || command == "makeupdate")
        fn = QFileDialog::getOpenFileName(this,"Catalog file",QString(),"*.usc");
    if(!fn.isEmpty())
        ui->lCatalog->setText(fn);
    somethingChanged();
    return 0;
}

int UniSyncWindow::pSsrc(void)
{
    QString dn;
    dn = QFileDialog::getExistingDirectory(this,"Folder");
    if(!dn.isEmpty())
        ui->lSource->setText(dn);
    somethingChanged();
    return 0;
}

int UniSyncWindow::pSdst(void)
{
    QString dn;
    dn = QFileDialog::getExistingDirectory(this,"Folder");
    if(!dn.isEmpty())
        ui->lDestination->setText(dn);
    somethingChanged();
    return 0;
}

int UniSyncWindow::pSupd(void)
{
    QString dn;
    dn = QFileDialog::getExistingDirectory(this,"Folder");
    if(!dn.isEmpty())
        ui->lUpdate->setText(dn);
    somethingChanged();
    return 0;
}

int UniSyncWindow::pFuncSelected(int i)
{
    changeFunc();
    composeParameters();
    return 0;
}

int UniSyncWindow::changeFunc(void)
{
    QString ctext;

    ctext = ui->cFunc->currentText();
    if(ctext.startsWith("CREATE"))
    {
        ui->pCatalog->setEnabled(true);
        ui->lCatalog->setText("???");
        ui->pSource->setEnabled(true);
        ui->lSource->setText("???");
        ui->pDestination->setEnabled(false);
        ui->lDestination->setText("");
        ui->pUpdate->setEnabled(false);
        ui->lUpdate->setText("");
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
        command = "create";
        return 0;
    }

    if(ctext.startsWith("DIFF"))
    {
        ui->pCatalog->setEnabled(false);
        ui->lCatalog->setText("");
        ui->pSource->setEnabled(true);
        ui->lSource->setText("???");
        ui->pDestination->setEnabled(true);
        ui->lDestination->setText("???");
        ui->pUpdate->setEnabled(false);
        ui->lUpdate->setText("");
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
        command = "diff";
        return 0;
    }

    if(ctext.startsWith("CATDIFF"))
    {
        ui->pCatalog->setEnabled(true);
        ui->lCatalog->setText("???");
        ui->pSource->setEnabled(true);
        ui->lSource->setText("???");
        ui->pDestination->setEnabled(false);
        ui->lDestination->setText("");
        ui->pUpdate->setEnabled(false);
        ui->lUpdate->setText("");
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
        command = "catdiff";
        return 0;
    }

    if(ctext.startsWith("SYNC"))
    {
        ui->pCatalog->setEnabled(false);
        ui->lCatalog->setText("");
        ui->pSource->setEnabled(true);
        ui->lSource->setText("???");
        ui->pDestination->setEnabled(true);
        ui->lDestination->setText("???");
        ui->pUpdate->setEnabled(false);
        ui->lUpdate->setText("");
        ui->cFixtime->setEnabled(true);
        command = "sync";
        return 0;
    }

    if(ctext.startsWith("MAKEUPDATE"))
    {
        ui->pCatalog->setEnabled(true);
        ui->lCatalog->setText("???");
        ui->pSource->setEnabled(true);
        ui->lSource->setText("???");
        ui->pDestination->setEnabled(false);
        ui->lDestination->setText("");
        ui->pUpdate->setEnabled(true);
        ui->lUpdate->setText("???");
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
        command = "makeupdate";
        return 0;
    }

    if(ctext.startsWith("MAKESYNCUPDATE"))
    {
        ui->pCatalog->setEnabled(false);
        ui->lCatalog->setText("");
        ui->pSource->setEnabled(true);
        ui->lSource->setText("???");
        ui->pDestination->setEnabled(true);
        ui->lDestination->setText("???");
        ui->pUpdate->setEnabled(true);
        ui->lUpdate->setText("???");
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
        command = "makesyncupdate";
        return 0;
    }

    if(ctext.startsWith("APPLYUPDATE"))
    {
        ui->pCatalog->setEnabled(false);
        ui->lCatalog->setText("");
        ui->pSource->setEnabled(false);
        ui->lSource->setText("");
        ui->pDestination->setEnabled(true);
        ui->lDestination->setText("???");
        ui->pUpdate->setEnabled(true);
        ui->lUpdate->setText("???");
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
        command = "applyupdate";
        return 0;
    }

    ui->pCatalog->setEnabled(false);
    ui->lCatalog->setText("");
    ui->pSource->setEnabled(false);
    ui->lSource->setText("");
    ui->pDestination->setEnabled(false);
    ui->lDestination->setText("");
    ui->pUpdate->setEnabled(false);
    ui->lUpdate->setText("");
    ui->cFixtime->setChecked(false);
    ui->cFixtime->setEnabled(false);
    command = "help";

    return 0;
}

int UniSyncWindow::composeParameters(void)
{
    args.clear();
    args.push_back(command);
    if(!ui->lCatalog->text().isEmpty() && ui->lCatalog->text() != "???")
        args.push_back( QString("cat:%1").arg(ui->lCatalog->text()) );
    if(!ui->lSource->text().isEmpty() && ui->lSource->text() != "???")
        args.push_back( ui->lSource->text() );
    if(!ui->lDestination->text().isEmpty() && ui->lDestination->text() != "???")
        args.push_back( ui->lDestination->text() );
    if(!ui->lUpdate->text().isEmpty() && ui->lUpdate->text() != "???")
        args.push_back( QString("update:%1").arg(ui->lUpdate->text()) );

    if(ui->rHashNo->isChecked())
    {
        args.push_back( "-nohash" );
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
    }
    if(ui->rHashMd5->isChecked())
    {
        args.push_back( "-md5" );
        if(command == "sync")
            ui->cFixtime->setEnabled(true);
    }
    if(ui->rHashSha2->isChecked())
    {
        args.push_back( "-sha2" );
        if(command == "sync")
            ui->cFixtime->setEnabled(true);
    }

    if(ui->cModTimes->isChecked())
        args.push_back( "-mtime" );

    if(ui->cDontHash->isChecked())
    {
        args.push_back( "-skiphash" );
        ui->cFixtime->setChecked(false);
        ui->cFixtime->setEnabled(false);
    }
    else
    {
        if(command == "sync" && !ui->rHashNo->isChecked())
            ui->cFixtime->setEnabled(true);
    }

    if(ui->cFixtime->isChecked())
        args.push_back( "-fixtime" );

    if(ui->rVerb1->isChecked())
        args.push_back( "-v" );
    if(ui->rVerb2->isChecked())
        args.push_back( "-vv" );

    QList<QString>::iterator i;
    for( i= excl_f.begin() ; i != excl_f.end() ; ++i)
        args.push_back(QString("-exclf=%1").arg(*i));

    for( i= excl_d.begin() ; i != excl_d.end() ; ++i)
        args.push_back(QString("-excld=%1").arg(*i));

    for( i= excl_p.begin() ; i != excl_p.end() ; ++i)
        args.push_back(QString("-exclp=%1").arg(*i));

    QString f = "unisync ";
    f += args.join(" ");
    ui->labelCmdLine->setText(f);
    return 0;
}

int UniSyncWindow::run(void)
{
    cPanel->clearText();
    unisync = new QProcess(this);
    unisync->setReadChannelMode(QProcess::MergedChannels);
    connect(unisync,SIGNAL(error(QProcess::ProcessError)),this,SLOT(errorOccured(QProcess::ProcessError)));
    connect(unisync,SIGNAL(started()),this,SLOT(usStarted()));
    connect(unisync,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(usFinished(int,QProcess::ExitStatus)));
    args.push_back("-guicall");
    unisync->start(UNISYNC_PROGRAMNAME,args,QIODevice::ReadWrite);
    return 0;
}

int UniSyncWindow::errorOccured( QProcess::ProcessError error)
{
#ifdef _WIN32
    QFile f(UNISYNC_PROGRAMNAME);
    if(!f.exists())
        QMessageBox::critical(this,"Error","I can not find the unisync.exe!");
    else
        QMessageBox::critical(this,"Error","The unisync process can not be started!");
#else
    QMessageBox::critical(this,"Error","The unisync process can not be started!");
#endif
    return 0;
}

int UniSyncWindow::dataArrived(void)
{
    QString data;
    QByteArray ba;
    if(unisync->bytesAvailable() > 0)
    {
        ba = unisync->readAll();
        data = QString::fromLocal8Bit(ba);

        QStringList lst = data.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
        while(!lst.isEmpty())
        {
            int typ=1;
            QString s = lst.first();
            if(s.startsWith("Error,"))
                typ = 2;
            if(s.startsWith("DELETED FOLDER:") ||
               s.startsWith("DELETED FILE:") ||
               s.startsWith("NEW FOLDER:") ||
               s.startsWith("NEW FILE:") ||
               s.startsWith("MODIFIED FOLDER:") ||
               s.startsWith("MODIFIED FILE:") ||
               s.startsWith("The folders seem to be identical")
              )
                typ = 3;
            cPanel->addText(s,typ);
            lst.pop_front();
        }
    }
    return 0;
}

int UniSyncWindow::usStarted(void)
{
    ui->buttonRun->setEnabled(false);
    ui->paramBox->setEnabled(false);
    timer.start();
    return 0;
}

int UniSyncWindow::usFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    timer.stop();
    dataArrived();
    ui->buttonRun->setEnabled(true);
    if(exitCode == 0)
        QMessageBox::information(this,"Information","The process successfully finished");
    else
        QMessageBox::warning(this,"Warning","The process finished with error status");

    ui->paramBox->setEnabled(true);
    excl_f.clear();
    excl_d.clear();
    excl_p.clear();
    args.clear();
    changeFunc();
    composeParameters();
    return 0;
}

QString UniSyncWindow::pExclGet(QString what)
{
    QDialog *d = new QDialog(this);
    d->setWindowIcon(QIcon(":/icon.png"));
    d->setWindowTitle("Add exclude...");
    QVBoxLayout *lay = new QVBoxLayout(d);
    QHBoxLayout *blay = new QHBoxLayout(NULL);
    QLabel *la = new QLabel(d);
    QLineEdit *le = new QLineEdit(d);
    QPushButton *ok = new QPushButton(d);
    ok->setText("Ok");
    la->setText(QString("Add %1 name to exclude").arg(what));
    lay->addWidget(la);
    lay->addWidget(le);
    blay->addStretch();
    blay->addWidget(ok);
    blay->addStretch();
    lay->addLayout(blay);
    connect(ok,SIGNAL(clicked()),d,SLOT(accept()));
    if(d->exec())
        return le->text();
    delete d;
    return QString();
}

int UniSyncWindow::pExclF(void)
{
    QString s;
    s = pExclGet("file");
    if(!s.isEmpty())
        excl_f.push_back(s);
    somethingChanged();
    return 0;
}

int UniSyncWindow::pExclD(void)
{
    QString s;
    s = pExclGet("directory");
    if(!s.isEmpty())
        excl_d.push_back(s);
    somethingChanged();
    return 0;
}

int UniSyncWindow::pExclP(void)
{
    QString s;
    s = pExclGet("path");
    if(!s.isEmpty())
        excl_p.push_back(s);
    somethingChanged();
    return 0;
}

int UniSyncWindow::pDelExclude(void)
{
    excl_f.clear();
    excl_d.clear();
    excl_p.clear();
    somethingChanged();
    return 0;
}

UniSyncWindow::~UniSyncWindow()
{
    delete ui;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UniSyncWindow w;
    w.show();

    return a.exec();
}
