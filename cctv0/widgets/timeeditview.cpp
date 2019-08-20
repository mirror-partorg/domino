#include "timeeditview.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include "utils/utils.h"

TimeEditView::TimeEditView(QWidget *parent) :
    QWidget(parent),
    m_time(23,59)
{
    QGridLayout *grid = new QGridLayout();
    setLayout(grid);
    QPushButton *b;
    grid->addWidget(new QWidget(), 0, 0);
    grid->addWidget(new QWidget(), 4, 4);

    b = new QPushButton(QIcon(getPixmap("goUP", 16,10)), "");
    b->setFlat(true);
    grid->addWidget(b, 1, 1);
    buttons.append(b);
    connect(b, SIGNAL(clicked()), this, SLOT(hourButtonUpClicked()));

    m_hourLabel = new QLabel("23");
    m_hourLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    grid->addWidget(m_hourLabel, 2, 1, Qt::AlignHCenter);
    b = new QPushButton(QIcon(getPixmap("goDOWN", 16,10)),"");
    b->setFlat(true);
    grid->addWidget(b, 3, 1);
    buttons.append(b);
    connect(b, SIGNAL(clicked()), this, SLOT(hourButtonDownClicked()));

    m_separatorLabel = new QLabel(":");
    m_separatorLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    grid->addWidget(m_separatorLabel, 1, 2, 3, 1);//, Qt::AlignHCenter);

    b = new QPushButton(QIcon(getPixmap("goUP", 16,10)), "");
    b->setFlat(true);
    grid->addWidget(b, 1, 3, 1, 1);
    buttons.append(b);
    connect(b, SIGNAL(clicked()), this, SLOT(minuteButtonUpClicked()));


    m_minuteLabel = new QLabel("59");
    m_minuteLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    grid->addWidget(m_minuteLabel, 2, 3, Qt::AlignHCenter);
    b = new QPushButton(QIcon(getPixmap("goDOWN", 16,10)),"");
    b->setFlat(true);
    grid->addWidget(b, 3, 3);

    buttons.append(b);
    connect(b, SIGNAL(clicked()), this, SLOT(minuteButtonDownClicked()));

}
void TimeEditView::setFont(const QFont &font)
{
    QWidget::setFont(font);
    int height;
    height = font.pixelSize();
    QFontMetrics fm(font);
    if ( height == -1 )
        height = fm.height();
    QSize s(height/2,height/2);
    foreach ( QPushButton *b, buttons)
        b->setIconSize(s);
}

/*
void TimeEditView::setFontHeight(int height)
{
    QFont f = font();
    f.setPixelSize(height);
    m_hourLabel->setFont(f);
    m_separatorLabel->setFont(f);
    m_minuteLabel->setFont(f);
    QSize s(height/2,height/2);
    foreach ( QPushButton *b, buttons)
        b->setIconSize(s);
}
*/
void TimeEditView::setTime(const QTime &time1)
{
    m_time = time1;
    m_hourLabel->setText(m_time.toString("hh"));
    m_minuteLabel->setText(m_time.toString("mm"));
    emit timeChanged(m_time);
}

void TimeEditView::hourButtonUpClicked( bool )
{
    int hour;
    hour = m_time.hour() + 1;
    if ( hour > 23 )
        hour = 0;
    setTime(QTime(hour, m_time.minute(), 0));
}
void TimeEditView::hourButtonDownClicked( bool )
{
    int hour;
    hour = m_time.hour() - 1;
    if ( hour < 0 )
        hour = 23;
    setTime(QTime(hour, m_time.minute(), 0));
}
void TimeEditView::minuteButtonUpClicked( bool )
{
    int minute;
    int hour;
    hour = m_time.hour();
    minute = m_time.minute() + 1;
    if ( minute > 59 ) {
        hour++;
        minute = 0;
    }
    if ( hour > 23 )
        hour = 0;
    setTime(QTime(hour, minute, 0));

}
void TimeEditView::minuteButtonDownClicked( bool )
{
    int minute;
    int hour;
    hour = m_time.hour();
    minute = m_time.minute() - 1;
    if ( minute < 0 ) {
        hour--;
        minute = 59;
    }
    if ( hour < 0 )
        hour = 23;
    setTime(QTime(hour, minute, 0));
}
