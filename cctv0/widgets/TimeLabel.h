#ifndef TIME_LABEL_H
#define TIME_LABEL_H

#include <QWidget>
#include <QString>
#include <QDateTime>
#include <QDialog>


class TimeLabel : public QWidget
{
    Q_OBJECT
public:
    explicit TimeLabel(QWidget *parent = 0);

    virtual QSize sizeHint() const;
    //virtual QSize minimumSizeHint() const;
    void setTextHeight(int height);
    inline QDateTime time() { return m_time; };


signals:
    void clicked();
    
public slots:
    void setTime(const QDateTime &time);

protected:
    void paintEvent(QPaintEvent *);
    void enterEvent( QEvent * );
    void leaveEvent( QEvent * );
    void timerEvent ( QTimerEvent * );
    void resizeEvent(QResizeEvent *);
    void mouseMoveEvent( QMouseEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );

private:
    void startFade();
    void stopFade();
    QSize getSizeHint() const;


    int     m_borderSize;
    int     m_sizeHintWidth;
    int     m_sizeHintHeight;

    QDateTime m_time;
    struct
    {
        int step;
        int timer;
    } m_anim;

    QSize m_sizeHint;
    int   m_textHeight;


};

#endif // TIME_LABEL_H
