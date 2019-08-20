/****************************************************************************
** Meta object code from reading C++ file 'SeekWidget.h'
**
** Created: Wed 3. Oct 15:44:17 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../vlc/SeekWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SeekWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcSeekWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      26,   21,   14,   14, 0x08,
      49,   21,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VlcSeekWidget[] = {
    "VlcSeekWidget\0\0end()\0time\0"
    "updateCurrentTime(int)\0updateFullTime(int)\0"
};

void VlcSeekWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcSeekWidget *_t = static_cast<VlcSeekWidget *>(_o);
        switch (_id) {
        case 0: _t->end(); break;
        case 1: _t->updateCurrentTime((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 2: _t->updateFullTime((*reinterpret_cast< const int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcSeekWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcSeekWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VlcSeekWidget,
      qt_meta_data_VlcSeekWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcSeekWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcSeekWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcSeekWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcSeekWidget))
        return static_cast<void*>(const_cast< VlcSeekWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int VlcSeekWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
