/********************************************************************************
** Form generated from reading UI file 'qtanonimizationform.ui'
**
** Created: Tue 17. Apr 15:55:25 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTANONIMIZATIONFORM_H
#define UI_QTANONIMIZATIONFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTableView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtAnonimizationForm
{
public:
    QTableView *fileTable;
    QPushButton *RunProcessButton;
    QPushButton *readFolderButton;
    QPushButton *MarkFacesButton;
    QPushButton *SetFilterButton;
    QPushButton *ClearFilterButton;
    QComboBox *FilterCBox;
    QLabel *FilterLabel;

    void setupUi(QWidget *QtAnonimizationForm)
    {
        if (QtAnonimizationForm->objectName().isEmpty())
            QtAnonimizationForm->setObjectName(QString::fromUtf8("QtAnonimizationForm"));
        QtAnonimizationForm->resize(917, 631);
        fileTable = new QTableView(QtAnonimizationForm);
        fileTable->setObjectName(QString::fromUtf8("fileTable"));
        fileTable->setGeometry(QRect(10, 50, 901, 511));
		fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        RunProcessButton = new QPushButton(QtAnonimizationForm);
        RunProcessButton->setObjectName(QString::fromUtf8("RunProcessButton"));
        RunProcessButton->setGeometry(QRect(230, 570, 75, 23));
        readFolderButton = new QPushButton(QtAnonimizationForm);
        readFolderButton->setObjectName(QString::fromUtf8("readFolderButton"));
        readFolderButton->setGeometry(QRect(10, 570, 91, 23));
        MarkFacesButton = new QPushButton(QtAnonimizationForm);
        MarkFacesButton->setObjectName(QString::fromUtf8("MarkFacesButton"));
        MarkFacesButton->setGeometry(QRect(110, 570, 111, 23));
        SetFilterButton = new QPushButton(QtAnonimizationForm);
        SetFilterButton->setObjectName(QString::fromUtf8("SetFilterButton"));
        SetFilterButton->setGeometry(QRect(370, 10, 75, 23));
        ClearFilterButton = new QPushButton(QtAnonimizationForm);
        ClearFilterButton->setObjectName(QString::fromUtf8("ClearFilterButton"));
        ClearFilterButton->setGeometry(QRect(450, 10, 75, 23));
        FilterCBox = new QComboBox(QtAnonimizationForm);
        FilterCBox->setObjectName(QString::fromUtf8("FilterCBox"));
        FilterCBox->setGeometry(QRect(50, 10, 311, 22));
        FilterCBox->setModelColumn(0);
		FilterCBox->addItem("Wszystkie");
		FilterCBox->addItem("Wczytany");
		FilterCBox->addItem("Sparametryzowany");
		FilterCBox->addItem("Przetworzony");
		FilterCBox->addItem("Zatwierdzony");
		FilterCBox->addItem("Pominiety");
		FilterCBox->addItem("Do poprawy");

        retranslateUi(QtAnonimizationForm);
        QObject::connect(RunProcessButton, SIGNAL(clicked()), QtAnonimizationForm, SLOT(runProcess()));
        QObject::connect(readFolderButton, SIGNAL(clicked()), QtAnonimizationForm, SLOT(readFolder()));
        QObject::connect(MarkFacesButton, SIGNAL(clicked()), QtAnonimizationForm, SLOT(faceMarking()));
        QObject::connect(SetFilterButton, SIGNAL(clicked()), QtAnonimizationForm, SLOT(setFilter()));
        QObject::connect(ClearFilterButton, SIGNAL(clicked()), QtAnonimizationForm, SLOT(clearFilter()));
        QObject::connect(fileTable, SIGNAL(doubleClicked(QModelIndex)), QtAnonimizationForm, SLOT(showMovie(QModelIndex)));

        QMetaObject::connectSlotsByName(QtAnonimizationForm);
    } // setupUi

    void retranslateUi(QWidget *QtAnonimizationForm)
    {
        QtAnonimizationForm->setWindowTitle(QApplication::translate("QtAnonimizationForm", "Form", 0, QApplication::UnicodeUTF8));
        RunProcessButton->setText(QApplication::translate("QtAnonimizationForm", "Anonimizuj", 0, QApplication::UnicodeUTF8));
        readFolderButton->setText(QApplication::translate("QtAnonimizationForm", "Otworz folder", 0, QApplication::UnicodeUTF8));
        MarkFacesButton->setText(QApplication::translate("QtAnonimizationForm", "Zaznacz Parametry", 0, QApplication::UnicodeUTF8));
        SetFilterButton->setText(QApplication::translate("QtAnonimizationForm", "Ustaw filtr", 0, QApplication::UnicodeUTF8));
        ClearFilterButton->setText(QApplication::translate("QtAnonimizationForm", "Usun filtr", 0, QApplication::UnicodeUTF8));
        //FilterLabel->setText(QApplication::translate("QtAnonimizationForm", "Filtruj", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QtAnonimizationForm: public Ui_QtAnonimizationForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTANONIMIZATIONFORM_H
