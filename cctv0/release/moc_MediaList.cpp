/****************************************************************************
** Meta object code from reading C++ file 'MediaList.h'
**
** Created: Wed 3. Oct 15:44:13 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../vlc/MediaList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MediaList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcMediaList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   14,   13,   13, 0x05,
      47,   14,   13,   13, 0x05,
      80,   14,   13,   13, 0x05,
     113,   14,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_VlcMediaList[] = {
    "VlcMediaList\0\0,\0itemAdded(libvlc_media_t*,int)\0"
    "willAddItem(libvlc_media_t*,int)\0"
    "itemDeleted(libvlc_media_t*,int)\0"
    "willDeleteItem(libvlc_media_t*,int)\0"
};

void VlcMediaList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcMediaList *_t = static_cast<VlcMediaList *>(_o);
        switch (_id) {
        case 0: _t->itemAdded((*reinterpret_cast< libvlc_media_t*(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 1: _t->willAddItem((*reinterpret_cast< libvlc_media_t*(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 2: _t->itemDeleted((*reinterpret_cast< libvlc_media_t*(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 3: _t->willDeleteItem((*reinterpret_cast< libvlc_media_t*(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcMediaList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcMediaList::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VlcMediaList,
      qt_meta_data_VlcMediaList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcMediaList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcMediaList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcMediaList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcMediaList))
        return static_cast<void*>(const_cast< VlcMediaList*>(this));
    return QObject::qt_metacast(_clname);
}

int VlcMediaList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void VlcMediaList::itemAdded(libvlc_media_t * _t1, const int & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VlcMediaList::willAddItem(libvlc_media_t * _t1, const int & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VlcMediaList::itemDeleted(libvlc_media_t * _t1, const int & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VlcMediaList::willDeleteItem(libvlc_media_t * _t1, const int & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
