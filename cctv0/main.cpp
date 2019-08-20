#include <QApplication>
#include <QSettings>
#include <QTextCodec>
#include <QtGui>
#include <QSplashScreen>

#include "mainwindow.h"
#include "config.h"
#include "qsqlconnectiondialog.h"
#include "paramdialog.h"
#include "utils/utils.h"

#ifdef Q_WS_X11
    #include <X11/Xlib.h>
#endif

QString *appname = NULL;

QString APPLICATION_NAME1()
{
    if ( !appname )
        appname = new QString(APPLICATION_NAME);
    return *appname;
}

void SetApplicationName(const char *program)
{
    QFileInfo info(program);
    QString name;
    name = info.baseName().toLower();
    appname = new QString(name);
}

bool init(int argc, char **argv)
{

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    QString piecesPath;

    settings.setValue("no-audio", false);
    settings.setValue("verbose", false);
    settings.setValue("no-transform", false);
    for ( int i = 0; i < argc; i++ ) {

        if ( !strcmp(argv[i], "--no-audio") )
            settings.setValue("no-audio", true);

        if ( !strcmp(argv[i], "--verbose") )
            settings.setValue("verbose", true);

        if ( !strcmp(argv[i], "--no-transform") )
            settings.setValue("no-transform", true);
    }

    piecesPath = settings.value("piece-path").toString();
    //piecesPath = QString("smb://backup-1/video/pieces");
    if ( piecesPath.isEmpty() || ! QFileInfo(piecesPath).exists() ) {
        QFileDialog d;
        QString sad, s;
        piecesPath = d.getExistingDirectory(NULL, "Выберите директорию видео", piecesPath, QFileDialog::DontUseSheet);
        //piecesPath = d.getOpenFileName(NULL,QString("Выберите директорию видео"), piecesPath, sad, &s, QFileDialog::ShowDirsOnly);

        if ( piecesPath.isEmpty() )
            return NULL;
        settings.setValue("piece-path", piecesPath);
    }
    QList<QStringList> camNames;

    for ( int i=0; i < 10; i++ ) {
        QStringList line;
        line = settings.value(QString("cams-%1").arg(i)).toStringList();
        if ( line.isEmpty() )
            break;
        camNames.append(line);
    }
    if ( camNames.isEmpty() ) {
        ParamDialog p;
        while ( true ) {
            if ( p.exec() != QDialog::Accepted )
                return false;
            if ( p.changed() ) {
                for ( int i=0; i < 10; i++ ) {
                    QStringList line;
                    line = settings.value(QString("cams-%1").arg(i)).toStringList();
                    if ( line.isEmpty() )
                        break;
                    camNames.append(line);
                }
                if ( !camNames.isEmpty() )
                    break;
            }
        }
    }


    if ( !QSqlConnectionDialog::initSqlConnection(&settings) )
        return NULL;

    return true;
}

int main(int argc, char *argv[])
{


#ifdef Q_WS_X11
//    XInitThreads();
#endif

    QApplication a(argc, argv);
    SetApplicationName(argv[0]);

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());

    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    a.setWindowIcon(QIcon(APPLICATION_ICON));
    a.setApplicationName(APPLICATION_NAME);
    a.setApplicationVersion(APPLICATION_VERSION);
    a.setOrganizationName(ORGANIZATION_NAME);
    a.setOrganizationDomain(ORGANIZATION_DOMAIN);

    a.setStyleSheet(settings.value("style-sheet").toString());

    QPixmap pixmap("/home/mirror/work/Qt/cctv0/images.old/icon128.png");
    pixmap = getPixmap("splash", 128, 128);
    QSplashScreen splash(pixmap);
    splash.show();
    splash.showMessage(QObject::tr(""),
                      Qt::AlignLeft | Qt::AlignTop, Qt::black);  //This line represents the alignment of text, color and position
    a.processEvents();

    if ( !init(argc, argv) )
        exit(1);


    MainWindow w;

    w.show();
    splash.finish(&w);
    w.setTime(QDateTime::currentDateTime().addSecs(-600));

    return a.exec();
}
