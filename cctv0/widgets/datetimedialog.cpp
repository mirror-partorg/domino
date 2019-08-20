#include "datetimedialog.h"
#include "ui_datetimedialog.h"

DateTimeDialog::DateTimeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateTimeDialog)
{
    ui->setupUi(this);
    connect(ui->dateEdit, SIGNAL(selectionChanged()), this, SLOT(dateChanged()));
    connect(ui->timeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(timeChanged(QTime)));
    connect(ui->dateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(dateTimeChanged(QDateTime)));
}

DateTimeDialog::~DateTimeDialog()
{
    delete ui;
}
void DateTimeDialog::setDateTime(QDateTime time)
{
    if ( m_time == time )
        return;
    m_time = time;
    ui->dateTimeEdit->setDateTime(m_time);
    ui->dateEdit->setSelectedDate(m_time.date());
    ui->timeEdit->setTime(m_time.time());
}
void DateTimeDialog::dateChanged()
{
    QDateTime d(ui->dateEdit->selectedDate(), ui->dateTimeEdit->time());
    setDateTime(d);
}
void DateTimeDialog::timeChanged(QTime)
{
    QDateTime d(ui->dateTimeEdit->date(), ui->timeEdit->time());
    setDateTime(d);

}

void DateTimeDialog::dateTimeChanged(QDateTime)
{
    setDateTime(ui->dateTimeEdit->dateTime());

}
