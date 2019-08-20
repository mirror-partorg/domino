/*****************************************************************************
 * input_slider.cpp : VolumeSlider and SeekSlider
 ****************************************************************************
 * Copyright (C) 2006-2011 the VideoLAN team
 * $Id: ea6d4355a04ebfd2377bca0d3e1bcc4f1a3b1ebb $
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *          Ludovic Fauvet <etix@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include "SeekSlider.h"

#include <QPaintEvent>
#include <QPainter>
#include <QBitmap>
#include <QPainter>
#include <QStyleOptionSlider>
#include <QLinearGradient>
#include <QTimer>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QSize>
#include <QPalette>
#include <QColor>
#include <QPoint>
#include <QPropertyAnimation>
#include <QApplication>

#define MINIMUM 0
#define MAXIMUM 1000
#define CHAPTERSSPOTSIZE 3
#define FADEDURATION 300
#define FADEOUTDELAY 2000

SeekSlider::SeekSlider( Qt::Orientation q, QWidget *_parent, bool _static )
          : QSlider( q, _parent ), b_classic( _static )
{
    isSliding = false;
    f_buffering = 1.0;
    mHandleOpacity = 1.0;

    /* Timer used to fire intermediate updatePos() when sliding */
    seekLimitTimer = new QTimer( this );
    seekLimitTimer->setSingleShot( true );
    setTickPosition(QSlider::TicksBothSides);

    /* Tooltip bubble */
    mTimeTooltip = new TimeTooltip( this );
    mTimeTooltip->setMouseTracking( true );

    /* Properties */
    setRange( MINIMUM, MAXIMUM );
    setSingleStep( 2 );
    setPageStep( 10 );
    setMouseTracking( true );
    setTracking( true );
    setFocusPolicy( Qt::NoFocus );

    /* Init to 0 */
    QDateTime t = QDateTime::currentDateTime();
    setPosition( -1.0, t, 0 );
    //!!secstotimestr( psz_length, 0 );

    animHandle = new QPropertyAnimation( this, "handleOpacity", this );
    animHandle->setDuration( FADEDURATION );
    animHandle->setStartValue( 0.0 );
    animHandle->setEndValue( 1.0 );

    hideHandleTimer = new QTimer( this );
    hideHandleTimer->setSingleShot( true );
    hideHandleTimer->setInterval( FADEOUTDELAY );

    connect( this, SIGNAL(sliderMoved( int )), this, SLOT(startSeekTimer()) );
    connect( seekLimitTimer, SIGNAL(timeout()), this, SLOT(updatePos()) );
    connect( hideHandleTimer, SIGNAL(timeout()), this, SLOT(hideHandle()) );
    mTimeTooltip->installEventFilter( this );
}

SeekSlider::~SeekSlider()
{
}


/***
 * \brief Main public method, superseeding setValue. Disabling the slider when neeeded
 *
 * \param pos Position, between 0 and 1. -1 disables the slider
 * \param time Elapsed time. Unused
 * \param legnth Duration time.
 ***/
void SeekSlider::setPosition( float pos, QDateTime &time, int length )
{
    startTime = time;
    if( pos == -1.0 )
    {
        setEnabled( false );
        isSliding = false;
    }
    else
        setEnabled( true );

    if( !isSliding )
        setValue( (int)( pos * 1000.0 ) );

    inputLength = length;
}

void SeekSlider::startSeekTimer()
{
    /* Only fire one update, when sliding, every 150ms */
    if( isSliding && !seekLimitTimer->isActive() )
        seekLimitTimer->start( 150 );
}

void SeekSlider::updatePos()
{
    float f_pos = (float)( value() ) / 1000.0;
    QDateTime t;
    t = startTime.addSecs(f_pos*inputLength);
    emit sliderDragged( t ); /* Send new position to VLC's core */
}

void SeekSlider::updateBuffering( float f_buffering_ )
{
    f_buffering = f_buffering_;
    repaint();
}

