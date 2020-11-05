#include "uwimain.h"
#include "ui_uwimain.h"

UwiMain::UwiMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UwiMain)
{
    ui->setupUi(this);
}

UwiMain::~UwiMain()
{
    delete ui;
}


