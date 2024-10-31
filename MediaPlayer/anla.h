#ifndef ANLA_H
#define ANLA_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QProgressBar>
#include <QAction>
#include <QListWidget>
#include <QFileDialog>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class Anla; }
QT_END_NAMESPACE

class Anla : public QMainWindow
{
    Q_OBJECT

public:
    Anla(QWidget *parent = nullptr);
    ~Anla();
private slots:
    void play();
    void pause();
    void stop();
    void next();
    void prev();
    void searchSong();
    void resetLibrary();
    void setVolume(int volume);
    void setPosition(quint64 position);
    void openFile();
    void currentSong();


private:
    Ui::Anla *ui;
    QMediaPlayer *mediaPlayer;
    QPushButton *play_button;
    QPushButton *pause_button;
    QPushButton *stop_button;
    QPushButton *next_button;
    QPushButton *prev_button;
    QPushButton *reset_button;
    QLineEdit *search_bar;
    QSlider *volumeSlider;
    QSlider *progression;
    QListWidget *music_list;
    QVector<QUrl> library;
    int currentSongIndex;
    QLabel *song_label;

};
#endif // ANLA_H
