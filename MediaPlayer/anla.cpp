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
    song_list = ui->song_list;

    volume_label = ui->volume_label;
    int initVolume=mediaPlayer->volume();
    volume_label->setText(QString::number(initVolume));

    progression_label = ui->progression_label;
    progression_label->setText("00:00");

    if (initializeDatabase()) {
            libraryDatabase();
        } else {
            qDebug() << "Échec de l'initialisation de la base de données.";
        }

    connect(ui->actionOpen_File,&QAction::triggered,this,&Anla::openFile);
    connect(ui->play_button,&QPushButton::clicked,this,&Anla::play);
    connect(ui->pause_button,&QPushButton::clicked,this,&Anla::pause);
    connect(ui->stop_button,&QPushButton::clicked,this,&Anla::stop);
    connect(ui->next_button,&QPushButton::clicked,this,&Anla::next);
    connect(ui->prev_button,&QPushButton::clicked,this,&Anla::prev);
    connect(ui->reset_button,&QPushButton::clicked,this,&Anla::resetLibrary);
    connect(ui->random_button,&QPushButton::clicked,this,&Anla::random);
    connect(ui->Add_Button, &QPushButton::clicked, this, &Anla::addToLibrary);
    connect(ui->play_all_button, &QPushButton::clicked, this, &Anla::playPlaylist);
    connect(ui->search_bar,&QLineEdit::textEdited,this,&Anla::searchSong);
    connect(ui->volumeSlider,&QSlider::valueChanged,this,&Anla::setVolume);
    connect(ui->progression,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);
    connect(mediaPlayer,&QMediaPlayer::positionChanged,ui->progression,&QSlider::setValue);
    connect(mediaPlayer,&QMediaPlayer::positionChanged,this,&Anla::updateProgression);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, [this](qint64 duration) {
        ui->progression->setMaximum(duration);
    });
    connect(ui->actionOpen_m3uFile, &QAction::triggered, this, &Anla::openM3uFile);
    connect(ui->actionSave_M3uFile, &QAction::triggered, this, &Anla::saveM3uFile);

    connect(ui->music_list,&QListWidget::currentRowChanged,this,[this](int row){
        if(row>=0 && row< library.size()){
            currentSongIndex=row;
            mediaPlayer->setMedia(library[currentSongIndex]);
            /*mediaPlayer->play();*/
            currentSong(library[currentSongIndex]);

        }
    } );


    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && currentPlaylistIndex != -1) {
            playNextInPlaylist();
        }
    });
}


Anla::~Anla()
{
    delete ui;
}


void Anla::openFile(){
    QStringList files = QFileDialog::getOpenFileNames(this, "Open Audio File", "", "Audio Files (*.mp3 *.wav *.ogg);;All Files(*)");
    for (const QString &file : files) {
            QUrl url = QUrl::fromLocalFile(file);
            playlist.append(url);
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(file).fileName());
            ui->song_list->addItem(item);

        }
}



void Anla::play(){
    if (mediaPlayer->mediaStatus()==QMediaPlayer::NoMedia){
        openFile();
    }else{
        qDebug() << "Lecture de la chanson:" << library[currentSongIndex].toString();
        mediaPlayer->play();
        currentSong(library[currentSongIndex]);

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
    volume_label->setText(QString::number(volume));
}


void Anla::setPosition(quint64 position){
    ui->progression->setValue(position);
    mediaPlayer->setPosition(position);
}


void Anla::updateProgression(qint64 position){
    int seconds = (position / 1000) % 60;
    int minutes = (position / 60000) % 60;
    progression_label->setText(QString::asprintf("%02d:%02d", minutes, seconds));
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


void Anla::searchSong() {
    QString searchText = ui->search_bar->text().toLower();


    for (int i = 0; i < library.size(); ++i) {
        QString fileName = QFileInfo(library[i].toLocalFile()).fileName().toLower();


        if (fileName.contains(searchText)) {
            ui->music_list->setCurrentRow(i);
            currentSongIndex = i;
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


void Anla::currentSong(const QUrl &songUrl){
    QString songName = QFileInfo(songUrl.toLocalFile()).fileName();
    song_label->setText(QString("Lecture de : ") + songName);
}



void Anla::openM3uFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Playlist", "", "Playlists (*.m3u);;All Files (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QTextStream in(&file);
    playlist.clear();
    ui->song_list->clear();
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.startsWith("#")) {
            line=line.trimmed();

            if (line.startsWith("\"") && line.endsWith("\"")) {
                        line = line.mid(1, line.length() - 2);
            }
            QUrl url = QUrl::fromLocalFile(line);
            playlist.append(url);
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(line).fileName());
            ui->song_list->addItem(item);
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


bool Anla::initializeDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Anla_music.db");

    if (!db.open()) {
        qDebug() << "Erreur d'ouverture de la base de données:" << db.lastError().text();
        return false;
    }


    QSqlQuery query;
    QString createTable = "CREATE TABLE IF NOT EXISTS songs ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "path TEXT UNIQUE NOT NULL,"
                          "name TEXT NOT NULL)";
    if (!query.exec(createTable)) {
        qDebug() << "Erreur de création de la table:" << query.lastError().text();
        return false;
    }
    return true;
}


void Anla::libraryDatabase() {
    QSqlQuery query("SELECT path, name FROM songs");

    while (query.next()) {
        QString path = query.value(0).toString();
        QString name = query.value(1).toString();

        QUrl url = QUrl::fromLocalFile(path);
        library.append(url);
        ui->music_list->addItem(name);
    }
}


void Anla::saveSongDatabase(const QUrl &songUrl) {
    QString path = songUrl.toLocalFile();
    QString name = QFileInfo(path).fileName();

    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO songs (path, name) VALUES (:path, :name)");
    query.bindValue(":path", path);
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Erreur d'insertion dans la base de données:" << query.lastError().text();
    }
}


void Anla::random() {
    if (!library.isEmpty()) {
        int newSongIndex;

        do {
            newSongIndex = QRandomGenerator::global()->bounded(library.size());
        } while (newSongIndex == currentSongIndex);
        currentSongIndex = newSongIndex;
        mediaPlayer->setMedia(library[currentSongIndex]);
        mediaPlayer->play();
        currentSong(library[currentSongIndex]);
    }
}


void Anla::addToLibrary(){
    for (const QUrl &url : playlist) {
            library.append(url);
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(url.toLocalFile()).fileName());
            ui->music_list->addItem(item);
            saveSongDatabase(url);
        }
}




/*void Anla::searchInPlaylist(){
    QString searchText = ui->search_song_list->text().toLower();

    if (searchText.isEmpty()) {
        ui->song_list->clearSelection();
        return;
    }

    for (int i = 0; i < playlist.size(); ++i) {
        QString fileName = QFileInfo(playlist[i].toLocalFile()).fileName().toLower();


        if (fileName.contains(searchText)) {
            ui->song_list->setCurrentRow(i);
            currentPlaylistIndex = i;
            return;
        }
    }
}*/


void Anla::playPlaylist() {
    if (!playlist.isEmpty()) {
        currentPlaylistIndex = 0;
        mediaPlayer->setMedia(playlist[currentPlaylistIndex]);
        mediaPlayer->play();
        currentSong(playlist[currentPlaylistIndex]);
    }
}


void Anla::playNextInPlaylist() {
    if (currentPlaylistIndex + 1 < playlist.size()) {
        ++currentPlaylistIndex;
        mediaPlayer->setMedia(playlist[currentPlaylistIndex]);
        mediaPlayer->play();
        currentSong(playlist[currentPlaylistIndex]);
    } else {
        currentPlaylistIndex = -1;
    }
}

