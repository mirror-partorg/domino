#include "player.h"
#include "vlc/Common.h"
#include "vlc/Error.h"

#include <QtGui>
#include <QtSql>
#include "log.h"
#include "config.h"
#include "qsqlconnectiondialog.h"
#include "utils/utils.h"

static int connectionIndex = 0;

Player::Player(const QString &cam, QWidget *parent) :
    QFrame(parent),
    m_audioVolume(DEFAULT_AUDIO_VOLUME),
    m_paused(false),
    m_video(false),
    m_unmuted(false),
    m_muted(false),
    m_evailable(false)
{

    connectionName = QString("%1-%2").arg(cam).arg(connectionIndex++);
    QSqlConnectionDialog::cloneDatabase(connectionName);


    camName = cam;
    crntTime = QDateTime::currentDateTime();//.addDays(-1);
    camHumanName = camName;
    frontState = false;

    updateVideoInfo();
    QVBoxLayout *layout;
    volumeIcons.append(QIcon(getPixmap("volumeMUTED", 16,16)));
    volumeIcons.append(QIcon(getPixmap("volumeLOW", 16,16)));
    volumeIcons.append(QIcon(getPixmap("volumeMEDIUM", 16,16)));
    volumeIcons.append(QIcon(getPixmap("volumeHIGH", 16,16)));

    layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    layout->setSpacing(0);
    setLayout(layout);
    QStringList args;
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());

    args = VlcCommon::args();

    if ( settings.value("no-audio").toBool() )
        args.append("--no-audio");

    if ( settings.value("verbose").toBool() )
        args.append("--verbose=121212");
    args.append("--verbose=0");
    //args.append("--ffmpeg-hw");

    if ( videoRotate ) {
        if ( settings.value("no-transform").toBool() ) {
            args.append("--video-filter=rotate");
            args.append(QString("--rotate-angle=%1").arg(360-videoRotate));
        }
        else {
            if ( vlcInstance->majorVersion() < 2  ) {
                args.append("--vout-filter=transform");
                args.append(QString("--transform-type=%1").arg(360-videoRotate));
            }
            else {
                args.append("--video-filter=transform");
                args.append(QString("--transform-type=%1").arg(videoRotate));
            }
        }
    }

    camFilePath = settings.value("piece-path").toString();

    vlcInstance = new VlcInstance(args);
    if ( !vlcInstance )
        SHOW_ERROR(tr("Невожможно инициализировать LibVLC"));

    vlcWidget = new VlcVideoWidget();
    vlcPlayer = new VlcMediaPlayer(vlcInstance);
    vlcVolume = new VlcVolumeSlider();
    vlcSeek   = new VlcSeekWidget();

    //setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setFrameStyle(QFrame::Box);
    setLineWidth(PLAYER_BORDER_LINE_WIDTH);
    setStyleSheet(QString("color:%1").arg(PLAYER_BORDER_COLOR));

    vlcWidget->setMediaPlayer(vlcPlayer);
    vlcPlayer->setVideoWidget(vlcWidget);
    vlcVolume->setMediaPlayer(vlcPlayer);
    //vlcSeek->setMediaPlayer(vlcPlayer);

    QPalette plt = palette();
    plt.setColor( QPalette::Window, Qt::black );
    //vlcWidget->
    setPalette( plt );

    vlcWidget->setAutoFillBackground( true );
    setAutoFillBackground( true );
    //vlcWidget->setContentsMargins(1,1,1,1);


    info = new ClickableLabel(this);
    //info->setStyleSheet("border-image:url(:/images/no_videos.png) ; border-width:3px");
    info->setPixmap(getPixmap("noVIDEO", 95, 95));
    info->setAlignment(Qt::AlignCenter);
    connect(info, SIGNAL(doubleClick()), this, SLOT(videofullscreen()));




    //info->setStyleSheet("background-image:url(:/images/no_videos.png) ; background-origin: margin; background-position: center;  background-repeat: none;  border-width:3px");

    layout->addWidget(info, 1);

    layout->addWidget(vlcWidget, 1);
    //layout->addWidget(vlcSeek, 0);
    layout->addWidget(vlcVolume, 0);

    vlcVolume->setVolume(0);
    vlcVolume->setVisible(false);
    //vlcSeek->setVisible(false);

    makeToolBar();
    layout->addWidget(toolBar);


    connect(vlcPlayer, SIGNAL(timeChanged(int)), this, SLOT(updateCurrentTime(int)));
    connect(vlcPlayer, SIGNAL(end()), this, SLOT(endVideoPiece()));
    connect(vlcPlayer, SIGNAL(lengthChanged(int)), this, SLOT(updateFullTime(int)));
    connect(vlcWidget, SIGNAL(fullscreen()), this, SLOT(videofullscreen()));
    connect(vlcWidget, SIGNAL(leftClick(QPoint)), this, SLOT(videoLeftClick(QPoint)));

    setShowVideo(false, crntTime);
}

