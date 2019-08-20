/****************************************************************************
** Meta object code from reading C++ file 'videogrid.h'
**
** Created: Mon 8. Oct 14:53:19 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../videogrid.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videogrid.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VideoGrid[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      34,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   10,   10,   10, 0x0a,
      54,   10,   10,   10, 0x0a,
      66,   61,   10,   10, 0x0a,
      85,   10,   10,   10, 0x0a,
     104,   10,   10,   10, 0x0a,
     122,   10,   10,   10, 0x0a,
     133,   10,   10,   10, 0x0a,
     167,   61,   10,   10, 0x0a,
     187,   10,   10,   10, 0x0a,
     212,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VideoGrid[] = {
    "VideoGrid\0\0timeChanged(QDateTime)\0"
    "paused(bool)\0play()\0stop()\0time\0"
    "setTime(QDateTime)\0playerFullscreen()\0"
    "playerActivated()\0calcSize()\0"
    "frontPlayerTimeChanged(QDateTime)\0"
    "syncTime(QDateTime)\0playerNoVideoAvailable()\0"
    "playerTogglePause()\0"
};

void VideoGrid::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VideoGrid *_t = static_cast<VideoGrid *>(_o);
        switch (_id) {
        case 0: _t->timeChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 1: _t->paused((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->play(); break;
        case 3: _t->stop(); break;
        case 4: _t->setTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 5: _t->playerFullscreen(); break;
        case 6: _t->playerActivated(); break;
        case 7: _t->calcSize(); break;
        case 8: _t->frontPlayerTimeChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 9: _t->syncTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 10: _t->playerNoVideoAvailable(); break;
        case 11: _t->playerTogglePause(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VideoGrid::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VideoGrid::staticMetaObject = {
    { &QSplitter::staticMetaObject, qt_meta_stringdata_VideoGrid,
      qt_meta_data_VideoGrid, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VideoGrid::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VideoGrid::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VideoGrid::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VideoGrid))
        return static_cast<void*>(const_cast< VideoGrid*>(this));
    return QSplitter::qt_metacast(_clname);
}

int VideoGrid::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSplitter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void VideoGrid::timeChanged(const QDateTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VideoGrid::paused(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
