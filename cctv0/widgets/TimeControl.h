#ifndef TIME_CONTROL_H
#define TIME_CONTROL_H

#include <QWidget>
#include <QDateTime>
#include "TimeLabel.h"
#include "SeekSlider.h"
#include <QLabel>
#include <QToolButton>


class TimeControl : public QWidget
{
    Q_OBJECT
public:
    explicit TimeControl(QWidget *parent = 0);
    QDateTime time();

signals:
    void dateTimeChanged(QDateTime);
    
public slots:
    void setTime(const QDateTime &time);

private slots:
    void labelClicked();
    void sliderDragged(QDateTime);
    void leftSkipClicked();
    void rightSkipClicked();

private:

    void updateSkip();

    SeekSlider *m_slider;
    TimeLabel  *m_label;

    QDateTime m_time;
    QDateTime m_startTime;

    QToolButton  *m_leftSkip;
    QToolButton  *m_rightSkip;
    QLabel       *m_rightLabel;
    QLabel       *m_leftLabel;

    bool          m_skipLook;

};


#endif // TIME_CONTROL_H
