#ifndef ANLA_H
#define ANLA_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Anla; }
QT_END_NAMESPACE

class Anla : public QMainWindow
{
    Q_OBJECT

public:
    Anla(QWidget *parent = nullptr);
    ~Anla();

private:
    Ui::Anla *ui;
};
#endif // ANLA_H
