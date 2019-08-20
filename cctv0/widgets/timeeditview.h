#ifndef TIMEEDITVIEW_H
#define TIMEEDITVIEW_H

#include <QWidget>
#include <QTime>
#include <QLabel>
#include <QPushButton>

class TimeEditView : public QWidget
{
    Q_OBJECT
public:
    explicit TimeEditView(QWidget *parent = 0);
    inline const QTime time() { return m_time; };
    void setFont(const QFont &font);

public slots:
    void setTime(const QTime &time);

signals:
    void timeChanged(const QTime &time);
    
private slots:
    void hourButtonUpClicked( bool checked = false );
    void hourButtonDownClicked( bool checked = false );
    void minuteButtonUpClicked( bool checked = false );
    void minuteButtonDownClicked( bool checked = false );
private:
    QTime m_time;
    QLabel *m_hourLabel;
    QLabel *m_minuteLabel;
    QLabel *m_separatorLabel;
    QList <QPushButton*>  buttons;
};

#endif // TIMEEDITVIEW_H
