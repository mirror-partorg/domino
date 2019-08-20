#include "mainwindow.h"
#include <QtGui>
#include "videogrid.h"
#include "config.h"
#include "widgets/TimeControl.h"
#include "paramdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    createGrid();
    createActions();
    createToolBars();


    eventDock = new QDockWidget(tr("События"), this);
    //!!eventDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    customerList = new QListWidget(eventDock);

    eventDock->setWidget(customerList);
    addDockWidget(Qt::RightDockWidgetArea, eventDock);

    readSettings();
}
void MainWindow::setTime(const QDateTime &time)
{
    grid->setTime(time);
}

void MainWindow::createGrid()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    QList<QStringList> camNames;

    for ( int i=0; i < 10; i++ ) {
        QStringList line;
        line = settings.value(QString("cams-%1").arg(i)).toStringList();
        if ( line.isEmpty() )
            break;
        camNames.append(line);
    }
    grid = new VideoGrid();
    grid->addItems(camNames);
    setWindowTitle(APPLICATION_TITLE);

    //grid->setTime(QDateTime(QDate(2012,10,1),QTime(22,9,30)));

    setCentralWidget(grid);
    connect(grid, SIGNAL(timeChanged(QDateTime)), this, SLOT(gridTimeChanged(QDateTime)));
    connect(grid, SIGNAL(paused(bool)), this, SLOT(paused(bool)));
    //grid->setPaused(true);

}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    grid->stop();
    event->accept();
}


void MainWindow::readSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    //QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    //QSize size = settings.value("size", QSize(600, 400)).toSize();
    QByteArray state = settings.value("window-state").toByteArray();
    QByteArray geometry = settings.value("window-geometry").toByteArray();
    restoreGeometry(geometry);
    restoreState(state);

    //resize(size);
    //restoreState(state);
    //move(pos);
}
void MainWindow::writeSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    settings.setValue("volume", vol->value());
    if ( !isFullScreen() ) {
        settings.setValue("window-geometry", saveGeometry());
        settings.setValue("pos", pos());
        settings.setValue("size", size());
        settings.setValue("window-state", saveState());
    }
    else
        settings.setValue("window-state-fullscreen", saveState());

}
void MainWindow::createActions()
{
    quitAction = new QAction(QIcon(":/images/shutdown.png"), tr("Выйти"), this);

    quitAction->setStatusTip(tr("Выход"));
    quitAction->setShortcuts(QKeySequence::Quit);

    fullScreenAction = new QAction(QIcon(":/images/view-fullscreen.png"), tr("Полный экран"), this);
    QList<QKeySequence> *key;
    key = new QList<QKeySequence>();
    key->append(QKeySequence(Qt::Key_F11));
    fullScreenAction->setShortcuts(*key);
    fullScreenAction->setStatusTip(tr("Полноэкранный режим"));

    playAction = new QAction(QIcon("://images/media-start.png"), tr("Играть/Пауза"), this);
    key = new QList<QKeySequence>();
    key->append(QKeySequence(Qt::Key_Space));
    playAction->setShortcuts(*key);
    playAction->setStatusTip(tr("Играть/Пауза"));

    leftAction = new QAction(QIcon("://images/media-left.png"), tr("На 10 секунд назад"), this);
    key = new QList<QKeySequence>();
    key->append(QKeySequence(Qt::Key_Left));
    leftAction->setShortcuts(*key);
    leftAction->setStatusTip(tr("На 10 секунд назад"));

    rightAction = new QAction(QIcon("://images/media-right.png"), tr("На 10 секунд вперед"), this);
    key = new QList<QKeySequence>();
    key->append(QKeySequence(Qt::Key_Right));
    rightAction->setShortcuts(*key);
    rightAction->setStatusTip(tr("На 10 секунд вперед"));

    leftPageAction = new QAction(QIcon("://images/media-left-page.png"), tr("На 10 минут назад"), this);
    key = new QList<QKeySequence>();
    key->append(QKeySequence(Qt::Key_PageUp));
    leftPageAction->setShortcuts(*key);
    leftPageAction->setStatusTip(tr("На 10 минут назад"));

    rightPageAction = new QAction(QIcon("://images/media-right-page.png"), tr("На 10 минут вперед"), this);
    key = new QList<QKeySequence>();
    key->append(QKeySequence(Qt::Key_PageDown));
    rightPageAction->setShortcuts(*key);
    rightPageAction->setStatusTip(tr("На 10 минут вперед"));

    aboutAction = new QAction(tr("О программе"), this);

    paramAction = new QAction(tr("Настройки..."), this);


    connect(leftAction, SIGNAL(triggered()), this, SLOT(left()));
    connect(rightAction, SIGNAL(triggered()), this, SLOT(right()));
    connect(leftPageAction, SIGNAL(triggered()), this, SLOT(leftPage()));
    connect(rightPageAction, SIGNAL(triggered()), this, SLOT(rightPage()));
    connect(playAction, SIGNAL(triggered()), this, SLOT(playPause()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(shutdown()));
    connect(fullScreenAction, SIGNAL(triggered()), this, SLOT(fullScreen()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(paramAction, SIGNAL(triggered()), this, SLOT(param()));
}
void MainWindow::shutdown()
{
    writeSettings();
    grid->stop();
    exit(0);
}
void MainWindow::playPause()
{
    pauseToggled(!speed->paused());
}

void MainWindow::createToolBars()
{

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());

    QMenu *fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(quitAction);
    fileMenu = menuBar()->addMenu(tr("&Действия"));

    fileMenu->addAction(playAction);
    fileMenu->addAction(leftAction);
    fileMenu->addAction(rightAction);
    fileMenu->addAction(leftPageAction);
    fileMenu->addAction(rightPageAction);

    fileMenu = menuBar()->addMenu(tr("&Вид"));
    fileMenu->addAction(fullScreenAction);

    fileMenu = menuBar()->addMenu(tr("&Инструменты"));
    fileMenu->addAction(paramAction);

    fileMenu = menuBar()->addMenu(tr("&Помощь"));
    fileMenu->addAction(aboutAction);

    controlToolBar = new QToolBar(tr("Действия"));
    addToolBar(Qt::TopToolBarArea, controlToolBar);
    controlToolBar->addAction(quitAction);
    controlToolBar->addAction(leftPageAction);
    controlToolBar->addAction(leftAction);
    controlToolBar->addAction(playAction);
    controlToolBar->addAction(rightAction);
    controlToolBar->addAction(rightPageAction);
    controlToolBar->addAction(fullScreenAction);

    connect(controlToolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)));
    controlToolBar = new QToolBar(tr("Управление"));
    addToolBar(Qt::TopToolBarArea, controlToolBar);

    speed = new SpeedDial();
    connect(speed, SIGNAL(speedChanged(double)), this, SLOT(newSpeed(double)));
    connect(speed, SIGNAL(pauseToggled(bool)), this, SLOT(pauseToggled(bool)));

    timeControl = new TimeControl();
    timeControl->setMinimumHeight(50);
    connect(timeControl, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dateTimeSelected(QDateTime)));


    vol = new VolumeDial();
    vol->setMaximum(200);
    vol->setValue(settings.value("volume", DEFAULT_AUDIO_VOLUME).toInt());
    newVolume(vol->value());
    connect(vol, SIGNAL(valueChanged(int)), this, SLOT(newVolume(int)));
    connect(vol, SIGNAL(muteToggled(bool)), this, SLOT(muteToggled(bool)));
    controlToolBar->setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);

    QWidget *w;
    /*w = new QWidget;
    w->setMinimumWidth(10);

    controlToolBar->addWidget(w);*/
    controlToolBar->addWidget(speed);
    controlToolBar->addWidget(timeControl);
    controlToolBar->addWidget(vol);
    w = new QWidget;
    w->setMinimumWidth(10);
    controlToolBar->addWidget(w);
}
void MainWindow::fullScreen()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    if ( isFullScreen() ) {
        settings.setValue("window-state-fullscreen", saveState());
        showNormal();
        restoreState(settings.value("window-state").toByteArray());
        menuBar()->setVisible(true);
    }
    else {
        settings.setValue("window-state", saveState());
        showFullScreen();
        restoreState(settings.value("window-state-fullscreen").toByteArray());
        menuBar()->setVisible(false);
    }
}
void MainWindow::newVolume(int volume)
{
    grid->setAudioVolume(vol->muted() ? 0 : volume);
}

