/****************************************************************************
** Meta object code from reading C++ file 'timeeditview.h'
**
** Created: Wed 3. Oct 15:44:22 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/timeeditview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timeeditview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TimeEditView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   14,   13,   13, 0x0a,
      61,   53,   13,   13, 0x08,
      87,   13,   13,   13, 0x28,
     109,   53,   13,   13, 0x08,
     137,   13,   13,   13, 0x28,
     161,   53,   13,   13, 0x08,
     189,   13,   13,   13, 0x28,
     213,   53,   13,   13, 0x08,
     243,   13,   13,   13, 0x28,

       0        // eod
};

static const char qt_meta_stringdata_TimeEditView[] = {
    "TimeEditView\0\0time\0timeChanged(QTime)\0"
    "setTime(QTime)\0checked\0hourButtonUpClicked(bool)\0"
    "hourButtonUpClicked()\0hourButtonDownClicked(bool)\0"
    "hourButtonDownClicked()\0"
    "minuteButtonUpClicked(bool)\0"
    "minuteButtonUpClicked()\0"
    "minuteButtonDownClicked(bool)\0"
    "minuteButtonDownClicked()\0"
};

void TimeEditView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TimeEditView *_t = static_cast<TimeEditView *>(_o);
        switch (_id) {
        case 0: _t->timeChanged((*reinterpret_cast< const QTime(*)>(_a[1]))); break;
        case 1: _t->setTime((*reinterpret_cast< const QTime(*)>(_a[1]))); break;
        case 2: _t->hourButtonUpClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->hourButtonUpClicked(); break;
        case 4: _t->hourButtonDownClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->hourButtonDownClicked(); break;
        case 6: _t->minuteButtonUpClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->minuteButtonUpClicked(); break;
        case 8: _t->minuteButtonDownClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->minuteButtonDownClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TimeEditView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TimeEditView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TimeEditView,
      qt_meta_data_TimeEditView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TimeEditView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TimeEditView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TimeEditView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimeEditView))
        return static_cast<void*>(const_cast< TimeEditView*>(this));
    return QWidget::qt_metacast(_clname);
}

int TimeEditView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void TimeEditView::timeChanged(const QTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
