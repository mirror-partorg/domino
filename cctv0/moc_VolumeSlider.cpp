/****************************************************************************
** Meta object code from reading C++ file 'VolumeSlider.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vlc/VolumeSlider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VolumeSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcVolumeSlider[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   16,   32,   16, 0x0a,
      52,   44,   16,   16, 0x0a,
      73,   66,   16,   16, 0x0a,
      92,   16,   88,   16, 0x0a,
     104,  101,   16,   16, 0x0a,
     124,   16,   16,   16, 0x0a,
     137,   16,   16,   16, 0x0a,
     148,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VlcVolumeSlider[] = {
    "VlcVolumeSlider\0\0newVolume(int)\0bool\0"
    "mute()\0enabled\0setMute(bool)\0volume\0"
    "setVolume(int)\0int\0volume()\0up\0"
    "volumeControl(bool)\0volumeDown()\0"
    "volumeUp()\0updateVolume()\0"
};

void VlcVolumeSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcVolumeSlider *_t = static_cast<VlcVolumeSlider *>(_o);
        switch (_id) {
        case 0: _t->newVolume((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 1: { bool _r = _t->mute();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: _t->setMute((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 3: _t->setVolume((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 4: { int _r = _t->volume();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: _t->volumeControl((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 6: _t->volumeDown(); break;
        case 7: _t->volumeUp(); break;
        case 8: _t->updateVolume(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcVolumeSlider::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcVolumeSlider::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VlcVolumeSlider,
      qt_meta_data_VlcVolumeSlider, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcVolumeSlider::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcVolumeSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcVolumeSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcVolumeSlider))
        return static_cast<void*>(const_cast< VlcVolumeSlider*>(this));
    return QWidget::qt_metacast(_clname);
}

int VlcVolumeSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void VlcVolumeSlider::newVolume(const int & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
