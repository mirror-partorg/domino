/****************************************************************************************
* Copyright (c) 2009 Thomas Luebking <thomas.luebking@web.de>                          *
*                                                                                      *
* This program is free software; you can redistribute it and/or modify it under        *
* the terms of the GNU General Public License as published by the Free Software        *
* Foundation; either version 2 of the License, or (at your option) any later           *
* version.                                                                             *
*                                                                                      *
* This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
* PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
*                                                                                      *
* You should have received a copy of the GNU General Public License along with         *
* this program.  If not, see <http://www.gnu.org/licenses/>.                           *
****************************************************************************************/

#include "VolumeDial.h"

#include <QConicalGradient>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QToolBar>
#include <QToolTip>
#include <QSvgRenderer>
#include "utils/utils.h"

#include <cmath>

VolumeDial::VolumeDial( QWidget *parent ) : QDial( parent )
    , m_isClick( false )
    , m_isDown( false )
    , m_muted( false )
  , m_borderWidth(2)
{
    m_anim.step = 0;
    m_anim.timer = 0;
    setMouseTracking( true );
    setMinimumSize(24,24);
    connect ( this, SIGNAL( valueChanged(int) ), SLOT( valueChangedSlot(int) ) );
}

void VolumeDial::addWheelProxies( QList<QWidget*> proxies )
{
    foreach ( QWidget *proxy, proxies )
    {
        if ( !m_wheelProxies.contains( proxy ) )
        {
            proxy->installEventFilter( this );
            connect ( proxy, SIGNAL( destroyed(QObject*) ), this, SLOT( removeWheelProxy(QObject*) ) );
            m_wheelProxies << proxy;
        }
    }
}

void VolumeDial::enterEvent( QEvent * )
{
    startFade();
}

// NOTICE: we intercept wheelEvents for ourself to prevent the tooltip hiding on them,
// see ::wheelEvent()
// this is _NOT_ redundant to the code in MainToolbar.cpp
bool VolumeDial::eventFilter( QObject *o, QEvent *e )
{
    if ( e->type() == QEvent::Wheel && !static_cast<QWheelEvent*>(e)->modifiers() )
    {
        if ( o == this || m_wheelProxies.contains( static_cast<QWidget*>( o ) ) )
        {
            QWheelEvent *wev = static_cast<QWheelEvent*>(e);
            if ( o != this )
            {
                QPoint pos( 0, 0 ); // the event needs to be on us or nothing will happen
                QWheelEvent nwev( pos, mapToGlobal( pos ), wev->delta(), wev->buttons(), wev->modifiers() );
                wheelEvent( &nwev );
            }
            else
                wheelEvent( wev );
            return true;
        }
        else // we're not needed globally anymore
            qApp->removeEventFilter( this );
    }
    return false;
}

void VolumeDial::leaveEvent( QEvent * )
{
    startFade();
}

static bool onRing( const QRect &r, const QPoint &p )
{
    const QPoint c = r.center();
    const int dx = p.x() - c.x();
    const int dy = p.y() - c.y();
    return sqrt(dx*dx + dy*dy) > r.width()/4;
}

void VolumeDial::mouseMoveEvent( QMouseEvent *me )
{
    if ( me->buttons() == Qt::NoButton )
        setCursor( onRing( rect(), me->pos() ) ? Qt::PointingHandCursor : Qt::ArrowCursor );
    else if ( m_isClick )
        me->accept();
    else
        QDial::mouseMoveEvent( me );
}

void VolumeDial::mousePressEvent( QMouseEvent *me )
{
    if ( me->button() != Qt::LeftButton )
    {
        QDial::mousePressEvent( me );
        return;
    }

    m_isClick = !onRing( rect(), me->pos() );

    if ( m_isClick )
        update(); // hide the ring
    else
    {
        setCursor( Qt::PointingHandCursor ); // hint dragging
        QDial::mousePressEvent( me ); // this will directly jump to the proper position
    }

    // for value changes caused by mouseevent we'll only let our adjusted value changes be emitted
    // see ::sliderChange()
    m_formerValue = value();
    blockSignals( true );
}

void VolumeDial::mouseReleaseEvent( QMouseEvent *me )
{
    if ( me->button() != Qt::LeftButton )
        return;

    blockSignals( false ); // free signals
    setCursor( Qt::ArrowCursor );
    setSliderDown( false );

    if ( m_isClick )
    {
        m_isClick = !onRing( rect(), me->pos() );
        if ( m_isClick )
            emit muteToggled( !m_muted );
    }

    m_isClick = false;
}

void VolumeDial::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    int icon = m_muted ? 0 : 3;
    if ( icon && value() < 66 )
        icon = value() < 33 ? 1 : 2;
    p.drawPixmap( m_borderWidth,m_borderWidth, m_icon[ icon ] );
    if ( !m_isClick )
    {
        QRect mRec;
        mRec = rect().adjusted(m_borderWidth+4,m_borderWidth+4,-m_borderWidth-4,-m_borderWidth-4);

        p.setPen( QPen( m_sliderGradient, 3, Qt::SolidLine, Qt::RoundCap ) );
        p.setRenderHint( QPainter::Antialiasing );
        p.drawArc( mRec, -110*16, - value()*320*16 / (maximum() - minimum()) );
    }
    p.end();
}

