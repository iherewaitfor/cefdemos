/****************************************************************************
** Meta object code from reading C++ file 'autosignalsemitter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "autosignalsemitter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'autosignalsemitter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AutoSignalsEmitter_t {
    QByteArrayData data[8];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AutoSignalsEmitter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AutoSignalsEmitter_t qt_meta_stringdata_AutoSignalsEmitter = {
    {
QT_MOC_LITERAL(0, 0, 18), // "AutoSignalsEmitter"
QT_MOC_LITERAL(1, 19, 11), // "proxySignal"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 6), // "sender"
QT_MOC_LITERAL(4, 39, 11), // "QMetaMethod"
QT_MOC_LITERAL(5, 51, 6), // "signal"
QT_MOC_LITERAL(6, 58, 9), // "arguments"
QT_MOC_LITERAL(7, 68, 9) // "proxySlot"

    },
    "AutoSignalsEmitter\0proxySignal\0\0sender\0"
    "QMetaMethod\0signal\0arguments\0proxySlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AutoSignalsEmitter[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   31,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QObjectStar, 0x80000000 | 4, QMetaType::QVariantList,    3,    5,    6,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void AutoSignalsEmitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<AutoSignalsEmitter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->proxySignal((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< QMetaMethod(*)>(_a[2])),(*reinterpret_cast< QVariantList(*)>(_a[3]))); break;
        case 1: _t->proxySignalEmit(_a); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AutoSignalsEmitter::*)(QObject * , QMetaMethod , QVariantList );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AutoSignalsEmitter::proxySignal)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AutoSignalsEmitter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_AutoSignalsEmitter.data,
    qt_meta_data_AutoSignalsEmitter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *AutoSignalsEmitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutoSignalsEmitter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AutoSignalsEmitter.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AutoSignalsEmitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void AutoSignalsEmitter::proxySignal(QObject * _t1, QMetaMethod _t2, QVariantList _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
