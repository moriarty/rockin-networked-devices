/********************************************************************************
** Form generated from reading UI file 'conveyor_belt_gui.ui'
**
** Created: Thu Aug 28 08:22:51 2014
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONVEYOR_BELT_GUI_H
#define UI_CONVEYOR_BELT_GUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *button_close;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QRadioButton *radio_button_forward;
    QRadioButton *radio_button_reverse;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_velocity;
    QSlider *slider_velocity;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *button_start;
    QPushButton *button_stop;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(320, 225);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        button_close = new QPushButton(centralwidget);
        button_close->setObjectName(QString::fromUtf8("button_close"));
        button_close->setGeometry(QRect(110, 140, 98, 27));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 20, 283, 98));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        radio_button_forward = new QRadioButton(layoutWidget);
        radio_button_forward->setObjectName(QString::fromUtf8("radio_button_forward"));

        horizontalLayout->addWidget(radio_button_forward);

        radio_button_reverse = new QRadioButton(layoutWidget);
        radio_button_reverse->setObjectName(QString::fromUtf8("radio_button_reverse"));

        horizontalLayout->addWidget(radio_button_reverse);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_velocity = new QLabel(layoutWidget);
        label_velocity->setObjectName(QString::fromUtf8("label_velocity"));

        horizontalLayout_2->addWidget(label_velocity);

        slider_velocity = new QSlider(layoutWidget);
        slider_velocity->setObjectName(QString::fromUtf8("slider_velocity"));
        slider_velocity->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(slider_velocity);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        button_start = new QPushButton(layoutWidget);
        button_start->setObjectName(QString::fromUtf8("button_start"));

        horizontalLayout_3->addWidget(button_start);

        button_stop = new QPushButton(layoutWidget);
        button_stop->setObjectName(QString::fromUtf8("button_stop"));

        horizontalLayout_3->addWidget(button_stop);


        verticalLayout->addLayout(horizontalLayout_3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 320, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        button_close->setText(QApplication::translate("MainWindow", "Close", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Belt Direction:", 0, QApplication::UnicodeUTF8));
        radio_button_forward->setText(QApplication::translate("MainWindow", "Forward", 0, QApplication::UnicodeUTF8));
        radio_button_reverse->setText(QApplication::translate("MainWindow", "Reverse", 0, QApplication::UnicodeUTF8));
        label_velocity->setText(QApplication::translate("MainWindow", "Velocity:", 0, QApplication::UnicodeUTF8));
        button_start->setText(QApplication::translate("MainWindow", "Start", 0, QApplication::UnicodeUTF8));
        button_stop->setText(QApplication::translate("MainWindow", "Stop", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONVEYOR_BELT_GUI_H
