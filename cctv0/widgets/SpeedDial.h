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

#ifndef SpeedDIAL_H
#define SpeedDIAL_H

#include <QDial>


class SpeedDial : public QDial
{
    Q_OBJECT

public:
    SpeedDial( QWidget *parent = 0 );
    /**
        Add a list of widgets that should not hide the tooltip on wheelevents, but instead cause
        wheelevents on the dial
        You do NOT have to remove them on deconstruction.
    */
    void addWheelProxies( QList<QWidget*> proxies );
    QSize sizeHint() const;

public slots:
    /**
       Remove an added wheelproxy. The slot is automatically bound to the widgets deconstruction
       signal when added. You don't have to do that.
    */
    void removeWheelProxy( QObject * );
    void setPaused( bool pause );
    void setSpeed(double);
    bool paused();
    double speed();

signals:
    void pauseToggled( bool pause );
    void speedChanged(double value);

protected:
    void enterEvent( QEvent * );
    bool eventFilter( QObject *o, QEvent *e );
    void leaveEvent( QEvent * );
    void paintEvent( QPaintEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void resizeEvent(QResizeEvent *);
    void sliderChange( SliderChange change );
    void timerEvent ( QTimerEvent * );
    friend class MainToolbar;
    void wheelEvent( QWheelEvent * );

private:
    void startFade();
    void stopFade();
    void renderIcons();
    void updateSliderGradient();

private slots:
    void valueChangedSlot( int );

private:
    double intValueToDouble(int);
    int    doubleValueToInt(double);
    QString intValueToString(int);

    QPixmap m_icon[2];
    QPixmap m_sliderGradient;
    int m_formerValue;
    QList<QWidget*> m_wheelProxies;
    struct
    {
        int step;
        int timer;
    } m_anim;
    bool m_isClick, m_isDown, m_paused;
    QColor m_highlightColor;
    int m_borderWidth;
};

#endif  // end include guard
