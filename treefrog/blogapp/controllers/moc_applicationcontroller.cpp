/****************************************************************************
** Meta object code from reading C++ file 'applicationcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "applicationcontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'applicationcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ApplicationController_t {
    QByteArrayData data[4];
    char stringdata[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_ApplicationController_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_ApplicationController_t qt_meta_stringdata_ApplicationController = {
    {
QT_MOC_LITERAL(0, 0, 21),
QT_MOC_LITERAL(1, 22, 16),
QT_MOC_LITERAL(2, 39, 0),
QT_MOC_LITERAL(3, 40, 13)
    },
    "ApplicationController\0staticInitialize\0"
    "\0staticRelease\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ApplicationController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a,
       3,    0,   25,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ApplicationController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ApplicationController *_t = static_cast<ApplicationController *>(_o);
        switch (_id) {
        case 0: _t->staticInitialize(); break;
        case 1: _t->staticRelease(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ApplicationController::staticMetaObject = {
    { &TActionController::staticMetaObject, qt_meta_stringdata_ApplicationController.data,
      qt_meta_data_ApplicationController,  qt_static_metacall, 0, 0}
};


const QMetaObject *ApplicationController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ApplicationController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ApplicationController.stringdata))
        return static_cast<void*>(const_cast< ApplicationController*>(this));
    return TActionController::qt_metacast(_clname);
}

int ApplicationController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TActionController::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
