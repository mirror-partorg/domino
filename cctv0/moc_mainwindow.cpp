/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      23,   11,   11,   11, 0x08,
      42,   36,   11,   11, 0x08,
      57,   11,   11,   11, 0x08,
      79,   74,   11,   11, 0x08,
     103,   97,   11,   11, 0x08,
     116,   97,   11,   11, 0x08,
     135,   11,   11,   11, 0x08,
     162,   11,   11,   11, 0x08,
     190,   11,   11,   11, 0x08,
     202,   11,   11,   11, 0x08,
     209,   11,   11,   11, 0x08,
     217,   11,   11,   11, 0x08,
     228,   11,   11,   11, 0x08,
     240,   11,   11,   11, 0x08,
     248,   11,   11,   11, 0x08,
     256,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0shutdown()\0fullScreen()\0"
    "value\0newVolume(int)\0newSpeed(double)\0"
    "mute\0muteToggled(bool)\0pause\0paused(bool)\0"
    "pauseToggled(bool)\0gridTimeChanged(QDateTime)\0"
    "dateTimeSelected(QDateTime)\0playPause()\0"
    "left()\0right()\0leftPage()\0rightPage()\0"
    "about()\0param()\0visibilityChanged(bool)\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->shutdown(); break;
        case 1: _t->fullScreen(); break;
        case 2: _t->newVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->newSpeed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->muteToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->paused((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->pauseToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->gridTimeChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 8: _t->dateTimeSelected((*reinterpret_cast< QDateTime(*)>(_a[1]))); break;
        case 9: _t->playPause(); break;
        case 10: _t->left(); break;
        case 11: _t->right(); break;
        case 12: _t->leftPage(); break;
        case 13: _t->rightPage(); break;
        case 14: _t->about(); break;
        case 15: _t->param(); break;
        case 16: _t->visibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
