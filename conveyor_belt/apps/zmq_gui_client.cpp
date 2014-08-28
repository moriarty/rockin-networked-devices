#include "conveyor_belt_gui.h"
#include "conveyor_belt_gui.moc"
#include <qt4/QtGui/QApplication>

int main(int argc, char* argv[])
{
    QApplication qt_app(argc, argv);
    ConveyorBeltGui gui;

    gui.show();

    return qt_app.exec();
}
