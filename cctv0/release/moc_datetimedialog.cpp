/****************************************************************************
** Meta object code from reading C++ file 'datetimedialog.h'
**
** Created: Wed 3. Oct 15:44:21 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/datetimedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datetimedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DateTimeDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      39,   15,   15,   15, 0x08,
      53,   15,   15,   15, 0x08,
      72,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DateTimeDialog[] = {
    "DateTimeDialog\0\0setDateTime(QDateTime)\0"
    "dateChanged()\0timeChanged(QTime)\0"
    "dateTimeChanged(QDateTime)\0"
};

void DateTimeDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DateTimeDialog *_t = static_cast<DateTimeDialog *>(_o);
        switch (_id) {
        case 0: _t->setDateTime((*reinterpret_cast< QDateTime(*)>(_a[1]))); break;
        case 1: _t->dateChanged(); break;
        case 2: _t->timeChanged((*reinterpret_cast< QTime(*)>(_a[1]))); break;
        case 3: _t->dateTimeChanged((*reinterpret_cast< QDateTime(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DateTimeDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DateTimeDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DateTimeDialog,
      qt_meta_data_DateTimeDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DateTimeDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DateTimeDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DateTimeDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DateTimeDialog))
        return static_cast<void*>(const_cast< DateTimeDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DateTimeDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
