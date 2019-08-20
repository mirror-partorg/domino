#include "TimeControl.h"
#include <QPainter>
#include <QTime>
#include <QDate>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QSvgRenderer>
#include "datetimedialog.h"
#include "config.h"

static QPixmap getPixmap(const QString name, int width, int height);


TimeControl::TimeControl(QWidget *parent) :
    QWidget(parent),
    m_skipLook(true)
{
    m_time = QDateTime::currentDateTime().addSecs(-1200);
    QHBoxLayout *layout;

    layout = new QHBoxLayout();
    setLayout(layout);
    //layout->setSpacing(2);

    m_leftLabel = new QLabel();
    m_leftSkip = new QToolButton();

    m_rightSkip = new QToolButton();
    m_rightLabel = new QLabel();



    m_leftSkip->setAutoRaise(true);
    m_leftSkip->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_leftSkip->setIcon(QIcon(getPixmap("tiny_skip_left", 10,14)));
    m_leftSkip->setToolTip("Передвинуть на полчаса назад");
    //m_leftSkip->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    m_rightSkip->setAutoRaise(true);
    m_rightSkip->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_rightSkip->setIcon(QIcon(getPixmap("tiny_skip_right", 10,14)));
    m_rightSkip->setToolTip("Передвинуть на полчаса вперед");

    m_slider = new SeekSlider(Qt::Horizontal);
    m_label = new TimeLabel();
    //!!m_label->setTextHeight();


    layout->addWidget(m_leftSkip,0);
    layout->addWidget(m_leftLabel,0);

    layout->addWidget(m_slider,1);

    layout->addWidget(m_rightLabel,0);
    layout->addWidget(m_rightSkip,0);
    layout->addWidget(m_label,0);
    m_label->setTextHeight(30);

    connect(m_label, SIGNAL(clicked()), this, SLOT(labelClicked()));
    connect(m_slider, SIGNAL(sliderDragged(QDateTime)), this, SLOT(sliderDragged(QDateTime)));
    connect(m_leftSkip, SIGNAL(clicked()), this, SLOT(leftSkipClicked()));
    connect(m_rightSkip, SIGNAL(clicked()), this, SLOT(rightSkipClicked()));
}
static QPixmap getPixmap(const QString name, int width, int height)
{
    QSvgRenderer svgRenderer(QString(":/images/theme.svg"));
    QPixmap pixmap(width, height);
    pixmap.fill( Qt::transparent );
    QPainter painter(&pixmap);
    svgRenderer.render(&painter, name, QRect(0, 0, width,height));
    return pixmap;

}

QDateTime getLeftTime(const QDateTime &t)
{
    QDate date;
    QTime time;
    int hour;
    int minute;

    date = t.date();
    time = t.time();

    hour = time.hour();
    minute = time.minute();

    if ( minute < 15 ) {
        minute = 30;
        hour--;
    }
    else {
        if ( minute < 45 ) {
            minute = 0;
        }
        else {
            minute = 30;
        }
    }

    if ( hour < 0 ) {
        hour = 23;
        date = date.addDays(-1);
    }
    return QDateTime(date, QTime(hour, minute, 0));


}

void TimeControl::setTime(const QDateTime &time)
{
    m_time = time;
    m_label->setTime(m_time);
    //QDateTime st;

    if ( m_skipLook &&
         ( m_startTime.secsTo(time) < 0 ||
           m_startTime.secsTo(time) > SEEK_SLIDER_LENTH ) ) {
         m_startTime = getLeftTime(time);
         m_leftLabel->setText(m_startTime.toString("hh:mm"));
         m_rightLabel->setText(m_startTime.addSecs(SEEK_SLIDER_LENTH).toString("hh:mm"));
    }
    updateSkip();

    m_slider->setPosition(m_startTime.secsTo(m_time)/(SEEK_SLIDER_LENTH*1.), m_startTime, SEEK_SLIDER_LENTH);

    update();
}
void TimeControl::labelClicked()
{
    DateTimeDialog dialog(this);
    dialog.setDateTime(m_time);
    if ( dialog.exec() == QDialog::Accepted ) {
        setTime(dialog.dateTime());
        m_skipLook = true;
        emit dateTimeChanged(m_time);
    }
}
void TimeControl::sliderDragged(QDateTime time)
{
    m_time = time;
    m_label->setTime(m_time);
    updateSkip();
    emit dateTimeChanged(m_time);
}
void TimeControl::updateSkip()
{
    /*
    if ( m_startTime.secsTo(m_time) < SEEK_SLIDER_LENTH / 2 ) {
        m_leftSkip->setEnabled(true);
        m_rightSkip->setEnabled(false);
    }
    else {
        m_leftSkip->setEnabled(false);
        m_rightSkip->setEnabled(true);
    }
    */
}

void TimeControl::leftSkipClicked()
{
    m_startTime = m_startTime.addSecs(-30*60);
    m_leftLabel->setText(m_startTime.toString("hh:mm"));
    m_rightLabel->setText(m_startTime.addSecs(SEEK_SLIDER_LENTH).toString("hh:mm"));
    m_slider->setPosition(m_startTime.secsTo(m_time)/(SEEK_SLIDER_LENTH*1.), m_startTime, SEEK_SLIDER_LENTH);
    m_skipLook = (m_startTime.secsTo(m_time) <= SEEK_SLIDER_LENTH) &&
                 (m_startTime.secsTo(m_time) >= 0 );
    updateSkip();
}
void TimeControl::rightSkipClicked()
{
    m_startTime = m_startTime.addSecs(30*60);
    m_leftLabel->setText(m_startTime.toString("hh:mm"));
    m_rightLabel->setText(m_startTime.addSecs(SEEK_SLIDER_LENTH).toString("hh:mm"));
    m_slider->setPosition(m_startTime.secsTo(m_time)/(SEEK_SLIDER_LENTH*1.), m_startTime, SEEK_SLIDER_LENTH);
    m_skipLook = (m_startTime.secsTo(m_time) <= SEEK_SLIDER_LENTH) &&
                 (m_startTime.secsTo(m_time) >= 0 );
    updateSkip();
}
QDateTime TimeControl::time()
{
    return m_label->time();
}
