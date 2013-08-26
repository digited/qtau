/****************************************************************************
** Meta object code from reading C++ file 'rocatool.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../rocatool/rocatool.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rocatool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RocaTool_t {
    QByteArrayData data[17];
    char stringdata[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_RocaTool_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_RocaTool_t qt_meta_stringdata_RocaTool = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 9),
QT_MOC_LITERAL(2, 19, 0),
QT_MOC_LITERAL(3, 20, 8),
QT_MOC_LITERAL(4, 29, 5),
QT_MOC_LITERAL(5, 35, 5),
QT_MOC_LITERAL(6, 41, 5),
QT_MOC_LITERAL(7, 47, 5),
QT_MOC_LITERAL(8, 53, 5),
QT_MOC_LITERAL(9, 59, 5),
QT_MOC_LITERAL(10, 65, 5),
QT_MOC_LITERAL(11, 71, 5),
QT_MOC_LITERAL(12, 77, 5),
QT_MOC_LITERAL(13, 83, 5),
QT_MOC_LITERAL(14, 89, 5),
QT_MOC_LITERAL(15, 95, 5),
QT_MOC_LITERAL(16, 101, 6)
    },
    "RocaTool\0onLoadWav\0\0fileName\0onbF1\0"
    "onbF2\0onbF3\0onaF1\0onaF2\0onaF3\0onbS1\0"
    "onbS2\0onbS3\0onaS1\0onaS2\0onaS3\0onPlay\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RocaTool[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x0a,
       4,    1,   87,    2, 0x0a,
       5,    1,   90,    2, 0x0a,
       6,    1,   93,    2, 0x0a,
       7,    1,   96,    2, 0x0a,
       8,    1,   99,    2, 0x0a,
       9,    1,  102,    2, 0x0a,
      10,    1,  105,    2, 0x0a,
      11,    1,  108,    2, 0x0a,
      12,    1,  111,    2, 0x0a,
      13,    1,  114,    2, 0x0a,
      14,    1,  117,    2, 0x0a,
      15,    1,  120,    2, 0x0a,
      16,    0,  123,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void RocaTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RocaTool *_t = static_cast<RocaTool *>(_o);
        switch (_id) {
        case 0: _t->onLoadWav((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->onbF1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onbF2((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onbF3((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onaF1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onaF2((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->onaF3((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->onbS1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->onbS2((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->onbS3((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->onaS1((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->onaS2((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->onaS3((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->onPlay(); break;
        default: ;
        }
    }
}

const QMetaObject RocaTool::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_RocaTool.data,
      qt_meta_data_RocaTool,  qt_static_metacall, 0, 0}
};


const QMetaObject *RocaTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RocaTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RocaTool.stringdata))
        return static_cast<void*>(const_cast< RocaTool*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int RocaTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