int Player::getAdditionWidth()
{
    int h = 0;
    int l = frameWidth();
    h +=l*2;
    return h;
}

int Player::getAdditionHeight()
{
    int h = 0;
    int l = frameWidth();
    QSize s;
    s = toolBar->minimumSizeHint();
    h += s.height(); //24!!
    h +=l*2;
    return h;
};

Player::~Player()
{

}
void Player::videoLeftClick(const QPoint)
{
    emit activated();
}

void Player::open(const QString &url, const int &time)
{
    VlcMedia *vlcMedia;

    if ( !QFileInfo(url).exists() ) {
        SHOW_WARNING(QString("Файл \"%1\" не существует").arg(url));
        return;
    }
    vlcMedia = new VlcMedia(url, true, vlcInstance);
    vlcPlayer->open(vlcMedia, time, m_paused);

}
void Player::setTime(const QDateTime &time)
{

    if ( time >= crntTime.addMSecs(-DEFAULT_VIDEO_EXACTLY_MSEC)  &&
         time <= crntTime.addMSecs(DEFAULT_VIDEO_EXACTLY_MSEC) )
        return;

    if ( crntPiece.isLoading() )
        return;

    if ( crntPiece.within(time) ) {
        crntTime = time;
        vlcPlayer->setTime(crntPiece.secsTo(crntTime)*1000);
    }
    else {
        bool video;
        crntTime = time;
        video =  getVideoPiece(time);
        setShowVideo(video, time);
        if ( video )

            open(
                        QFileInfo(camFilePath, crntPiece.getFilename()).absoluteFilePath(),
                 crntPiece.secsTo(crntTime)*1000);
    }
    updateCurrentTime();
}
void Player::updateTime(const QDateTime &time)
{
    if ( m_paused )
        return;

    if ( !m_video ) {
        setTime(time);
    }
    else {
        int seconds = time.secsTo(crntPiece.addMSecs(vlcPlayer->time()));
        if ( seconds < -DEFAULT_VIDEO_EXACTLY_SEC ) {
            vlcPlayer->setTime(vlcPlayer->time()-seconds*1000);
        }
    }
}

void Player::endVideoPiece()
{
    bool video;

    crntPiece.setEnd(crntPiece.addMSecs(vlcPlayer->lenght()));
    lastPiece.setPiece(crntPiece);


    video = getVideoPiece(crntPiece.getEnd(), true);
    setShowVideo(video, lastPiece.getEnd());
    if ( video )
        open(QFileInfo(camFilePath, crntPiece.getFilename()).absoluteFilePath());
    updateCurrentTime();
}

