/****************************************************************************
** Meta object code from reading C++ file 'player.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "player.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'player.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Player[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      21,    7,    7,    7, 0x05,
      33,    7,    7,    7, 0x05,
      56,    7,    7,    7, 0x05,
      75,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      94,   89,    7,    7, 0x0a,
     113,   89,    7,    7, 0x0a,
     135,    7,    7,    7, 0x0a,
     151,  142,    7,    7, 0x08,
     173,  169,    7,    7, 0x28,
     187,   89,    7,    7, 0x08,
     210,    7,    7,    7, 0x28,
     230,    7,    7,    7, 0x08,
     246,   89,    7,    7, 0x08,
     266,    7,    7,    7, 0x08,
     284,    7,    7,    7, 0x08,
     307,    7,    7,    7, 0x08,
     328,  321,    7,    7, 0x08,
     343,    7,    7,    7, 0x08,
     359,    7,    7,    7, 0x08,
     373,    7,    7,    7, 0x08,
     387,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Player[] = {
    "Player\0\0fullscreen()\0activated()\0"
    "timeChanged(QDateTime)\0noVideoAvailable()\0"
    "togglePause()\0time\0setTime(QDateTime)\0"
    "updateTime(QDateTime)\0stop()\0url,time\0"
    "open(QString,int)\0url\0open(QString)\0"
    "updateCurrentTime(int)\0updateCurrentTime()\0"
    "endVideoPiece()\0updateFullTime(int)\0"
    "videofullscreen()\0videoLeftClick(QPoint)\0"
    "makeToolBar()\0volume\0setVolume(int)\0"
    "volumeClicked()\0playClicked()\0"
    "sizeClicked()\0photoClicked()\0"
};

void Player::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Player *_t = static_cast<Player *>(_o);
        switch (_id) {
        case 0: _t->fullscreen(); break;
        case 1: _t->activated(); break;
        case 2: _t->timeChanged((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 3: _t->noVideoAvailable(); break;
        case 4: _t->togglePause(); break;
        case 5: _t->setTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 6: _t->updateTime((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 7: _t->stop(); break;
        case 8: _t->open((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 9: _t->open((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->updateCurrentTime((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->updateCurrentTime(); break;
        case 12: _t->endVideoPiece(); break;
        case 13: _t->updateFullTime((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->videofullscreen(); break;
        case 15: _t->videoLeftClick((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 16: _t->makeToolBar(); break;
        case 17: _t->setVolume((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 18: _t->volumeClicked(); break;
        case 19: _t->playClicked(); break;
        case 20: _t->sizeClicked(); break;
        case 21: _t->photoClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Player::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Player::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_Player,
      qt_meta_data_Player, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Player::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Player::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Player::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Player))
        return static_cast<void*>(const_cast< Player*>(this));
    return QFrame::qt_metacast(_clname);
}

int Player::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void Player::fullscreen()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Player::activated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Player::timeChanged(const QDateTime & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Player::noVideoAvailable()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Player::togglePause()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
static const uint qt_meta_data_ClickableLabel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ClickableLabel[] = {
    "ClickableLabel\0\0doubleClick()\0"
};

void ClickableLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ClickableLabel *_t = static_cast<ClickableLabel *>(_o);
        switch (_id) {
        case 0: _t->doubleClick(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ClickableLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ClickableLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_ClickableLabel,
      qt_meta_data_ClickableLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClickableLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClickableLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClickableLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClickableLabel))
        return static_cast<void*>(const_cast< ClickableLabel*>(this));
    return QLabel::qt_metacast(_clname);
}

int ClickableLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ClickableLabel::doubleClick()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
