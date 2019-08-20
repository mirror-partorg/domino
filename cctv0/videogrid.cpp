#include "videogrid.h"
#include "player.h"
#include <QtGui>


VideoGrid::VideoGrid(QWidget *parent) :
    QSplitter(parent),
    bgPallete(QPalette::Window, QColor(PLAYER_BORDER_COLOR)),
    fullscreenPlayer(NULL),
    frontPlayer(NULL),
    isResizing(false),
    initSize(false),
    audioVolume(DEFAULT_AUDIO_VOLUME)
{

    QWidget *w;

    setMinimumSize(VIDEO_GRID_MIN_WIDTH, VIDEO_GRID_MIN_HEIGHT);
    setOrientation(Qt::Horizontal);
    setContentsMargins(0,0,0,0);
    setStyleSheet("QSplitter::handle { width: 0 }");
    setHandleWidth(VIDEO_GRID_HANDLE_WIDTH);

    vSplitter = new QSplitter();
    vSplitter->setContentsMargins(0,0,0,0);
    vSplitter->setHandleWidth(VIDEO_GRID_HANDLE_WIDTH);
    vSplitter->setOrientation(Qt::Vertical);

   /* w = new QWidget;
    w->setPalette(bgPallete);
    w->setAutoFillBackground(true);
    addWidget(w);*/

    addWidget(vSplitter);
    //m_label = new QLabel();

    w = new QWidget;
    //w->setPalette(bgPallete);
    //w->setAutoFillBackground(true);
    addWidget(w);

    //addWidget(m_label);
//    calcSize(VIDEO_GRID_MIN_WIDTH, VIDEO_GRID_MIN_HEIGHT);

}

void VideoGrid::addItems(const QList<QStringList> &gridCamNames)
{

    QWidget *w;

    foreach ( QStringList camNames, gridCamNames ) {

        QList<Player*> *line;
        QSplitter *splitter;

        splitter = new QSplitter();
        splitter->setContentsMargins(0,0,0,0);
        splitter->setHandleWidth(VIDEO_GRID_HANDLE_WIDTH);
        splitter->setOrientation(Qt::Horizontal);
        splitters.append(splitter);
        vSplitter->addWidget(splitter);

        line = new QList<Player*>();
        gridPlayers.append(line);

        foreach ( QString camName, camNames ) {
            Player *player;

            player = new Player(camName);
            //player->setEnabled(true);

            connect(player, SIGNAL(fullscreen()), this, SLOT(playerFullscreen()));
            connect(player, SIGNAL(activated()), this, SLOT(playerActivated()));
            connect(player, SIGNAL(noVideoAvailable()), this, SLOT(playerNoVideoAvailable()));
            connect(player, SIGNAL(togglePause()), this, SLOT(playerTogglePause()));

            players.append(player);
            line->append(player);
            splitter->addWidget(player);
        }
        w = new QWidget();
        w->setPalette(bgPallete);
        w->setAutoFillBackground(true);
        splitter->addWidget(w);//!!

    }
    w = new QWidget;
    //w->setPalette(bgPallete);
    //w->setAutoFillBackground(true);
    vSplitter->addWidget(w);
    if ( players.count() > 0 )
        updateFrontPlayer(players[0]);
}
void VideoGrid::play()
{
    /*foreach ( QList<Player*> *row, players )
        foreach ( Player *player, *row )
            player->play();*/
}

void VideoGrid::stop()
{
    foreach ( Player *player, players )
        player->stop();
}

void VideoGrid::setTime(const QDateTime &time)
{
    foreach ( Player *player, players )
        player->setTime(time);
}

void VideoGrid::resizeEvent(QResizeEvent *event)
{
    //if ( !initSize ) {
        initSize = true;
        calcSize();
/*    }
    else
        if ( !isResizing ) {
            isResizing = true;
            QTimer::singleShot(VIDEO_GRID_RESIZE_MSEC, this, SLOT(calcSize()));
        }*/
    QWidget::resizeEvent(event);
}

