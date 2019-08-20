/********************************************************************************
** Form generated from reading UI file 'datetimedialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATETIMEDIALOG_H
#define UI_DATETIMEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCalendarWidget>
#include <QtGui/QDateTimeEdit>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "widgets/timeeditview.h"

QT_BEGIN_NAMESPACE

class Ui_DateTimeDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QCalendarWidget *dateEdit;
    TimeEditView *timeEdit;
    QDateTimeEdit *dateTimeEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DateTimeDialog)
    {
        if (DateTimeDialog->objectName().isEmpty())
            DateTimeDialog->setObjectName(QString::fromUtf8("DateTimeDialog"));
        DateTimeDialog->resize(567, 300);
        verticalLayout = new QVBoxLayout(DateTimeDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        dateEdit = new QCalendarWidget(DateTimeDialog);
        dateEdit->setObjectName(QString::fromUtf8("dateEdit"));
        dateEdit->setFirstDayOfWeek(Qt::Monday);
        dateEdit->setGridVisible(false);
        dateEdit->setSelectionMode(QCalendarWidget::SingleSelection);
        dateEdit->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
        dateEdit->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
        dateEdit->setNavigationBarVisible(true);
        dateEdit->setDateEditEnabled(true);

        horizontalLayout->addWidget(dateEdit);

        timeEdit = new TimeEditView(DateTimeDialog);
        timeEdit->setObjectName(QString::fromUtf8("timeEdit"));
        timeEdit->setMinimumSize(QSize(250, 0));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Light, brush1);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush1);
        QBrush brush2(QColor(127, 127, 127, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush2);
        QBrush brush3(QColor(170, 170, 170, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush1);
        QBrush brush4(QColor(255, 255, 220, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        timeEdit->setPalette(palette);
        QFont font;
        font.setPointSize(42);
        font.setBold(true);
        font.setWeight(75);
        timeEdit->setFont(font);
        timeEdit->setAutoFillBackground(true);

        horizontalLayout->addWidget(timeEdit);


        verticalLayout->addLayout(horizontalLayout);

        dateTimeEdit = new QDateTimeEdit(DateTimeDialog);
        dateTimeEdit->setObjectName(QString::fromUtf8("dateTimeEdit"));

        verticalLayout->addWidget(dateTimeEdit);

        buttonBox = new QDialogButtonBox(DateTimeDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DateTimeDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DateTimeDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DateTimeDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DateTimeDialog);
    } // setupUi

    void retranslateUi(QDialog *DateTimeDialog)
    {
        DateTimeDialog->setWindowTitle(QApplication::translate("DateTimeDialog", "\320\222\321\213\320\261\320\276\321\200", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DateTimeDialog: public Ui_DateTimeDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATETIMEDIALOG_H
