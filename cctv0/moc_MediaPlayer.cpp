/****************************************************************************
** Meta object code from reading C++ file 'MediaPlayer.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vlc/MediaPlayer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MediaPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcMediaPlayer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      19,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      27,   15,   15,   15, 0x05,
      44,   15,   15,   15, 0x05,
      50,   15,   15,   15, 0x05,
      58,   15,   15,   15, 0x05,
      68,   15,   15,   15, 0x05,
      87,   15,   15,   15, 0x05,
     117,   15,   15,   15, 0x05,
     134,   15,   15,   15, 0x05,
     144,   15,   15,   15, 0x05,
     166,   15,   15,   15, 0x05,
     175,   15,   15,   15, 0x05,
     185,   15,   15,   15, 0x05,
     208,   15,   15,   15, 0x05,
     230,   15,   15,   15, 0x05,
     253,   15,   15,   15, 0x05,
     263,   15,   15,   15, 0x05,
     280,   15,   15,   15, 0x05,
     298,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     315,  308,   15,   15, 0x0a,
     326,   15,   15,   15, 0x0a,
     334,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VlcMediaPlayer[] = {
    "VlcMediaPlayer\0\0backward()\0buffering(float)\0"
    "end()\0error()\0forward()\0lengthChanged(int)\0"
    "mediaChanged(libvlc_media_t*)\0"
    "nothingSpecial()\0opening()\0"
    "pausableChanged(bool)\0paused()\0playing()\0"
    "positionChanged(float)\0seekableChanged(bool)\0"
    "snapshotTaken(QString)\0stopped()\0"
    "timeChanged(int)\0titleChanged(int)\0"
    "vout(int)\0paused\0play(bool)\0pause()\0"
    "stop()\0"
};

void VlcMediaPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcMediaPlayer *_t = static_cast<VlcMediaPlayer *>(_o);
        switch (_id) {
        case 0: _t->backward(); break;
        case 1: _t->buffering((*reinterpret_cast< const float(*)>(_a[1]))); break;
        case 2: _t->end(); break;
        case 3: _t->error(); break;
        case 4: _t->forward(); break;
        case 5: _t->lengthChanged((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 6: _t->mediaChanged((*reinterpret_cast< libvlc_media_t*(*)>(_a[1]))); break;
        case 7: _t->nothingSpecial(); break;
        case 8: _t->opening(); break;
        case 9: _t->pausableChanged((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 10: _t->paused(); break;
        case 11: _t->playing(); break;
        case 12: _t->positionChanged((*reinterpret_cast< const float(*)>(_a[1]))); break;
        case 13: _t->seekableChanged((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 14: _t->snapshotTaken((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->stopped(); break;
        case 16: _t->timeChanged((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 17: _t->titleChanged((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 18: _t->vout((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 19: _t->play((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->pause(); break;
        case 21: _t->stop(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcMediaPlayer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcMediaPlayer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VlcMediaPlayer,
      qt_meta_data_VlcMediaPlayer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcMediaPlayer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcMediaPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcMediaPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcMediaPlayer))
        return static_cast<void*>(const_cast< VlcMediaPlayer*>(this));
    return QObject::qt_metacast(_clname);
}

int VlcMediaPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void VlcMediaPlayer::backward()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void VlcMediaPlayer::buffering(const float & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VlcMediaPlayer::end()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void VlcMediaPlayer::error()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void VlcMediaPlayer::forward()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void VlcMediaPlayer::lengthChanged(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void VlcMediaPlayer::mediaChanged(libvlc_media_t * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void VlcMediaPlayer::nothingSpecial()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void VlcMediaPlayer::opening()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void VlcMediaPlayer::pausableChanged(const bool & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void VlcMediaPlayer::paused()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void VlcMediaPlayer::playing()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void VlcMediaPlayer::positionChanged(const float & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void VlcMediaPlayer::seekableChanged(const bool & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void VlcMediaPlayer::snapshotTaken(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void VlcMediaPlayer::stopped()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void VlcMediaPlayer::timeChanged(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void VlcMediaPlayer::titleChanged(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void VlcMediaPlayer::vout(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}
QT_END_MOC_NAMESPACE