void VolumeDial::removeWheelProxy( QObject *w )
{
    m_wheelProxies.removeOne( static_cast<QWidget*>(w) );
}

void VolumeDial::resizeEvent( QResizeEvent *re )
{
   // int w,h;
    //w = width();
    //h = height();
    if( width() != height() )
        resize( height(), height() );
    else
        QDial::resizeEvent( re );

    if( re->size() != re->oldSize() )
    {
        renderIcons();
        m_sliderGradient = QPixmap( size() );
        updateSliderGradient();
        update();
        setMinimumWidth(width());
    }
}

void VolumeDial::renderIcons()
{

    m_icon[0] = getPixmap("Muted", width()-m_borderWidth*2, height()-m_borderWidth*2);
    m_icon[1] = getPixmap("Volume_low", width()-m_borderWidth*2, height()-m_borderWidth*2);
    m_icon[2] = getPixmap("Volume_mid", width()-m_borderWidth*2, height()-m_borderWidth*2);
    m_icon[3] = getPixmap("Volume",     width()-m_borderWidth*2, height()-m_borderWidth*2);

    if( layoutDirection() == Qt::RightToLeft )
    {
        for ( int i = 0; i < 4; ++i )
            m_icon[i] = QPixmap::fromImage( m_icon[i].toImage().mirrored( true, false ) );
    }
}

void VolumeDial::startFade()
{
    if ( m_anim.timer )
        killTimer( m_anim.timer );
    m_anim.timer = startTimer( 40 );
}

void VolumeDial::stopFade()
{
    killTimer( m_anim.timer );
    m_anim.timer = 0;
    if ( m_anim.step < 0 )
        m_anim.step = 0;
    else if ( m_anim.step > 6 )
        m_anim.step = 6;
}

void VolumeDial::timerEvent( QTimerEvent *te )
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
    updateSliderGradient();
    repaint();
}

void VolumeDial::updateSliderGradient()
{
    m_sliderGradient.fill( Qt::transparent );
    QColor c = m_highlightColor;
    if ( !m_anim.step )
    {
        c.setAlpha( 99 );
        m_sliderGradient.fill( c );
        return;
    }

    QConicalGradient cg( m_sliderGradient.rect().center(), -90 );

    c.setAlpha( 99 + m_anim.step*156/6 );
    cg.setColorAt( 0, c );
    c.setAlpha( 99 + m_anim.step*42/6 );
    cg.setColorAt( 1, c );

    QPainter p( &m_sliderGradient );
    p.fillRect( m_sliderGradient.rect(), cg );
    p.end();
}

void VolumeDial::wheelEvent( QWheelEvent *wev )
{
    QDial::wheelEvent( wev );
    wev->accept();

    const QPoint tooltipPosition = mapToGlobal( rect().translated( 7, -22 ).bottomLeft() );
    QToolTip::showText( tooltipPosition, toolTip() );

    // NOTICE: this is a bit tricky.
    // the ToolTip "QTipLabel" just installed a global eventfilter that intercepts various
    // events and hides itself on them. Therefore every even wheelevent will close the tip
    // ("works - works not - works - works not - ...")
    // so we post-install our own global eventfilter to handle wheel events meant for us bypassing
    // the ToolTip eventfilter

    // first remove to prevent multiple installations but ensure we're on top of the ToolTip filter
    qApp->removeEventFilter( this );
    // it's ultimately removed in the timer triggered ::hideToolTip() slot
    qApp->installEventFilter( this );
}

void VolumeDial::setMuted( bool mute )
{
    m_muted = mute;

    setToolTip( m_muted ? tr( "Приглушенно" ) : tr( "Звук: %1%").arg(value() ) );
    update();
}
bool VolumeDial::muted()
{
    return m_muted;
}


QSize VolumeDial::sizeHint() const
{
    if ( QToolBar *toolBar = qobject_cast<QToolBar*>( parentWidget() ) ) {
        return toolBar->iconSize();
    }
    return QDial::sizeHint();
}

void VolumeDial::sliderChange( SliderChange change )
{
    if ( change == SliderValueChange && isSliderDown() && signalsBlocked() )
    {
        int d = value() - m_formerValue;
        if ( d && d < 33 && d > -33 ) // don't allow real "jumps" > 1/3
        {
            if ( d > 5 ) // ease movement
                d = 5;
            else if ( d < -5 )
                d = -5;
            m_formerValue += d;
            blockSignals( false );
            emit sliderMoved( m_formerValue );
            emit valueChanged( m_formerValue );
            blockSignals( true );
        }
        if ( d )
            setValue( m_formerValue );
    }
    QDial::sliderChange(change);
}

void VolumeDial::valueChangedSlot( int v )
{
    m_isClick = false;

    setToolTip( m_muted ? tr( "Приглушенно" ) : tr( "Звук: %1%").arg(v) );
    update();
}

