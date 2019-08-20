#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QDateTime>
#include <QPalette>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QIcon>
#include <QLCDNumber>
#include "vlc/VideoWidget.h"
#include "vlc/Media.h"
#include "vlc/MediaPlayer.h"
#include "vlc/Common.h"
#include "vlc/Instance.h"
#include "vlc/VolumeSlider.h"
#include "vlc/SeekWidget.h"
#include "config.h"

class ClickableLabel;

class Piece
{
public:
    Piece() {setEmpty();} ;
    inline void setPiece(QDateTime start, QDateTime end, QString filename, double correction)
    {startTime = start;endTime = end; name = filename; coef = (correction==0) ? 1 : correction;};
    inline void setPiece(Piece &p)
    {startTime = p.startTime;endTime = p.endTime; name = p.name; coef = p.coef;};

    inline void setEmpty() {startTime = QDateTime(); endTime = QDateTime();};
    inline bool within(const QDateTime &t) {return !startTime.isNull()
                                    && startTime <= t &&
                                  ( endTime.isNull() ?
                                      startTime.addSecs(DEFAULT_VIDEO_PIECE_MAX_LENTH_SEC) > t :
                                      endTime > t); };
    inline bool isLoading() { return endTime.isNull() && !startTime.isNull(); };
    inline double secsTo(const QDateTime &t) {
        return startTime.secsTo(t)/coef;
    };
    inline QString getFilename() { return name; };
    inline QDateTime addMSecs(qint64 secs) { return startTime.addMSecs(secs*coef); };
    inline void setEnd(const QDateTime &t) { if (endTime.isNull()) endTime = t;};
    inline QDateTime getEnd() { return endTime; };
    inline QDateTime getStart() { return startTime; };


private:
    QDateTime startTime;
    QDateTime endTime;
    QString   name;
    double    coef;
};

class Player : public QFrame
{
    Q_OBJECT
public:
    explicit Player(const QString &cam, QWidget *parent = 0);
    ~Player();

    inline int getVideoWidth() {return videoWidth;};
    inline int getVideoHeight() {return videoHeight;};
    int getAdditionHeight();
    int getAdditionWidth();

    void setFrontState(bool front);
    void setAudioVolume(int volume);
    void setPaused(bool pause);
    void setSpeed(double value);
    inline bool paused() {return m_paused; };
    //void setStopend(bool stop);
    void setSizeInfo(bool);
    QDateTime time();
    inline bool videoAvailable() {return m_video; };

signals:
    void fullscreen();
    //void rightClick(const QPoint);
    void activated();
    void timeChanged(const QDateTime &);
    void noVideoAvailable();
    void togglePause();

public slots:
    void setTime(const QDateTime &time);
    void updateTime(const QDateTime &time);
    void stop();

protected:
    //void enterEvent(QEvent *);
    //void leaveEvent(QEvent *);

private slots:
    void open(const QString &url, const int &time=0);
    void updateCurrentTime(int time=0);
    void endVideoPiece();
    void updateFullTime(int time);
    void videofullscreen();
    void videoLeftClick(const QPoint);
    void makeToolBar();
    void setVolume(const int &volume);
    //void pause();
    void volumeClicked();
    void playClicked();
    void sizeClicked();
    void photoClicked();

private:
    void updateVideoInfo();
    bool getVideoPiece(const QDateTime &time, bool next = false);
    void setShowVideo(bool video, const QDateTime &time);

    VlcInstance     *vlcInstance;
    VlcVideoWidget  *vlcWidget;

    VlcMediaPlayer  *vlcPlayer;
    VlcVolumeSlider *vlcVolume;
    VlcSeekWidget   *vlcSeek;
    QLabel          *timeLabel;
    ClickableLabel  *info;

    QString connectionName;

    QString camName;
    QString camFilePath;
    QString camHumanName;

    int     videoWidth;
    int     videoHeight;
    int     videoRotate;

    QDateTime crntTime;
    Piece nearestPiece;
    Piece crntPiece;
    Piece lastPiece;

    QPalette *plt;
    QPalette *pltHighlighted;
    QToolBar *toolBar;

    QToolButton *volumeButton;
    QList<QIcon> volumeIcons;
    QToolButton *playButton;
    QIcon        playIcons[2];
    QToolButton *photoButton;
    QToolButton *sizeButton;
    QIcon        sizeIcons[2];



    bool      frontState;

    int       m_audioVolume;
    bool      m_paused;
    bool      m_video;
    bool      m_unmuted;
    bool      m_muted;
    bool      m_evailable;

};

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = 0);
signals:
    void doubleClick();

protected:
    void mouseDoubleClickEvent(QMouseEvent *);
};

#endif // PLAYER_H
