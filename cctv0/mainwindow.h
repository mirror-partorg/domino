#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "videogrid.h"
#include "vlc/VideoWidget.h"
#include "widgets/VolumeDial.h"
#include "widgets/SpeedDial.h"
#include "widgets/TimeControl.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setTime(const QDateTime &time);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void shutdown();
    void fullScreen();
    void newVolume(int value);
    void newSpeed(double);
    void muteToggled( bool mute );
    void paused( bool pause );
    void pauseToggled( bool pause );
    void gridTimeChanged(const QDateTime &);
    void dateTimeSelected(QDateTime);
    void playPause();
    void left();
    void right();
    void leftPage();
    void rightPage();
    void about();
    void param();
    void visibilityChanged(bool);

private:
    void createActions();
    void createToolBars();
    void readSettings();
    void writeSettings();
    void createGrid();

    QToolBar *controlToolBar;
    QDockWidget *eventDock;

    QAction  *quitAction;
    QAction  *fullScreenAction;
    QAction  *playAction;
    QAction  *leftAction;
    QAction  *rightAction;
    QAction  *leftPageAction;
    QAction  *rightPageAction;
    QAction  *aboutAction;
    QAction  *paramAction;
    QListWidget *customerList;
    VolumeDial *vol;
    SpeedDial  *speed;
    TimeControl   *timeControl;

    VideoGrid *grid;


};

#endif // MAINWINDOW_H
