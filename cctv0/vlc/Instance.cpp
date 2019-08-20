/****************************************************************************
* VLC-Qt - Qt and libvlc connector library
* Copyright (C) 2012 Tadej Novak <tadej@tano.si>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <vlc/vlc.h>

//#include "Config.h"
#include "Enums.h"
#include "Error.h"
#include "Instance.h"

VlcInstance::VlcInstance(const QStringList &args,
                         QObject *parent)
    : QObject(parent),
      _vlcInstance(0)
{
    // Convert arguments to required format
    std::string stdStrings[args.size()];
    const char *vlcArgs[args.size()];
    for(int i = 0; i < args.size(); i++) {
        stdStrings[i] = args[i].toStdString();
        vlcArgs[i] = stdStrings[i].c_str();
    }

    // Create new libvlc instance
    _vlcInstance = libvlc_new(sizeof(vlcArgs) / sizeof(*vlcArgs), vlcArgs);

    qRegisterMetaType<Vlc::Meta>("Vlc::Meta");
    qRegisterMetaType<Vlc::State>("Vlc::State");

    VlcError::errmsg();

    // Check if instance is running
    if(_vlcInstance) {
        qDebug() << "Using libvlc version:" << version();
    } else {
        qDebug() << "libvlc-qt Error: libvlc failed to load!";
        exit(-100);
    }
}

VlcInstance::~VlcInstance()
{
    libvlc_release(_vlcInstance);
}

libvlc_instance_t *VlcInstance::core()
{
    return _vlcInstance;
}

QString VlcInstance::changeset()
{
    // Returns libvlc changeset
    return QString(libvlc_get_changeset());
}

QString VlcInstance::compiler()
{
    // Returns libvlc compiler version
    return QString(libvlc_get_compiler());
}

QString VlcInstance::version()
{
    // Returns libvlc version
    return QString(libvlc_get_version());
}
int VlcInstance::majorVersion()
{
    QString ver;
    ver = version();
    return ver.left(ver.indexOf('.')).toInt();
}

void VlcInstance::setUserAgent(const QString &application,
                               const QString &version)
{
    QString applicationOutput = application + " " + version;
    QString httpOutput = application + "/" + version; // "AppName/1.2.3 libvlc-qt/1.2.3"
    libvlc_set_user_agent(_vlcInstance, applicationOutput.toAscii().data(), httpOutput.toAscii().data());
}
