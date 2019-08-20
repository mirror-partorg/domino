/****************************************************************************
** Meta object code from reading C++ file 'VolumeDial.h'
**
** Created: Wed 3. Oct 15:44:18 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/VolumeDial.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VolumeDial.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VolumeDial[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   11,   11,   11, 0x0a,
      62,   12,   11,   11, 0x0a,
      82,   11,   77,   11, 0x0a,
      90,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VolumeDial[] = {
    "VolumeDial\0\0mute\0muteToggled(bool)\0"
    "removeWheelProxy(QObject*)\0setMuted(bool)\0"
    "bool\0muted()\0valueChangedSlot(int)\0"
};

void VolumeDial::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VolumeDial *_t = static_cast<VolumeDial *>(_o);
        switch (_id) {
        case 0: _t->muteToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->removeWheelProxy((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 2: _t->setMuted((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: { bool _r = _t->muted();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: _t->valueChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VolumeDial::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VolumeDial::staticMetaObject = {
    { &QDial::staticMetaObject, qt_meta_stringdata_VolumeDial,
      qt_meta_data_VolumeDial, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VolumeDial::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VolumeDial::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VolumeDial::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VolumeDial))
        return static_cast<void*>(const_cast< VolumeDial*>(this));
    return QDial::qt_metacast(_clname);
}

int VolumeDial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDial::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void VolumeDial::muteToggled(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