void SeekSlider::mouseReleaseEvent( QMouseEvent *event )
{
    event->accept();
    isSliding = false;
    seekLimitTimer->stop(); /* We're not sliding anymore: only last seek on release */
    if ( isJumping )
    {
        isJumping = false;
        return;
    }
    QSlider::mouseReleaseEvent( event );
    updatePos();
}

void SeekSlider::mousePressEvent( QMouseEvent* event )
{
    /* Right-click */
    if( event->button() != Qt::LeftButton &&
        event->button() != Qt::MidButton )
    {
        QSlider::mousePressEvent( event );
        return;
    }

    isJumping = false;

    isSliding = true ;
    setValue( QStyle::sliderValueFromPosition( MINIMUM, MAXIMUM, event->x(), width(), false ) );
    event->accept();
}
char *secstotimestr( char *psz_buffer, int32_t i_seconds )
{
    if( i_seconds < 0 )
    {
        secstotimestr( psz_buffer + 1, -i_seconds );
        *psz_buffer = '-';
        return psz_buffer;
    }

    div_t d;

    d = div( i_seconds, 60 );
    i_seconds = d.rem;
    d = div( d.quot, 60 );

    if( d.quot )
        snprintf( psz_buffer, MSTRTIME_MAX_SIZE, "%u:%02u:%02u",
                 d.quot, d.rem, i_seconds );
    else
        snprintf( psz_buffer, MSTRTIME_MAX_SIZE, "%02u:%02u",
                  d.rem, i_seconds );
    return psz_buffer;
}

void SeekSlider::mouseMoveEvent( QMouseEvent *event )
{
    if( isSliding )
    {
        setValue( QStyle::sliderValueFromPosition( MINIMUM, MAXIMUM, event->x(), width(), false) );
        emit sliderMoved( value() );
    }

    /* Tooltip */
    if ( inputLength > 0 )
    {
        int posX = qMax( rect().left(), qMin( rect().right(), event->x() ) );
        int sec = ( posX * inputLength ) / size().width();

        QDateTime t;
        t = startTime.addSecs(sec);

        mTimeTooltip->setText( t.toString("hh:mm:ss"), "");

        QPoint p( event->globalX() - ( event->x() - posX ) - ( mTimeTooltip->width() / 2 ),
                  QWidget::mapToGlobal( QPoint( 0, 0 ) ).y() - ( mTimeTooltip->height() - 2 ) );
        mTimeTooltip->move( p );
    }
    event->accept();
}

void SeekSlider::wheelEvent( QWheelEvent *event )
{
    /* Don't do anything if we are for somehow reason sliding */
    if( !isSliding )
    {
        setValue( value() + event->delta() / 12 ); /* 12 = 8 * 15 / 10
         Since delta is in 1/8 of ° and mouse have steps of 15 °
         and that our slider is in 0.1% and we want one step to be a 1%
         increment of position */
        QDateTime t;
        t = startTime.addSecs((value()/ 1000.0 )*inputLength );
        emit sliderDragged( t);
    }
    event->accept();
}

void SeekSlider::enterEvent( QEvent * )
{
    /* Cancel the fade-out timer */
    hideHandleTimer->stop();
    /* Only start the fade-in if needed */
    if( animHandle->direction() != QAbstractAnimation::Forward )
    {
        /* If pause is called while not running Qt will complain */
        if( animHandle->state() == QAbstractAnimation::Running )
            animHandle->pause();
        animHandle->setDirection( QAbstractAnimation::Forward );
        animHandle->start();
    }
    /* Don't show the tooltip if the slider is disabled or a menu is open */
    if( isEnabled() && inputLength > 0 && !qApp->activePopupWidget() )
        mTimeTooltip->show();
}

void SeekSlider::leaveEvent( QEvent * )
{
    hideHandleTimer->start();
    /* Hide the tooltip
       - if the mouse leave the slider rect (Note: it can still be
         over the tooltip!)
       - if another window is on the way of the cursor */
    if( !rect().contains( mapFromGlobal( QCursor::pos() ) ) ||
      ( !isActiveWindow() && !mTimeTooltip->isActiveWindow() ) )
    {
        mTimeTooltip->hide();
    }
}

