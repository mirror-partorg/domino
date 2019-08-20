#include "TimeLabel.h"
#include <QPainter>
#include <QTime>
#include <QDate>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QDebug>

TimeLabel::TimeLabel(QWidget *parent) :
    QWidget(parent),
    m_borderSize(2),
    m_sizeHint(0,0),
    m_textHeight(0)
{
    m_time = QDateTime::currentDateTime().addSecs(-1200);
    m_anim.step = 0;
    m_anim.timer = 0;
    setMinimumSize(QSize(25*3, 25));
    resize(25,25);
    //m_sizeHint = getSizeHint();
}

void TimeLabel::resizeEvent(QResizeEvent *)
{
    if ( !m_textHeight ||  m_textHeight > height() - 4 ) {
        m_borderSize = 2;
    }
    else {
        m_borderSize = 2;//!!(height() - m_textHeight)/2;
    }
    m_sizeHint = getSizeHint();
    setMinimumWidth(m_sizeHint.height());
}

QSize TimeLabel::getSizeHint() const
{
    QSize sizeHint;

    QFont bigFont = font();
    int textHeight;

    textHeight = (height() - m_borderSize*2) * 2/3. - 2;// m_borderSize;

    bigFont.setPixelSize(textHeight);

    QFontMetrics fm(bigFont);
    fm = QFontMetrics(bigFont);

    sizeHint.setHeight(height());
    sizeHint.setWidth(fm.width(QString("99:99:99"))+m_borderSize*2);

    return sizeHint;

}

QSize TimeLabel::sizeHint() const
{
    if ( m_sizeHint.height() != height() )
        return getSizeHint();
    else
        return m_sizeHint;
}

void TimeLabel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    //p.fillRect(rect(), QColor("blue"));

    QString timeStr = m_time.toString("hh:mm:ss");
    QString dateStr = m_time.toString("dd MMMM yyyy");

    QRectF timeRect;
    QRectF dateRect;
    QRect  contentsRect;
    contentsRect = rect();
    contentsRect.adjust( m_borderSize,  m_borderSize, -m_borderSize, -m_borderSize);

    timeRect = contentsRect;
    dateRect = contentsRect;

    timeRect.setHeight((contentsRect.height() * 2/3. ) - 2); // m_borderSize
    dateRect.moveTop((contentsRect.height() * 2/3. ) + 2 + timeRect.top()); // m_borderSize
    dateRect.setHeight(contentsRect.height() * 1/3.);

    QFont f = p.font();

    QFont bigFont = p.font();
    bigFont.setPixelSize(timeRect.height());

    p.setFont(bigFont);
    QPen timePen = p.pen();
    QColor color = QColor("black");
    if ( !m_anim.step )
        color.setAlphaF(0.7);
    timePen.setColor(color);
    p.setPen(timePen);
    p.drawText(timeRect, Qt::TextSingleLine | Qt::AlignHCenter, timeStr);

    QFont smallFont = p.font();
    smallFont.setPixelSize(dateRect.height());

    QFontMetrics fm = QFontMetrics(smallFont);
    const int tempWidth = fm.width(dateStr);
    if ( tempWidth > contentsRect.width() )
        smallFont.setPixelSize((contentsRect.width() * smallFont.pixelSize())/tempWidth);

    p.setFont(smallFont);
    QPen datePen = p.pen();
    //QColor dateColor = QColor("grey");//m_plainClockColor;
    if ( !m_anim.step )
        color.setAlphaF(0.5);
    else
        color.setAlphaF(0.7);
    datePen.setColor(color);
    p.setPen(datePen);
    p.drawText(dateRect, Qt::TextSingleLine | Qt::AlignHCenter, dateStr);

    p.setFont(f);

    //p.save();
}
void TimeLabel::enterEvent( QEvent * )
{
    startFade();
}
void TimeLabel::leaveEvent( QEvent * )
{
    startFade();
}
void TimeLabel::startFade()
{
    if ( m_anim.timer )
        killTimer( m_anim.timer );
    m_anim.timer = startTimer( 40 );
}

void TimeLabel::stopFade()
{
    killTimer( m_anim.timer );
    m_anim.timer = 0;
    if ( m_anim.step < 0 )
        m_anim.step = 0;
    else if ( m_anim.step > 6 )
        m_anim.step = 6;
}

void TimeLabel::timerEvent( QTimerEvent *te )
{
    if ( te->timerId() != m_anim.timer )
        return;
    if ( underMouse() ) // fade in
    {
        m_anim.step += 2;
        if ( m_anim.step > 5 )
            stopFade();
    }
    else // fade out
    {
        --m_anim.step;
        if ( m_anim.step < 1 )
            stopFade();
    }
//    updateSliderGradient();
    repaint();
}
void TimeLabel::setTime(const QDateTime &time)
{
    m_time = time;
    update();
}
void TimeLabel::mouseMoveEvent( QMouseEvent * )
{
}

void TimeLabel::mousePressEvent( QMouseEvent *me )
{
    if ( me->button() != Qt::LeftButton )
        return;
    emit clicked();
}
void TimeLabel::setTextHeight(int h)
{
    if ( m_textHeight == h )
        return;
    m_textHeight = h;
    if ( m_textHeight > height() - 4 ) {
        m_borderSize = 2;
    }
    else {
        m_borderSize = (height() - m_textHeight)/2;
    }
    m_sizeHint = getSizeHint();
    update();
}

void TimeLabel::mouseReleaseEvent( QMouseEvent *me )
{
    if ( me->button() != Qt::LeftButton )
        return;
}
