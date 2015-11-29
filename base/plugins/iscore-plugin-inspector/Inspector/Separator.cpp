#include <qboxlayout.h>
#include <qframe.h>
#include <qlayout.h>

#include "Separator.hpp"

Separator::Separator(QWidget *parent) :
    QWidget {parent}
{
    this->setLayout(new QVBoxLayout);
    QFrame* f = new QFrame;
    this->layout()->addWidget(f);

    f->setFrameShape(QFrame::HLine);
    f->setLineWidth(2);
}

Separator::~Separator()
{

}