void Player::updateVideoInfo()
{
    videoRotate = DEFAULT_VIDEO_ROTATE;
    videoWidth = DEFAULT_VIDEO_WIDTH;
    videoHeight = DEFAULT_VIDEO_HEIGHT;

    QSqlQuery q = QSqlQuery(QSqlDatabase::database(connectionName));
    QString text;

    text = QString("SELECT rotate,width,height FROM cam_video_info"
                   " WHERE cam = '%1' and time_start <= '%2'"
                   " ORDER BY time_start DESC LIMIT 1").arg(camName).arg(crntTime.toString("yyyy-MM-dd hh:mm:ss"));

    if ( !q.exec(text) ) {
        QSqlError err = q.lastError();
        SHOW_ERROR(tr("Ошибка запроса: %1").arg(err.text()));
        return;
    }
    if ( q.next() ) {
        videoRotate = q.value(0).toInt();
        videoWidth = q.value(1).toInt();
        videoHeight = q.value(2).toInt();
    }
}


void Player::setVolume(const int &volume)
{
    vlcVolume->setVolume(volume);
    int icon = volume ? 3 : 0;
    if ( icon && volume < 66 )
         icon = volume < 33 ? 1 : 2;
    volumeButton->setIcon(volumeIcons[icon]);
    volumeButton->setToolTip(volume ? "Приглушить": "Включить звук");
}

bool Player::getVideoPiece(const QDateTime &time, bool next)
{
    QSqlQuery q = QSqlQuery(QSqlDatabase::database(connectionName));
    QString text;

    text = QString("SELECT time_start,time_end,filename,correction FROM pieces"
                   " WHERE cam = '%1' ").arg(camName);

    if ( next )
        text.append(QString(" AND time_start >= '%1'"
                            " ORDER BY time_start LIMIT 1").arg(time.toString("yyyy-MM-dd hh:mm:ss")));
    else
        text.append(QString(" AND time_start <= '%1'"
                            " ORDER BY time_start DESC LIMIT 1").arg(time.toString("yyyy-MM-dd hh:mm:ss")));

    if ( !q.exec(text) ) {
        QSqlError err = q.lastError();
        SHOW_ERROR(tr("Ошибка запроса: %1").arg(err.text()));
        return false;
    }
    QString str;

    if ( q.next() ) {
        crntPiece.setPiece(q.value(0).toDateTime(), q.value(1).toDateTime(),
                    q.value(2).toString(), q.value(3).toDouble());

        if ( lastPiece.getStart() == crntPiece.getStart() && crntPiece.getEnd().isNull() )
            crntPiece.setEnd(lastPiece.getEnd());

        if ( crntPiece.within(time) )
            return true;
        if ( frontState && time.secsTo(crntPiece.getStart()) < DEFAULT_VIDEO_EXACTLY_START_SEC1 )
            return true;
        crntPiece.setEmpty();
    }
    text = QString("SELECT time_start,time_end,filename,correction FROM pieces"
                   " WHERE cam = '%1' ").arg(camName);
    text.append(QString(" AND time_start >= '%1'"
                        " ORDER BY time_start LIMIT 1").arg(time.toString("yyyy-MM-dd hh:mm:ss")));
    if ( !q.exec(text) ) {
        QSqlError err = q.lastError();
        SHOW_ERROR(tr("Ошибка запроса: %1").arg(err.text()));
        return false;
    }
    if ( q.next() ) {
        nearestPiece.setPiece(q.value(0).toDateTime(), q.value(1).toDateTime(),
                    q.value(2).toString(), q.value(3).toDouble());
        return false;
    }

    text = QString("SELECT time_start,time_end,filename,correction FROM pieces"
                   " WHERE cam = '%1' ").arg(camName);
    text.append(QString(" AND time_start < '%1'"
                        " ORDER BY time_start DESC LIMIT 1").arg(time.toString("yyyy-MM-dd hh:mm:ss")));
    if ( !q.exec(text) ) {
        QSqlError err = q.lastError();
        SHOW_ERROR(tr("Ошибка запроса: %1").arg(err.text()));
        return false;
    }
    if ( q.next() ) {
        nearestPiece.setPiece(q.value(0).toDateTime(), q.value(1).toDateTime(),
                    q.value(2).toString(), q.value(3).toDouble());
        return false;
    }
    nearestPiece.setEmpty();
    return false;
}
void Player::setShowVideo(bool video, const QDateTime &time)
{
    if ( !video ) {
        m_evailable = ( lastPiece.getEnd().secsTo(time) < DEFAULT_VIDEO_SLEEP_SEC ) ||
                      ( lastPiece.getStart().secsTo(time) < -DEFAULT_VIDEO_SLEEP_SEC );
    }
    m_video = video;
    info->setVisible(!video && !m_evailable);
    //info->setVisible(false);
    vlcWidget->setVisible(video || m_evailable);
    if ( !video ) {
        //!!vlcPlayer->pause();
        if ( !m_evailable && vlcPlayer->state() == Vlc::Playing )
            vlcPlayer->stop();
        emit noVideoAvailable();
    }
    playButton->setEnabled(video);
    volumeButton->setEnabled(video);
    photoButton->setEnabled(video);
    //!!sizeButton->setEnabled(video);
}
void Player::updateCurrentTime(int)
{
    crntTime = crntPiece.addMSecs(vlcPlayer->time());

    QString s;
    QString a;
    a = QString(" (%1x%2) (%3x%4)").arg(vlcWidget->width()).arg(vlcWidget->height()).arg(width()).arg(height());
    //if ( vlcWidget->width()/(1.*vlcWidget->height()) > videoWidth/(1.*videoHeight) )
      //  a = "+";
    //else
      //  a = "-";

   // s = crntTime.toString("yyyy-MM-dd hh:mm:ss (%1) %2").arg(vlcPlayer->lenght() - vlcPlayer->time()).arg(a);
    if ( m_video )
        //s = QString(" [%1]").arg(camHumanName)
        s = crntTime.toString(" [%1] hh:mm:ss").arg(camHumanName);
    else {
        if ( nearestPiece.getStart().isNull() )
            s = QString(" [%1] нет видео").arg(camHumanName);
        else
            s = nearestPiece.getStart().toString(" [%1] dd-MM-yyyy hh:mm:ss ").arg(camHumanName);
    }
    timeLabel->setText(s);
    //timeLabel->setText(a);
    emit timeChanged(crntTime);
}
QDateTime Player::time()
{
    return crntPiece.addMSecs(vlcPlayer->time());
}

