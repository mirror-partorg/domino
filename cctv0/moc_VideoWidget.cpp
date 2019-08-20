/****************************************************************************
** Meta object code from reading C++ file 'VideoWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "vlc/VideoWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VideoWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcVideoWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      29,   15,   15,   15, 0x05,
      48,   15,   15,   15, 0x05,
      66,   15,   15,   15, 0x05,
      78,   15,   15,   15, 0x05,
      96,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     108,   15,   15,   15, 0x0a,
     126,   15,   15,   15, 0x0a,
     145,   15,   15,   15, 0x0a,
     164,   15,   15,   15, 0x0a,
     188,   15,   15,   15, 0x0a,
     213,   15,   15,   15, 0x0a,
     241,  235,   15,   15, 0x0a,
     268,  235,   15,   15, 0x0a,
     307,  293,   15,   15, 0x0a,
     350,  344,   15,   15, 0x0a,
     371,   15,   15,   15, 0x08,
     383,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VlcVideoWidget[] = {
    "VlcVideoWidget\0\0fullscreen()\0"
    "rightClick(QPoint)\0leftClick(QPoint)\0"
    "wheel(bool)\0mouseShow(QPoint)\0mouseHide()\0"
    "enableMouseHide()\0disableMouseHide()\0"
    "toggleFullscreen()\0enableDefaultSettings()\0"
    "enablePreviousSettings()\0initDefaultSettings()\0"
    "ratio\0setAspectRatio(Vlc::Ratio)\0"
    "setCropRatio(Vlc::Ratio)\0deinterlacing\0"
    "setDeinterlacing(Vlc::Deinterlacing)\0"
    "scale\0setScale(Vlc::Scale)\0hideMouse()\0"
    "applyPreviousSettings()\0"
};

void VlcVideoWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcVideoWidget *_t = static_cast<VlcVideoWidget *>(_o);
        switch (_id) {
        case 0: _t->fullscreen(); break;
        case 1: _t->rightClick((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->leftClick((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: _t->wheel((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 4: _t->mouseShow((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 5: _t->mouseHide(); break;
        case 6: _t->enableMouseHide(); break;
        case 7: _t->disableMouseHide(); break;
        case 8: _t->toggleFullscreen(); break;
        case 9: _t->enableDefaultSettings(); break;
        case 10: _t->enablePreviousSettings(); break;
        case 11: _t->initDefaultSettings(); break;
        case 12: _t->setAspectRatio((*reinterpret_cast< const Vlc::Ratio(*)>(_a[1]))); break;
        case 13: _t->setCropRatio((*reinterpret_cast< const Vlc::Ratio(*)>(_a[1]))); break;
        case 14: _t->setDeinterlacing((*reinterpret_cast< const Vlc::Deinterlacing(*)>(_a[1]))); break;
        case 15: _t->setScale((*reinterpret_cast< const Vlc::Scale(*)>(_a[1]))); break;
        case 16: _t->hideMouse(); break;
        case 17: _t->applyPreviousSettings(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcVideoWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcVideoWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_VlcVideoWidget,
      qt_meta_data_VlcVideoWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcVideoWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcVideoWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcVideoWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcVideoWidget))
        return static_cast<void*>(const_cast< VlcVideoWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int VlcVideoWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void VlcVideoWidget::fullscreen()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void VlcVideoWidget::rightClick(const QPoint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VlcVideoWidget::leftClick(const QPoint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void VlcVideoWidget::wheel(const bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void VlcVideoWidget::mouseShow(const QPoint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void VlcVideoWidget::mouseHide()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}
QT_END_MOC_NAMESPACE
