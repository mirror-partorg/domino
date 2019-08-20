/********************************************************************************
** Form generated from reading UI file 'paramdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PARAMDIALOG_H
#define UI_PARAMDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ParamDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLineEdit *photoEdit;
    QLineEdit *videoEdit;
    QLabel *label;
    QPushButton *videoButton;
    QLabel *label_3;
    QLabel *label_4;
    QPushButton *photoButton;
    QPushButton *dbButton;
    QLabel *label_5;
    QLineEdit *styleEdit;
    QTableWidget *camsTable;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ParamDialog)
    {
        if (ParamDialog->objectName().isEmpty())
            ParamDialog->setObjectName(QString::fromUtf8("ParamDialog"));
        ParamDialog->resize(586, 317);
        verticalLayout_2 = new QVBoxLayout(ParamDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(ParamDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        photoEdit = new QLineEdit(groupBox);
        photoEdit->setObjectName(QString::fromUtf8("photoEdit"));

        gridLayout->addWidget(photoEdit, 3, 1, 1, 1);

        videoEdit = new QLineEdit(groupBox);
        videoEdit->setObjectName(QString::fromUtf8("videoEdit"));

        gridLayout->addWidget(videoEdit, 2, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        videoButton = new QPushButton(groupBox);
        videoButton->setObjectName(QString::fromUtf8("videoButton"));

        gridLayout->addWidget(videoButton, 2, 2, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 6, 0, 1, 1);

        photoButton = new QPushButton(groupBox);
        photoButton->setObjectName(QString::fromUtf8("photoButton"));

        gridLayout->addWidget(photoButton, 3, 2, 1, 1);

        dbButton = new QPushButton(groupBox);
        dbButton->setObjectName(QString::fromUtf8("dbButton"));

        gridLayout->addWidget(dbButton, 1, 1, 1, 2);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        styleEdit = new QLineEdit(groupBox);
        styleEdit->setObjectName(QString::fromUtf8("styleEdit"));

        gridLayout->addWidget(styleEdit, 4, 1, 1, 2);

        camsTable = new QTableWidget(groupBox);
        if (camsTable->columnCount() < 10)
            camsTable->setColumnCount(10);
        if (camsTable->rowCount() < 10)
            camsTable->setRowCount(10);
        camsTable->setObjectName(QString::fromUtf8("camsTable"));
        camsTable->setRowCount(10);
        camsTable->setColumnCount(10);

        gridLayout->addWidget(camsTable, 6, 1, 1, 2);


        verticalLayout_3->addLayout(gridLayout);


        verticalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(ParamDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        verticalLayout_2->addLayout(verticalLayout);

        QWidget::setTabOrder(dbButton, videoEdit);
        QWidget::setTabOrder(videoEdit, videoButton);
        QWidget::setTabOrder(videoButton, photoButton);
        QWidget::setTabOrder(photoButton, photoEdit);
        QWidget::setTabOrder(photoEdit, buttonBox);

        retranslateUi(ParamDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ParamDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ParamDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ParamDialog);
    } // setupUi

    void retranslateUi(QDialog *ParamDialog)
    {
        ParamDialog->setWindowTitle(QApplication::translate("ParamDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ParamDialog", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ParamDialog", "\320\237\321\203\321\202\321\214 \320\264\320\276 \320\262\320\270\320\264\320\265\320\276:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ParamDialog", "\320\241\320\243\320\221\320\224:", 0, QApplication::UnicodeUTF8));
        videoButton->setText(QApplication::translate("ParamDialog", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ParamDialog", "\320\237\321\203\321\202\321\214 \320\264\320\273\321\217 \321\204\320\276\321\202\320\276:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("ParamDialog", "\320\232\320\260\320\274\320\265\321\200\321\213:", 0, QApplication::UnicodeUTF8));
        photoButton->setText(QApplication::translate("ParamDialog", "\320\222\321\213\320\261\321\200\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        dbButton->setText(QApplication::translate("ParamDialog", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213...", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("ParamDialog", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213 \321\201\321\202\320\270\320\273\321\217:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ParamDialog: public Ui_ParamDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PARAMDIALOG_H