void Player::updateFullTime(int time)
{
    crntPiece.setEnd(crntPiece.addMSecs(time));
}
void Player::stop()
{
    vlcPlayer->stop();
}
void Player::setPaused(bool pause)
{
    if ( m_paused == pause )
        return;

    m_paused = pause;
    playButton->setIcon(playIcons[pause]);
    playButton->setToolTip(pause ? "Возобновить" : "Пауза");
    if ( pause )
        vlcPlayer->pause();
    else
        vlcPlayer->play(false);

}
/*
void Player::setStopend(bool stop)
{
    if ( m_stoped == stop )
        return;
    m_stoped = stop;
    playButton->setIcon(playIcons[m_stoped]);
    playButton->setToolTip(m_stoped ? "Возобновить" : "Пауза");
    if ( m_stoped ) {
        if ( !m_paused )
            vlcPlayer->pause();
    }
    else
        vlcPlayer->play(false);
}
*/
void Player::setSpeed(double value)
{
    vlcPlayer->setRate(value);
}

void Player::videofullscreen()
{
    emit fullscreen();
}
void Player::setFrontState(bool front)
{
    frontState = front;
    setAudioVolume(m_audioVolume);
    if ( front )
        setStyleSheet(QString("color:%1").arg(PLAYER_BORDER_ACTIVE_COLOR));
    else
        setStyleSheet(QString("color:%1").arg(PLAYER_BORDER_COLOR));
}
void Player::setSizeInfo(bool full)
{
    sizeButton->setIcon(sizeIcons[full]);
    sizeButton->setToolTip(full ? "Обычный размер" : "На весь экран");
}

