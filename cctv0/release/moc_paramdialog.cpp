/****************************************************************************
** Meta object code from reading C++ file 'paramdialog.h'
**
** Created: Mon 8. Oct 14:53:21 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../paramdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paramdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ParamDialog[] = {

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
      13,   12,   12,   12, 0x08,
      25,   12,   12,   12, 0x08,
      40,   12,   12,   12, 0x08,
      55,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ParamDialog[] = {
    "ParamDialog\0\0dbClicked()\0videoClicked()\0"
    "photoClicked()\0accept()\0"
};

void ParamDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ParamDialog *_t = static_cast<ParamDialog *>(_o);
        switch (_id) {
        case 0: _t->dbClicked(); break;
        case 1: _t->videoClicked(); break;
        case 2: _t->photoClicked(); break;
        case 3: _t->accept(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ParamDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ParamDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ParamDialog,
      qt_meta_data_ParamDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ParamDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ParamDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ParamDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ParamDialog))
        return static_cast<void*>(const_cast< ParamDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ParamDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
