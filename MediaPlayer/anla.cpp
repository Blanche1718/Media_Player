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
    connect(ui->actionOpen_m3uFile, &QAction::triggered, this, &Anla::openM3uFile);
    connect(ui->actionSave_M3uFile, &QAction::triggered, this, &Anla::saveM3uFile);

}

Anla::~Anla()
{
    delete ui;
}


void Anla::openFile(){
    QStringList files = QFileDialog::getOpenFileNames(this, "Open Audio File", "", "Audio Files (*.mp3 *.wav *.ogg);;All Files(*)");
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

        mediaPlayer->play();
        currentSong();

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
    currentSongIndex = -1;

}
void Anla::currentSong(){
    if (currentSongIndex >= 0 && currentSongIndex < library.size()) {
           QUrl song_url(library[currentSongIndex]);
           QString song_name = song_url.fileName();
           song_label->setText(QString("Lecture de : ") + song_name);
       } else {
           song_label->setText("");  // Réinitialiser l'affichage si l'index est invalide
       }
}

void Anla::openM3uFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Playlist", "", "Playlists (*.m3u);;All Files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QTextStream in(&file);
    library.clear();
    ui->music_list->clear();
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.startsWith("#")) {
            QUrl url = QUrl::fromLocalFile(line);

            if (line.startsWith("\"") && line.endsWith("\"")) {
                        line = line.mid(1, line.length() - 2);
                    }
            library.append(url);
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(line).fileName());
            ui->music_list->addItem(item);
        }
    }
    file.close();
}

void Anla::saveM3uFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Playlist", "", "Playlists (*.m3u);;All Files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return;

    QTextStream out(&file);
    for (const QUrl &url : qAsConst(library)) {
        out << url.toLocalFile() << "\n";
    }
    file.close();
}