void Player::setAudioVolume(int volume)
{
    m_audioVolume = volume;
    if ( (frontState || m_unmuted) && !m_muted )
        setVolume(volume);
    else
        setVolume(0);
}
void Player::sizeClicked()
{
    emit fullscreen();
}
void Player::volumeClicked()
{
    if ( frontState )
        m_muted = !m_muted;
    else
        m_unmuted = !m_unmuted;

    setAudioVolume(m_audioVolume);
}
void Player::playClicked()
{
    emit togglePause();

    /*if ( frontState )
        emit togglePause(!m_paused);
    else {
        if ( m_paused ) {
            m_stoped = false;
            emit togglePause(false);
        }
        else {
            m_stoped = !m_stoped;
            playButton->setIcon(playIcons[m_stoped || m_paused]);
            playButton->setToolTip((m_stoped || m_paused) ? "Возобновить" : "Пауза");
            if ( m_stoped )
                vlcPlayer->pause();
            else
                vlcPlayer->play(false);
        }
    }*/
}
void Player::photoClicked()
{
    QString path;
    QString name;
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    QString defaultPath;
    defaultPath = QDir(QDesktopServices::storageLocation ( QDesktopServices::DocumentsLocation )).filePath("Snapshot");

    path = settings.value("snapshot-path", defaultPath).toString();
    if ( !QDir(path).exists() ) {
        if ( !QDir().mkdir(path) )
            SHOW_ERROR(QString("Невозможно создать директорию  \"%1\"").arg(path))
            return;
    }
    name = crntPiece.addMSecs(vlcPlayer->time()).toString("%1-yyyMMddhhmmss.png").arg(camName);

    vlcPlayer->takeSnapshot(QFileInfo(path, name).absoluteFilePath(), videoWidth, videoHeight);
}

void Player::makeToolBar()
{
    toolBar = new QToolBar();
    toolBar->setIconSize(QSize(16,16));
    toolBar->setStyleSheet(QString("QToolBar{background:%1;border:0;}").arg(PLAYER_TOOLBAR_COLOR));


    playIcons[0] = QIcon(getPixmap("mediaPAUSE", 10,10));
    playIcons[1] = QIcon(getPixmap("mediaPLAY", 10,10));
    QToolButton *b;
    b = new QToolButton();
    b->setIcon(playIcons[0]);
    b->setProperty("noArrow", true);
    b->setToolTip("Пауза");
    playButton = b;
    connect(b, SIGNAL(clicked()), this, SLOT(playClicked()));
    toolBar->addWidget(b);


    b = new QToolButton();
    b->setIcon(volumeIcons[0]);
    b->setProperty("noArrow", true);
    b->setToolTip("Включить звук");
    volumeButton = b;
    connect(b, SIGNAL(clicked()), this, SLOT(volumeClicked()));
    toolBar->addWidget(b);

    b = new QToolButton();
    b->setIcon(QIcon(getPixmap("mediaPHOTO", 12,12)));
    b->setProperty("noArrow", true);
    b->setToolTip("Сделать фотографию");
    photoButton = b;
    connect(b, SIGNAL(clicked()), this, SLOT(photoClicked()));
    toolBar->addWidget(b);

    sizeIcons[0] = QIcon(getPixmap("fullSCREEN", 12,12));
    sizeIcons[1] = QIcon(getPixmap("originalSCREEN", 12,12));
    b = new QToolButton();
    b->setIcon(sizeIcons[0]);
    b->setProperty("noArrow", true);
    b->setToolTip("На весь экран");
    sizeButton = b;
    connect(b, SIGNAL(clicked()), this, SLOT(sizeClicked()));
    toolBar->addWidget(b);

    timeLabel = new QLabel();

    QFontMetrics fm = timeLabel->fontMetrics();
    timeLabel->setMaximumHeight(16);//fm.height()+4);
    timeLabel->setMinimumHeight(16);//fm.height()+4);
    timeLabel->setStyleSheet("color:white");
    toolBar->addWidget(timeLabel);
}
ClickableLabel::ClickableLabel(QWidget *parent):
    QLabel(parent)
{

}
void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClick();
}
