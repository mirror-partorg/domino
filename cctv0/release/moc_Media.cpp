/****************************************************************************
** Meta object code from reading C++ file 'Media.h'
**
** Created: Wed 3. Oct 15:44:14 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../vlc/Media.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Media.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcMedia[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      33,    9,    9,    9, 0x05,
      63,    9,    9,    9, 0x05,
      84,    9,    9,    9, 0x05,
     103,    9,    9,    9, 0x05,
     126,    9,    9,    9, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_VlcMedia[] = {
    "VlcMedia\0\0metaChanged(Vlc::Meta)\0"
    "subitemAdded(libvlc_media_t*)\0"
    "durationChanged(int)\0parsedChanged(int)\0"
    "freed(libvlc_media_t*)\0stateChanged(Vlc::State)\0"
};

void VlcMedia::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcMedia *_t = static_cast<VlcMedia *>(_o);
        switch (_id) {
        case 0: _t->metaChanged((*reinterpret_cast< const Vlc::Meta(*)>(_a[1]))); break;
        case 1: _t->subitemAdded((*reinterpret_cast< libvlc_media_t*(*)>(_a[1]))); break;
        case 2: _t->durationChanged((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 3: _t->parsedChanged((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 4: _t->freed((*reinterpret_cast< libvlc_media_t*(*)>(_a[1]))); break;
        case 5: _t->stateChanged((*reinterpret_cast< const Vlc::State(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcMedia::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcMedia::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VlcMedia,
      qt_meta_data_VlcMedia, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcMedia::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcMedia::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcMedia::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcMedia))
        return static_cast<void*>(const_cast< VlcMedia*>(this));
    return QObject::qt_metacast(_clname);
}

int VlcMedia::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void VlcMedia::metaChanged(const Vlc::Meta & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VlcMedia::subitemAdded(libvlc_media_t * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VlcMedia::durationChanged(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VlcMedia::parsedChanged(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void VlcMedia::freed(libvlc_media_t * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void VlcMedia::stateChanged(const Vlc::State & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
