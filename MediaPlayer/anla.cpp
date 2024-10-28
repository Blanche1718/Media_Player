#include "anla.h"
#include "ui_anla.h"

Anla::Anla(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Anla)
{
    ui->setupUi(this);
}

Anla::~Anla()
{
    delete ui;
}

