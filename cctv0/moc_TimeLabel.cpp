/****************************************************************************
** Meta object code from reading C++ file 'TimeLabel.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "widgets/TimeLabel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TimeLabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TimeLabel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      26,   21,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TimeLabel[] = {
    "TimeLabel\0\0clicked()\0time\0setTime(QDateTime)\0"
};

void TimeLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TimeLabel *_t = static_cast<TimeLabel *>(_o);
        switch (_id) {
        case 0: _t->clicked(); break;
        case 1: _t->setTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TimeLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TimeLabel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TimeLabel,
      qt_meta_data_TimeLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TimeLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TimeLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TimeLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimeLabel))
        return static_cast<void*>(const_cast< TimeLabel*>(this));
    return QWidget::qt_metacast(_clname);
}

int TimeLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void TimeLabel::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
