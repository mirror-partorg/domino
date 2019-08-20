/****************************************************************************
** Meta object code from reading C++ file 'SpeedDial.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "widgets/SpeedDial.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SpeedDial.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpeedDial[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      42,   36,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   10,   10,   10, 0x0a,
      90,   11,   10,   10, 0x0a,
     106,   10,   10,   10, 0x0a,
     128,   10,  123,   10, 0x0a,
     144,   10,  137,   10, 0x0a,
     152,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SpeedDial[] = {
    "SpeedDial\0\0pause\0pauseToggled(bool)\0"
    "value\0speedChanged(double)\0"
    "removeWheelProxy(QObject*)\0setPaused(bool)\0"
    "setSpeed(double)\0bool\0paused()\0double\0"
    "speed()\0valueChangedSlot(int)\0"
};

void SpeedDial::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpeedDial *_t = static_cast<SpeedDial *>(_o);
        switch (_id) {
        case 0: _t->pauseToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->speedChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->removeWheelProxy((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 3: _t->setPaused((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setSpeed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: { bool _r = _t->paused();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { double _r = _t->speed();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 7: _t->valueChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SpeedDial::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpeedDial::staticMetaObject = {
    { &QDial::staticMetaObject, qt_meta_stringdata_SpeedDial,
      qt_meta_data_SpeedDial, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpeedDial::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpeedDial::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpeedDial::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpeedDial))
        return static_cast<void*>(const_cast< SpeedDial*>(this));
    return QDial::qt_metacast(_clname);
}

int SpeedDial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDial::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void SpeedDial::pauseToggled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SpeedDial::speedChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
