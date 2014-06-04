/****************************************************************************
** Meta object code from reading C++ file 'qtanonimizationform.h'
**
** Created: Tue 17. Apr 15:55:26 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qtanonimizationform.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
//#error "The header file 'qtanonimizationform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
//#error "This file was generated using the moc from 4.7.4. It"
//#error "cannot be used with the include files from this version of Qt."
//#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QtAnonimizationForm[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,
      34,   20,   20,   20, 0x08,
      47,   20,   20,   20, 0x08,
      61,   20,   20,   20, 0x08,
      73,   20,   20,   20, 0x08,
      87,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QtAnonimizationForm[] = {
    "QtAnonimizationForm\0\0readFolder()\0"
    "runProcess()\0faceMarking()\0setFilter()\0"
    "clearFilter()\0showMovie(QModelIndex)\0"
};

const QMetaObject QtAnonimizationForm::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QtAnonimizationForm,
      qt_meta_data_QtAnonimizationForm, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QtAnonimizationForm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QtAnonimizationForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QtAnonimizationForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtAnonimizationForm))
        return static_cast<void*>(const_cast< QtAnonimizationForm*>(this));
    return QWidget::qt_metacast(_clname);
}

int QtAnonimizationForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readFolder(); break;
        case 1: runProcess(); break;
        case 2: faceMarking(); break;
        case 3: setFilter(); break;
        case 4: clearFilter(); break;
        case 5: showMovie((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
