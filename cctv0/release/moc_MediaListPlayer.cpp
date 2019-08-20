/****************************************************************************
** Meta object code from reading C++ file 'MediaListPlayer.h'
**
** Created: Wed 3. Oct 15:44:13 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../vlc/MediaListPlayer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MediaListPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcMediaListPlayer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,
      29,   19,   19,   19, 0x05,
      58,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,   68,   19,   19, 0x0a,
      86,   19,   19,   19, 0x0a,
      93,   19,   19,   19, 0x0a,
     100,   19,   19,   19, 0x0a,
     111,   19,   19,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VlcMediaListPlayer[] = {
    "VlcMediaListPlayer\0\0played()\0"
    "nextItemSet(libvlc_media_t*)\0stopped()\0"
    "index\0itemAt(int)\0next()\0play()\0"
    "previous()\0stop()\0"
};

void VlcMediaListPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcMediaListPlayer *_t = static_cast<VlcMediaListPlayer *>(_o);
        switch (_id) {
        case 0: _t->played(); break;
        case 1: _t->nextItemSet((*reinterpret_cast< libvlc_media_t*(*)>(_a[1]))); break;
        case 2: _t->stopped(); break;
        case 3: _t->itemAt((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 4: _t->next(); break;
        case 5: _t->play(); break;
        case 6: _t->previous(); break;
        case 7: _t->stop(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcMediaListPlayer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcMediaListPlayer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VlcMediaListPlayer,
      qt_meta_data_VlcMediaListPlayer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcMediaListPlayer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcMediaListPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcMediaListPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcMediaListPlayer))
        return static_cast<void*>(const_cast< VlcMediaListPlayer*>(this));
    return QObject::qt_metacast(_clname);
}

int VlcMediaListPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void VlcMediaListPlayer::played()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void VlcMediaListPlayer::nextItemSet(libvlc_media_t * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VlcMediaListPlayer::stopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