void SeekSlider::hideEvent( QHideEvent * )
{
    mTimeTooltip->hide();
}

bool SeekSlider::eventFilter( QObject *obj, QEvent *event )
{
    if( obj == mTimeTooltip )
    {
        if( event->type() == QEvent::Leave ||
            event->type() == QEvent::MouseMove )
        {
            QMouseEvent *e = static_cast<QMouseEvent*>( event );
            if( !rect().contains( mapFromGlobal( e->globalPos() ) ) )
                mTimeTooltip->hide();
        }
        return false;
    }
    else
        return QSlider::eventFilter( obj, event );
}

QSize SeekSlider::sizeHint() const
{
    return ( orientation() == Qt::Horizontal ) ? QSize( 200, 18 )
                                               : QSize( 18, 200 );
}

QSize SeekSlider::handleSize() const
{
    const int size = ( orientation() == Qt::Horizontal ? height() : width() );
    return QSize( size, size );
}

void SeekSlider::paintEvent( QPaintEvent *event )
{
    if( b_classic )
        return QSlider::paintEvent( event );

    QStyleOptionSlider option;
    initStyleOption( &option );

    /* */
    QPainter painter( this );
    painter.setRenderHints( QPainter::Antialiasing );

    // draw bar
    const int barCorner = 3;
    qreal sliderPos     = -1;
    int range           = MAXIMUM;
    QRect barRect       = rect();

    // adjust positions based on the current orientation
    if ( option.sliderPosition != 0 )
    {
        switch ( orientation() )
        {
            case Qt::Horizontal:
                sliderPos = ( ( (qreal)width() ) / (qreal)range )
                        * (qreal)option.sliderPosition;
                break;
            case Qt::Vertical:
                sliderPos = ( ( (qreal)height() ) / (qreal)range )
                        * (qreal)option.sliderPosition;
                break;
        }
    }

    switch ( orientation() )
    {
        case Qt::Horizontal:
            barRect.setHeight( height() /2 );
            break;
        case Qt::Vertical:
            barRect.setWidth( width() /2 );
            break;
    }

    barRect.moveCenter( rect().center() );

    // set the background color and gradient
    QColor backgroundBase( palette().window().color() );
    QLinearGradient backgroundGradient( 0, 0, 0, height() );
    backgroundGradient.setColorAt( 0.0, backgroundBase.darker( 140 ) );
    backgroundGradient.setColorAt( 1.0, backgroundBase );

    // set the foreground color and gradient
    //!!QColor foregroundBase( 50, 156, 255 );
    QColor foregroundBase("#b3b0ae");
    QLinearGradient foregroundGradient( 0, 0, 0, height() );
    foregroundGradient.setColorAt( 0.0,  foregroundBase );
    foregroundGradient.setColorAt( 1.0,  foregroundBase.darker( 140 ) );

    // draw a slight 3d effect on the bottom
    painter.setPen( QColor( 230, 230, 230 ) );
    painter.setBrush( Qt::NoBrush );
    painter.drawRoundedRect( barRect.adjusted( 0, 2, 0, 0 ), barCorner, barCorner );

    // draw background
    painter.setPen( Qt::NoPen );
    painter.setBrush( backgroundGradient );
    painter.drawRoundedRect( barRect, barCorner, barCorner );

    // adjusted foreground rectangle
    QRect valueRect = barRect.adjusted( 1, 1, -1, 0 );

    switch ( orientation() )
    {
        case Qt::Horizontal:
            valueRect.setWidth( qMin( width(), int( sliderPos ) ) );
            break;
        case Qt::Vertical:
            valueRect.setHeight( qMin( height(), int( sliderPos ) ) );
            valueRect.moveBottom( rect().bottom() );
            break;
    }

    if ( option.sliderPosition > minimum() && option.sliderPosition <= maximum() )
    {
        // draw foreground
        painter.setPen( Qt::NoPen );
        painter.setBrush( foregroundGradient );
        painter.drawRoundedRect( valueRect, barCorner, barCorner );
    }

    // draw buffering overlay
    if ( f_buffering < 1.0 )
    {
        QRect innerRect = barRect.adjusted( 1, 1,
                            barRect.width() * ( -1.0 + f_buffering ) - 1, 0 );
        QColor overlayColor = QColor( "Orange" );
        overlayColor.setAlpha( 128 );
        painter.setBrush( overlayColor );
        painter.drawRoundedRect( innerRect, barCorner, barCorner );
    }

    if ( option.state & QStyle::State_MouseOver || isAnimationRunning() )
    {

        // draw handle
        if ( sliderPos != -1 )
        {
            const int margin = 0;
            QSize hSize = handleSize() - QSize( 6, 6 );
            QPoint pos;

            switch ( orientation() )
            {
                case Qt::Horizontal:
                    pos = QPoint( sliderPos - ( hSize.width() / 2 ), 2 );
                    pos.rx() = qMax( margin, pos.x() );
                    pos.rx() = qMin( width() - hSize.width() - margin, pos.x() );
                    break;
                case Qt::Vertical:
                    pos = QPoint( 2, height() - ( sliderPos + ( hSize.height() / 2 ) ) );
                    pos.ry() = qMax( margin, pos.y() );
                    pos.ry() = qMin( height() - hSize.height() - margin, pos.y() );
                    break;
            }

            QPalette p;
            QPoint shadowPos( pos - QPoint( 2, 2 ) );
            QSize sSize( handleSize() - QSize( 2, 2 ) );

            // prepare the handle's gradient
            QLinearGradient handleGradient( 0, 0, 0, hSize.height() );
            handleGradient.setColorAt( 0.0, p.window().color().lighter( 120 ) );
            handleGradient.setColorAt( 0.9, p.window().color().darker( 120 ) );

            // prepare the handle's shadow gradient
            QColor shadowBase = p.shadow().color();
            if( shadowBase.lightness() > 100 )
                shadowBase = QColor( 60, 60, 60 ); // Palette's shadow is too bright
            QColor shadowDark( shadowBase.darker( 150 ) );
            QColor shadowLight( shadowBase.lighter( 180 ) );
            shadowLight.setAlpha( 50 );

            QRadialGradient shadowGradient( shadowPos.x() + ( sSize.width() / 2 ),
                                            shadowPos.y() + ( sSize.height() / 2 ),
                                            qMax( sSize.width(), sSize.height() ) / 2 );
            shadowGradient.setColorAt( 0.4, shadowDark );
            shadowGradient.setColorAt( 1.0, shadowLight );

            painter.setPen( Qt::NoPen );
            painter.setOpacity( mHandleOpacity );

            // draw the handle's shadow
            painter.setBrush( shadowGradient );
            painter.drawEllipse( shadowPos.x(), shadowPos.y() + 1, sSize.width(), sSize.height() );

            // finally draw the handle
            painter.setBrush( handleGradient );
            painter.drawEllipse( pos.x(), pos.y(), hSize.width(), hSize.height() );
        }
    }
}

qreal SeekSlider::handleOpacity() const
{
    return mHandleOpacity;
}

void SeekSlider::setHandleOpacity(qreal opacity)
{
    mHandleOpacity = opacity;
    /* Request a new paintevent */
    update();
}

void SeekSlider::hideHandle()
{
    /* If pause is called while not running Qt will complain */
    if( animHandle->state() == QAbstractAnimation::Running )
        animHandle->pause();
    /* Play the animation backward */
    animHandle->setDirection( QAbstractAnimation::Backward );
    animHandle->start();
}

bool SeekSlider::isAnimationRunning() const
{
    return animHandle->state() == QAbstractAnimation::Running
            || hideHandleTimer->isActive();
}

