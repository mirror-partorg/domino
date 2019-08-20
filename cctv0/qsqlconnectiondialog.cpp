/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsqlconnectiondialog.h"
#include "ui_qsqlconnectiondialog.h"
#include "config.h"
#include <QSqlError>
#include "log.h"

#include <QSqlDatabase>
#include <QTextCodec>
#include <QTextStream>
#include <QFile>

QSqlConnectionDialog::QSqlConnectionDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    QStringList drivers = QSqlDatabase::drivers();

    // remove compat names
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    drivers.removeAll("QODBC3");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");

    /*if (!drivers.contains("QSQLITE"))
        ui.dbCheckBox->setEnabled(false);*/

    ui.comboDriver->addItems(drivers);
}

QSqlConnectionDialog::~QSqlConnectionDialog()
{
}

QString QSqlConnectionDialog::driverName() const
{
    return ui.comboDriver->currentText();
}

QString QSqlConnectionDialog::databaseName() const
{
    return ui.editDatabase->text();
}

QString QSqlConnectionDialog::userName() const
{
    return ui.editUsername->text();
}

QString QSqlConnectionDialog::password() const
{
    return ui.editPassword->text();
}

QString QSqlConnectionDialog::hostName() const
{
    return ui.editHostname->text();
}

int QSqlConnectionDialog::port() const
{
    return ui.portSpinBox->value();
}

void QSqlConnectionDialog::setDriverName(const QString &value)
{
    ui.comboDriver->setCurrentIndex(0);
    for ( int i = 0; i < ui.comboDriver->count(); i++ )
        if ( ui.comboDriver->itemText(i) == value ) {
            ui.comboDriver->setCurrentIndex(i);
            break;
        }
}
void QSqlConnectionDialog::setDatabaseName(const QString &value)
{
    ui.editDatabase->setText(value);
}
void QSqlConnectionDialog::setUserName(const QString &value)
{
    ui.editUsername->setText(value);
}
void QSqlConnectionDialog::setPassword(const QString &value)
{
    ui.editPassword->setText(value);
}
void QSqlConnectionDialog::setHostName(const QString &value)
{
    ui.editHostname->setText(value);
}
void QSqlConnectionDialog::setPort(const int port)
{
    ui.portSpinBox->setValue(port);
}

/*bool QSqlConnectionDialog::useInMemoryDatabase() const
{
    return ui.dbCheckBox->isChecked();
}
*/

void QSqlConnectionDialog::on_okButton_clicked()
{
    if (ui.comboDriver->currentText().isEmpty()) {
        QMessageBox::information(this, tr("No database driver selected"),
                                 tr("Please select a database driver"));
        ui.comboDriver->setFocus();
    } else {
        accept();
    }
}

bool QSqlConnectionDialog::initSqlConnection(QSettings *settings)
{

    QString driverName;
    QString databaseName;
    QString hostName;
    QString userName;
    QString password;
    int port;

    if ( !settings )
        settings = new QSettings(ORGANIZATION_NAME, APPLICATION_NAME1());

    settings->beginGroup("sql-connection");
    driverName = settings->value("driver-name").toString();
    databaseName = settings->value("database-name").toString();
    hostName = settings->value("host-name").toString();
    userName = settings->value("user-name").toString();
    password = settings->value("password").toString();
    port = settings->value("port", -1).toInt();
    QSqlConnectionDialog *dialog = NULL;
    while ( true ) {
        if ( hostName != "" ) {
            QSqlDatabase db = QSqlDatabase::addDatabase(driverName, QString("MainConnection"));
            db.setDatabaseName(databaseName);
            db.setHostName(hostName);
            db.setPort(port);
            if ( !db.open(userName, password) ) {
                 QMessageBox::critical(NULL, QWidget::tr("Ошибка подключения"),
                                       db.lastError().text().toLocal8Bit());
                 db = QSqlDatabase();
                 QSqlDatabase::removeDatabase(QString("MainConnection"));
            }
            else {
                settings->setValue("driver-name", driverName);
                settings->setValue("database-name", databaseName);
                settings->setValue("host-name", hostName);
                settings->setValue("user-name", userName);
                settings->setValue("password", password);
                settings->setValue("port", port);
                if ( dialog )
                    delete dialog;
                settings->endGroup();
                return true;
            }
        }
        if ( !dialog ) {
            dialog = new QSqlConnectionDialog();
            dialog->setWindowTitle(QString("%1 Настройа параметров соединения").arg(APPLICATION_NAME));
            dialog->setHostName(hostName);
            dialog->setDriverName(driverName);
            dialog->setDatabaseName(databaseName);
            dialog->setUserName(userName);
            dialog->setPort(port);
            dialog->setPassword(password);
        }
        if ( dialog->exec() != QDialog::Accepted )
            return false;

        driverName = dialog->driverName();
        databaseName = dialog->databaseName();
        hostName = dialog->hostName();
        userName = dialog->userName();
        password = dialog->password();
        port = dialog->port();
     }
    return true;
}
bool QSqlConnectionDialog::sqlConnectionDialog(QSettings *settings)
{

    QString driverName;
    QString databaseName;
    QString hostName;
    QString userName;
    QString password;
    int port;

    if ( !settings )
        settings = new QSettings(ORGANIZATION_NAME, APPLICATION_NAME);

    settings->beginGroup("sql-connection");
    driverName = settings->value("driver-name").toString();
    databaseName = settings->value("database-name").toString();
    hostName = settings->value("host-name").toString();
    userName = settings->value("user-name").toString();
    password = settings->value("password").toString();
    port = settings->value("port", -1).toInt();
    QSqlConnectionDialog *dialog = NULL;
    dialog = new QSqlConnectionDialog();
    dialog->setWindowTitle(QString("%1 Настройа параметров соединения").arg(APPLICATION_NAME));
    dialog->setHostName(hostName);
    dialog->setDriverName(driverName);
    dialog->setDatabaseName(databaseName);
    dialog->setUserName(userName);
    dialog->setPort(port);
    dialog->setPassword(password);
    if ( dialog->exec() != QDialog::Accepted )
        return false;
    settings->setValue("driver-name", dialog->driverName());
    settings->setValue("database-name",dialog->databaseName());
    settings->setValue("host-name", dialog->hostName());
    settings->setValue("user-name", dialog->userName());
    settings->setValue("password", dialog->password());
    settings->setValue("port", dialog->port());
    return true;
}
QSqlDatabase QSqlConnectionDialog::cloneDatabase(const QString& connectionName)
{
    QSqlDatabase db = QSqlDatabase::cloneDatabase(QSqlDatabase::database("MainConnection"), connectionName);
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME1());
    QString userName;
    QString password;

    settings.beginGroup("sql-connection");
    userName = settings.value("user-name").toString();
    password = settings.value("password").toString();

    if ( !db.open(userName, password) )
         QMessageBox::critical(NULL, QWidget::tr("Ошибка подключения"), db.lastError().text().toLocal8Bit());

    return db;
}
