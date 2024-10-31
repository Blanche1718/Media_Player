#include "anla.h"
#include "ui_anla.h"


Anla::Anla(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Anla),
      mediaPlayer(new QMediaPlayer(this)),
      currentSongIndex(0)




{
    ui->setupUi(this);

    song_label = ui->song_label;


    library.append(QUrl::fromLocalFile("C:/Users/DELL/Music/Billie Eilish WILDFLOWER Official Lyric Video_wX1tbAFOef_Fny.mp3"));
    library.append(QUrl::fromLocalFile("C:/Users/DELL/Music/burna_boy_for_my_han..._71787.m4a"));
    library.append(QUrl::fromLocalFile("C:/Users/DELL/Music/intuition_aac_71522.m4a"));


    for (const auto &song : library){
        QListWidgetItem *item = new QListWidgetItem(QFileInfo(song.toLocalFile()).fileName());
        ui->music_list->addItem(item);
    }


    connect(ui->actionOpen_File,&QAction::triggered,this,&Anla::openFile);
    connect(ui->play_button,&QPushButton::clicked,this,&Anla::play);
    connect(ui->pause_button,&QPushButton::clicked,this,&Anla::pause);
    connect(ui->stop_button,&QPushButton::clicked,this,&Anla::stop);
    connect(ui->next_button,&QPushButton::clicked,this,&Anla::next);
    connect(ui->prev_button,&QPushButton::clicked,this,&Anla::prev);
    connect(ui->reset_button,&QPushButton::clicked,this,&Anla::resetLibrary);
    connect(ui->search_bar,&QLineEdit::textEdited,this,&Anla::searchSong);
    connect(ui->volumeSlider,&QSlider::valueChanged,this,&Anla::setVolume);
    connect(ui->progression,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);
    connect(mediaPlayer,&QMediaPlayer::positionChanged,ui->progression,&QSlider::setValue);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->progression->setMaximum(duration);
    });


    connect(ui->music_list,&QListWidget::currentRowChanged,this,[this](int row){
        if(row>=0 && row< library.size()){
            currentSongIndex=row;
            mediaPlayer->setMedia(library[currentSongIndex]);
            mediaPlayer->play();

        }
    } );

    connect(ui->music_list, &QListWidget::currentRowChanged, this, &Anla::currentSong);






}

Anla::~Anla()
{
    delete ui;
}


void Anla::openFile(){
    QStringList files = QFileDialog::getOpenFileNames(this, "Open Audio File", "", "Audio Files (*.mp3 *.wav *.m4a *.ogg);;All Files(*)");
    for (const QString &file : files) {
            QUrl url = QUrl::fromLocalFile(file);
            library.append(url);
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(file).fileName());
            ui->music_list->addItem(item);
        }
}

void Anla::play(){
    if (mediaPlayer->mediaStatus()==QMediaPlayer::NoMedia){
        openFile();
    }else{
        currentSong();
        mediaPlayer->play();

    }

}


void Anla::pause(){
    mediaPlayer->pause();
}

void Anla::stop(){
    mediaPlayer->stop();
}

void Anla::setVolume(int volume){
    mediaPlayer->setVolume(volume);
}

void Anla::setPosition(quint64 position){
    mediaPlayer->setPosition(position);
}

void Anla::next(){
    if(!library.isEmpty()){
        currentSongIndex=(currentSongIndex+1)%library.size();
        ui->music_list->setCurrentRow(currentSongIndex);
        mediaPlayer->setMedia(library[currentSongIndex]);
        mediaPlayer->play();
    }

}

void Anla::prev(){
    if(!library.isEmpty()){
        currentSongIndex=(currentSongIndex-1+library.size())%library.size();
        ui->music_list->setCurrentRow(currentSongIndex);
        mediaPlayer->setMedia(library[currentSongIndex]);
        mediaPlayer->play();
    }

}

void Anla::searchSong(){
    QString searchText = ui->search_bar->text().toLower();
      for (int i = 0; i < library.size(); ++i) {
          QString fileName = QFileInfo(library[i].toLocalFile()).fileName().toLower();
          if (fileName.contains(searchText)) {
              ui->music_list->setCurrentRow(i);
              currentSongIndex = i;
              mediaPlayer->setMedia(library[currentSongIndex]);
              mediaPlayer->play();
              return;
          }
      }

}
void Anla::resetLibrary(){
    ui->music_list->clear();
    song_label->setText("");
    library.clear();

}
void Anla::currentSong(){
    QUrl song_url(library[currentSongIndex]);
    QString song_name=song_url.fileName();
    song_label->setText(QString("Lecture de : ")+ song_name);
}

