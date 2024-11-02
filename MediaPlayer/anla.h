#ifndef ANLA_H
#define ANLA_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QTextStream>
#include <QLineEdit>
#include <QProgressBar>
#include <QAction>
#include <QListWidget>
#include <QFileDialog>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <algorithm>
#include <random>


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
    void currentSong(const QUrl &songUrl);
    void openM3uFile();
    void saveM3uFile();
    bool initializeDatabase();
    void libraryDatabase();
    void saveSongDatabase(const QUrl &songUrl);
    void random();
    void updateProgression(qint64 position);
    void addToLibrary();
    void searchInPlaylist();
    void playPlaylist();
    void playNextInPlaylist();





private:
    Ui::Anla *ui;
    QMediaPlayer *mediaPlayer;
    QPushButton *play_button;
    QPushButton *pause_button;
    QPushButton *stop_button;
    QPushButton *next_button;
    QPushButton *prev_button;
    QPushButton *reset_button;
    QPushButton *random_button;
    QLineEdit *search_bar;
    QSlider *volumeSlider;
    QSlider *progression;
    QListWidget *music_list;
    QListWidget *song_list;
    QVector<QUrl> library;
    QList<QUrl> playlist;
    int currentSongIndex;
    int currentPlaylistIndex=-1;
    QLabel *song_label;
    QLabel *volume_label;
    QLabel *progression_label;
    QSqlDatabase db;
};
#endif // ANLA_H
