/****************************************************************************
** Meta object code from reading C++ file 'd7irc_qt.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "d7irc_qt.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'd7irc_qt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_IRCTextEntry_t {
    QByteArrayData data[4];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IRCTextEntry_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IRCTextEntry_t qt_meta_stringdata_IRCTextEntry = {
    {
QT_MOC_LITERAL(0, 0, 12), // "IRCTextEntry"
QT_MOC_LITERAL(1, 13, 10), // "textSubmit"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 4) // "text"

    },
    "IRCTextEntry\0textSubmit\0\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IRCTextEntry[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void IRCTextEntry::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IRCTextEntry *_t = static_cast<IRCTextEntry *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textSubmit((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (IRCTextEntry::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IRCTextEntry::textSubmit)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject IRCTextEntry::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_IRCTextEntry.data,
      qt_meta_data_IRCTextEntry,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *IRCTextEntry::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IRCTextEntry::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_IRCTextEntry.stringdata0))
        return static_cast<void*>(const_cast< IRCTextEntry*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int IRCTextEntry::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void IRCTextEntry::textSubmit(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_IRCWorker_t {
    QByteArrayData data[17];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IRCWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IRCWorker_t qt_meta_stringdata_IRCWorker = {
    {
QT_MOC_LITERAL(0, 0, 9), // "IRCWorker"
QT_MOC_LITERAL(1, 10, 7), // "privmsg"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 4), // "text"
QT_MOC_LITERAL(4, 24, 7), // "connect"
QT_MOC_LITERAL(5, 32, 4), // "serv"
QT_MOC_LITERAL(6, 37, 4), // "join"
QT_MOC_LITERAL(7, 42, 4), // "nick"
QT_MOC_LITERAL(8, 47, 4), // "chan"
QT_MOC_LITERAL(9, 52, 4), // "part"
QT_MOC_LITERAL(10, 57, 5), // "begin"
QT_MOC_LITERAL(11, 63, 4), // "tick"
QT_MOC_LITERAL(12, 68, 11), // "sendPrivmsg"
QT_MOC_LITERAL(13, 80, 6), // "target"
QT_MOC_LITERAL(14, 87, 3), // "msg"
QT_MOC_LITERAL(15, 91, 7), // "sendRaw"
QT_MOC_LITERAL(16, 99, 3) // "raw"

    },
    "IRCWorker\0privmsg\0\0text\0connect\0serv\0"
    "join\0nick\0chan\0part\0begin\0tick\0"
    "sendPrivmsg\0target\0msg\0sendRaw\0raw"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IRCWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       6,    2,   60,    2, 0x06 /* Public */,
       9,    2,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   70,    2, 0x0a /* Public */,
      11,    0,   71,    2, 0x0a /* Public */,
      12,    2,   72,    2, 0x0a /* Public */,
      15,    1,   77,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   13,   14,
    QMetaType::Void, QMetaType::QString,   16,

       0        // eod
};

void IRCWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IRCWorker *_t = static_cast<IRCWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->privmsg((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->connect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->join((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->part((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->begin(); break;
        case 5: _t->tick(); break;
        case 6: _t->sendPrivmsg((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: _t->sendRaw((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (IRCWorker::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IRCWorker::privmsg)) {
                *result = 0;
            }
        }
        {
            typedef void (IRCWorker::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IRCWorker::connect)) {
                *result = 1;
            }
        }
        {
            typedef void (IRCWorker::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IRCWorker::join)) {
                *result = 2;
            }
        }
        {
            typedef void (IRCWorker::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IRCWorker::part)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject IRCWorker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_IRCWorker.data,
      qt_meta_data_IRCWorker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *IRCWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IRCWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_IRCWorker.stringdata0))
        return static_cast<void*>(const_cast< IRCWorker*>(this));
    return QObject::qt_metacast(_clname);
}

int IRCWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void IRCWorker::privmsg(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void IRCWorker::connect(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void IRCWorker::join(const QString & _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void IRCWorker::part(const QString & _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_IRCBufferModel_t {
    QByteArrayData data[4];
    char stringdata0[32];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IRCBufferModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IRCBufferModel_t qt_meta_stringdata_IRCBufferModel = {
    {
QT_MOC_LITERAL(0, 0, 14), // "IRCBufferModel"
QT_MOC_LITERAL(1, 15, 11), // "serverAdded"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 3) // "idx"

    },
    "IRCBufferModel\0serverAdded\0\0idx"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IRCBufferModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QModelIndex,    3,

       0        // eod
};

void IRCBufferModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IRCBufferModel *_t = static_cast<IRCBufferModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->serverAdded((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (IRCBufferModel::*_t)(const QModelIndex & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&IRCBufferModel::serverAdded)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject IRCBufferModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_IRCBufferModel.data,
      qt_meta_data_IRCBufferModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *IRCBufferModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IRCBufferModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_IRCBufferModel.stringdata0))
        return static_cast<void*>(const_cast< IRCBufferModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int IRCBufferModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void IRCBufferModel::serverAdded(const QModelIndex & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_IRCUserModel_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IRCUserModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IRCUserModel_t qt_meta_stringdata_IRCUserModel = {
    {
QT_MOC_LITERAL(0, 0, 12) // "IRCUserModel"

    },
    "IRCUserModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IRCUserModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void IRCUserModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject IRCUserModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_IRCUserModel.data,
      qt_meta_data_IRCUserModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *IRCUserModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IRCUserModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_IRCUserModel.stringdata0))
        return static_cast<void*>(const_cast< IRCUserModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int IRCUserModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_IRCGUILogic_t {
    QByteArrayData data[5];
    char stringdata0[35];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_IRCGUILogic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_IRCGUILogic_t qt_meta_stringdata_IRCGUILogic = {
    {
QT_MOC_LITERAL(0, 0, 11), // "IRCGUILogic"
QT_MOC_LITERAL(1, 12, 12), // "bufferChange"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 3), // "idx"
QT_MOC_LITERAL(4, 30, 4) // "prev"

    },
    "IRCGUILogic\0bufferChange\0\0idx\0prev"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_IRCGUILogic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,    3,    4,

       0        // eod
};

void IRCGUILogic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        IRCGUILogic *_t = static_cast<IRCGUILogic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->bufferChange((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject IRCGUILogic::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_IRCGUILogic.data,
      qt_meta_data_IRCGUILogic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *IRCGUILogic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IRCGUILogic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_IRCGUILogic.stringdata0))
        return static_cast<void*>(const_cast< IRCGUILogic*>(this));
    return QObject::qt_metacast(_clname);
}

int IRCGUILogic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
