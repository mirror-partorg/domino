#ifndef VIDEOGRID_H
#define VIDEOGRID_H

#include <QWidget>
#include <QSplitter>
#include <QBoxLayout>
#include <QPalette>
#include "player.h"
#include "config.h"


class VideoGrid : public QSplitter
{
    Q_OBJECT
public:
    explicit VideoGrid(QWidget *parent = 0);
    void addItems(const QList<QStringList> &gridCamNames);
    void setAudioVolume(int volume);
    void setPaused(bool pause);
    void setSpeed(double value);

signals:
    void timeChanged(const QDateTime &);
    void paused(bool);

public slots:
    void play();
    void stop();
    void setTime(const QDateTime &time);
    void playerFullscreen();
    void playerActivated();
    void calcSize();
    void frontPlayerTimeChanged(const QDateTime &);
    void syncTime(const QDateTime &time);
    void playerNoVideoAvailable();
    void playerTogglePause();

protected:
    void resizeEvent(QResizeEvent *event);


private:
    bool tryCalcSize(int w, int h, QList<int> *rowSizes, QList< QList<int>* > *colsSizes);
    void updateFrontPlayer(Player *frontPlayer);

    //Qt::Orientation orientation; only Horizontal;

    QPalette     bgPallete;

    QSplitter   *vSplitter;
    //QSplitter   *hSplitter;

    QList<QSplitter*>          splitters;
    QList< QList<Player*>* >   gridPlayers;

    QList<Player*>  players;
    Player         *fullscreenPlayer;
    Player         *frontPlayer;

    bool isResizing;
    bool initSize;

    int  audioVolume;
    //QLabel *m_label;

};

#endif // VIDEOGRID_H