void MainWindow::muteToggled( bool mute )
{
    vol->setMuted(mute);
    grid->setAudioVolume(mute ? 0 : vol->value());
}
void MainWindow::paused( bool pause )
{
    speed->setPaused(pause);
}
void MainWindow::left()
{
    dateTimeSelected(timeControl->time().addSecs(-10));
}
void MainWindow::right()
{
    dateTimeSelected(timeControl->time().addSecs(10));
}
void MainWindow::leftPage()
{
    dateTimeSelected(timeControl->time().addSecs(-10*60));
}
void MainWindow::rightPage()
{
    dateTimeSelected(timeControl->time().addSecs(10*60));
}

void MainWindow::pauseToggled( bool pause )
{
    speed->setPaused(pause);
    grid->setPaused(pause);
    if ( !pause )
        grid->setSpeed(speed->speed());
}
void MainWindow::newSpeed(double value)
{
    if ( !speed->paused() )
        grid->setSpeed(value);
}
void MainWindow::gridTimeChanged(const QDateTime &time)
{
    timeControl->setTime(time);
}
void MainWindow::dateTimeSelected(QDateTime dateTime)
{
    grid->setTime(dateTime);
    timeControl->setTime(dateTime);
}
void MainWindow::about()
{
    QMessageBox::about(this, tr("О программе"), tr("<b>%1 %2</b><br>© 2012 %3. Все права защищены.<br>%4").arg(APPLICATION_NAME).arg(APPLICATION_VERSION).arg(COPYRIGHT).arg(EMAIL));
}
void MainWindow::param()
{
    ParamDialog p(this);
    if ( p.exec() == QDialog::Accepted && p.changed() ) {
        QDateTime time;
        bool pause;
        time = timeControl->time();
        pause = speed->paused();

        grid->stop();
        delete grid;
        createGrid();
        if ( pause )
            grid->setPaused(true);
        newVolume(vol->value());
        grid->setTime(time);
    }
}
void MainWindow::visibilityChanged(bool visable)
{
    if ( !visable ) {
        if ( !menuBar()->isVisible() )
            menuBar()->setVisible(true);
    }
}