bool VideoGrid::tryCalcSize(int w, int h, QList<int> *rowSizes, QList< QList<int>* > *colsSizes)
{
    int hi;
    rowSizes->clear();
    colsSizes->clear();

    if ( w == 967 )
        w = 967;

    for ( int row=0; row < splitters.count(); row++ ) {
        double r;
        int wr;
        int mah, maw;
        int handleWidths;
        QList<Player*> *rowPlayers;

        rowPlayers = gridPlayers[row];
        handleWidths = VIDEO_GRID_HANDLE_WIDTH*(splitters[row]->count()-1);
        wr = w - handleWidths;
        r = 0;
        mah = 0;
        maw = 0;
        foreach ( Player *player, *rowPlayers ) {
            double ratio;
            ratio = player->getVideoWidth()/(1.*player->getVideoHeight());
            r += ratio;
            if ( mah < player->getAdditionHeight() )
                mah = player->getAdditionHeight();
            maw += player->getAdditionWidth();
        }
        hi = round((wr-maw)/r) + mah;
        rowSizes->append(hi);
        QList<int> *cols = new QList<int>();
        colsSizes->append(cols);
        foreach ( Player *player, *rowPlayers ) {
            int wi;
            double ratio;
            ratio = player->getVideoWidth()/(1.*player->getVideoHeight());
//            wi = round((hi-mah)*ratio) + player->getAdditionWidth();
            wi = (hi-mah)*ratio + player->getAdditionWidth();
            cols->append(wi);
        }
        cols->append(0);
    }
    hi=0;
    for ( int i =0; i < rowSizes->count(); i++ )
        hi+=(*rowSizes)[i]+VIDEO_GRID_HANDLE_WIDTH;

    rowSizes->append(h-hi);

    return ( h >= hi );
}

void VideoGrid::calcSize()
{
    isResizing = true;

    int w,h;
    int top,left,right,bottom;

    left=top=right=bottom=VIDEO_GRID_HANDLE_WIDTH+frameWidth();
    int width_;
    int height_;

    width_ = width();
    height_ = height();


    h = height_ - top - bottom;
    w = width_ - left - right;

    if ( fullscreenPlayer ) {

        QList<int> rowSizes;
        for ( int row=0; row < splitters.count(); row++ ) {
            QList<int> colSizes;
            bool isrow;
            isrow = false;
            foreach ( Player *player, *gridPlayers[row] ) {
                if ( player == fullscreenPlayer ) {
                    isrow = true;
                    colSizes.append(w);
                }
                else
                    colSizes.append(0);
            }
            splitters[row]->setSizes(colSizes);
            if ( isrow )
                rowSizes.append(h);
            else
                rowSizes.append(0);
        }
        vSplitter->setSizes(rowSizes);
        QList<int> sizes;
        sizes = QList<int>() << w << 0;
        setSizes(sizes);
        /*QList<int> sizes;
        sizes = sizes();
        sizes[0] = w;
        sizes[1] = width_ - w - VIDEO_GRID_HANDLE_WIDTH - left - right;
        setSizes(sizes);*/
    }
    else {

        QList<int> rowSizes;
        QList< QList<int>* > colSizes;

        for (; w > 100; w-- )
            if ( tryCalcSize(w,h, &rowSizes, &colSizes) ) {
                int new_w = 0;
                foreach ( QList<int>* line, colSizes ) {
                    int wa;
                    wa = 0;
                    foreach (int wc, *line)
                        wa+=wc;
                    if ( wa > new_w  )
                        new_w = wa;
                }
                w = new_w;
                foreach ( QList<int>* line, colSizes ) {
                    int wa;
                    wa = 0;
                    foreach (int wc, *line)
                        wa+=wc;
                    if ( wa < w  )
                        (*line)[line->count()-1] = (w - wa);
                }


                QList<int> sizes;
                int diff, diff1, wAll;
                wAll = width_ - VIDEO_GRID_HANDLE_WIDTH - left - right;
                diff = 0;//(wAll - w) / 2;
                diff1 = wAll - w - diff;
                //sizes.append(diff);
                sizes.append(w);
                sizes.append(diff1);
                setSizes(sizes);

                vSplitter->setSizes(rowSizes);
                for ( int row=0; row < splitters.count(); row++ ) {
                    splitters[row]->setSizes(*colSizes[row]);

                }
                break;
            }
        }

    isResizing = false;
    //m_label->setText(QString("%1\n%2").arg(m_label->width()).arg(width()));

}

