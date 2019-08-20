#ifndef DATETIMEDIALOG_H
#define DATETIMEDIALOG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class DateTimeDialog;
}

class DateTimeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DateTimeDialog(QWidget *parent = 0);
    ~DateTimeDialog();
    inline QDateTime dateTime() { return m_time;};
public slots:
    void setDateTime(QDateTime);
private slots:
    void dateChanged();
    void timeChanged(QTime);
    void dateTimeChanged(QDateTime);

private:
    Ui::DateTimeDialog *ui;
    QDateTime m_time;

};

#endif // DATETIMEDIALOG_H
