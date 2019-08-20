/****************************************************************************
** Meta object code from reading C++ file 'VideoControl.h'
**
** Created: Wed 3. Oct 15:44:16 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../vlc/VideoControl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VideoControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VlcVideoControl[] = {

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
      19,   17,   16,   16, 0x05,
      61,   16,   16,   16, 0x05,
      93,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     131,  122,   16,   16, 0x0a,
     153,   16,   16,   16, 0x08,
     177,   16,   16,   16, 0x08,
     195,   16,   16,   16, 0x08,
     216,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VlcVideoControl[] = {
    "VlcVideoControl\0\0,\0"
    "actions(QList<QAction*>,Vlc::ActionsType)\0"
    "subtitleTracks(QList<QAction*>)\0"
    "videoTracks(QList<QAction*>)\0subtitle\0"
    "loadSubtitle(QString)\0updateSubtitleActions()\0"
    "updateSubtitles()\0updateVideoActions()\0"
    "updateVideo()\0"
};

void VlcVideoControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VlcVideoControl *_t = static_cast<VlcVideoControl *>(_o);
        switch (_id) {
        case 0: _t->actions((*reinterpret_cast< QList<QAction*>(*)>(_a[1])),(*reinterpret_cast< const Vlc::ActionsType(*)>(_a[2]))); break;
        case 1: _t->subtitleTracks((*reinterpret_cast< QList<QAction*>(*)>(_a[1]))); break;
        case 2: _t->videoTracks((*reinterpret_cast< QList<QAction*>(*)>(_a[1]))); break;
        case 3: _t->loadSubtitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->updateSubtitleActions(); break;
        case 5: _t->updateSubtitles(); break;
        case 6: _t->updateVideoActions(); break;
        case 7: _t->updateVideo(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VlcVideoControl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VlcVideoControl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VlcVideoControl,
      qt_meta_data_VlcVideoControl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VlcVideoControl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VlcVideoControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VlcVideoControl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VlcVideoControl))
        return static_cast<void*>(const_cast< VlcVideoControl*>(this));
    return QObject::qt_metacast(_clname);
}

int VlcVideoControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void VlcVideoControl::actions(QList<QAction*> _t1, const Vlc::ActionsType _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VlcVideoControl::subtitleTracks(QList<QAction*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VlcVideoControl::videoTracks(QList<QAction*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
