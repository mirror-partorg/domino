#-------------------------------------------------
#
# Project created by QtCreator 2012-09-12T12:01:53
#
#-------------------------------------------------

QT       += core gui sql svg

LIBS     += -lvlc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cctv0
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp \
    logindialog.cpp \
    qsqlconnectiondialog.cpp \
    videogrid.cpp \
    vlc/Video.cpp \
    vlc/MetaManager.cpp \
    vlc/MediaPlayer.cpp \
    vlc/MediaListPlayer.cpp \
    vlc/MediaList.cpp \
    vlc/Media.cpp \
    vlc/Instance.cpp \
    vlc/Error.cpp \
    vlc/Enums.cpp \
    vlc/Common.cpp \
    vlc/Audio.cpp \
    vlc/VolumeSlider.cpp \
    vlc/VideoWidget.cpp \
    vlc/VideoControl.cpp \
    vlc/SeekWidget.cpp \
    vlc/AudioControl.cpp \
    widgets/VolumeDial.cpp \
    widgets/SpeedDial.cpp \
    widgets/TimeControl.cpp \
    widgets/datetimedialog.cpp \
    widgets/timeeditview.cpp \
    widgets/SeekSlider.cpp \
    widgets/TimeTooltip.cpp \
    widgets/TimeLabel.cpp \
    utils/utils.cpp \
    paramdialog.cpp


HEADERS  += mainwindow.h \
    player.h \
    config.h \
    logindialog.h \
    qsqlconnectiondialog.h \
    log.h \
    videogrid.h \ 
    vlc/Video.h \
    vlc/MetaManager.h \
    vlc/MediaPlayer.h \
    vlc/MediaListPlayer.h \
    vlc/MediaList.h \
    vlc/Media.h \
    vlc/Instance.h \
    vlc/Error.h \
    vlc/Enums.h \
    vlc/Common.h \
    vlc/Audio.h \
    vlc/VolumeSlider.h \
    vlc/VideoWidget.h \
    vlc/VideoControl.h \
    vlc/SeekWidget.h \
    vlc/AudioControl.h \
    widgets/VolumeDial.h \
    widgets/SpeedDial.h \
    widgets/TimeControl.h \
    widgets/datetimedialog.h \
    widgets/timeeditview.h \
    widgets/SeekSlider.h \
    widgets/TimeTooltip.h \
    widgets/TimeLabel.h \
    utils/utils.h \
    paramdialog.h
    

FORMS    += \
    logindialog.ui \
    qsqlconnectiondialog.ui \
    widgets/datetimedialog.ui \
    paramdialog.ui

RESOURCES += \
    cctv.qrc

RC_FILE = cctv.rc