void VideoGrid::updateFrontPlayer(Player *player)
{
    if ( player == frontPlayer )
        return;

    if ( frontPlayer )
        disconnect(frontPlayer, SIGNAL(timeChanged(QDateTime)), this, SLOT(frontPlayerTimeChanged(QDateTime)));

    frontPlayer = player;
    if ( player->videoAvailable() ) {
        if ( player->paused() ) {
            foreach (Player *p, players)
                p->setPaused(true);
            emit paused(true);
        }
    }

    foreach ( Player *player, players ) {
       player->setFrontState(player == frontPlayer);
    }
    connect(frontPlayer, SIGNAL(timeChanged(QDateTime)), this, SLOT(frontPlayerTimeChanged(QDateTime)));

    if ( player->videoAvailable() )
        emit frontPlayerTimeChanged(player->time());
}

void VideoGrid::playerFullscreen()
{
    if ( !fullscreenPlayer ) {
        fullscreenPlayer = (Player*)(this->sender());
        updateFrontPlayer(fullscreenPlayer);
        fullscreenPlayer->setSizeInfo(true);
        if ( fullscreenPlayer->videoAvailable() )
            foreach ( Player *player, players )
                if ( player != fullscreenPlayer ) 
                     player->setPaused(true);
    }
    else {
        QDateTime time;
        time = fullscreenPlayer->time();
        fullscreenPlayer->setSizeInfo(false);
        if ( fullscreenPlayer->videoAvailable() ) {
            foreach ( Player *player, players )
                if ( player != fullscreenPlayer ) {
                    player->setTime(time);
		    if ( !fullscreenPlayer->paused() )	
                       player->setPaused(false);
                }
        }
        fullscreenPlayer = NULL;
    }
    calcSize();
}
void VideoGrid::playerActivated()
{
    Player *player = (Player*)(this->sender());
    updateFrontPlayer(player);
}
void VideoGrid::setAudioVolume(int volume)
{
    if ( audioVolume == volume )
        return;
    audioVolume = volume;
    foreach ( Player *player, players )
       player->setAudioVolume(volume);

}
void VideoGrid::setPaused(bool pause)
{
    foreach ( Player *player, players )
        player->setPaused(pause);

}
void VideoGrid::setSpeed(double value)
{
    foreach ( Player *player, players )
        player->setSpeed(value);
}
void VideoGrid::syncTime(const QDateTime &time)
{
    foreach ( Player *player, players )
        player->updateTime(time);
}

void VideoGrid::frontPlayerTimeChanged(const QDateTime &time)
{
    //Player *player = (Player*)(this->sender());
    if ( !frontPlayer ||  !frontPlayer->videoAvailable() )
        return;
    syncTime(time);
    emit timeChanged(time);
}

void VideoGrid::playerNoVideoAvailable()
{
    Player *closePlayer = (Player*)(this->sender());
    if ( fullscreenPlayer == closePlayer )
        return;

    if ( closePlayer == frontPlayer ) {
        foreach ( Player *player, players )
            if ( player->videoAvailable() ) {
                updateFrontPlayer(player);
                break;
            }
    }

}
void VideoGrid::playerTogglePause()
{
    bool pause;
    Player *player = (Player*)(this->sender());
    pause = !player->paused();
    if ( player != frontPlayer ) {
        if ( pause )
            player->setPaused(true);
        else {
            bool allPaused;
            allPaused = true;
            foreach (Player *p, players )
                if ( !p->paused() ) {
                    allPaused = false;
                    break;
                }
            player->setPaused(false);
            if ( allPaused ) {
                updateFrontPlayer(player);
                emit paused(false);
            }
        }
    }
    else {
        if ( pause ) {
            foreach (Player *p, players)
                p->setPaused(pause);
        }
        else {
            bool allPaused;
            allPaused = true;
            foreach (Player *p, players )
                if ( !p->paused() ) {
                    allPaused = false;
                    break;
                }
            player->setPaused(false);
            if ( !allPaused ) {
                foreach (Player *p, players)
                    p->setPaused(pause);
            }
        }
        emit paused(pause);
    }
}
