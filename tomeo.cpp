//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//  2811 cw3 : twak 11/11/2021
//  Initial Commit

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"

// Includes added later
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSlider>
#include <QProgressBar>
#include <QFile>
std::vector<QString> titles;
//QString path;

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();



            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
//                std::string fString = f.toStdString();
//                int lastSlash = fString.find_last_of("/");
//                std::string reverse = fString.substr(lastSlash+1, fString.length());
//                QString reverseQ = QString::fromStdString(reverse.substr(0, reverse.find_first_of(".")));
//                titles.push_back(reverseQ);
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
//                std::string fString = f.toStdString();
//                int lastSlash = fString.find_last_of("/");
//                std::string reverse = fString.substr(lastSlash+1, fString.length());
//                QString reverseQ = QString::fromStdString(reverse.substr(0, reverse.find_first_of(".")));
//                titles.push_back(reverseQ);
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                        std::string fString = f.toStdString();
                        int lastSlash = fString.find_last_of("/");
                        std::string reverse = fString.substr(lastSlash+1, fString.length());
                        QString reverseQ = QString::fromStdString(reverse.substr(0, reverse.find_first_of(".")));
                        titles.push_back(reverseQ);
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}


//sets the layout of the search text, button and filter

void set_search_layout(QHBoxLayout * sl){
    QLineEdit *search_text = new QLineEdit();
    QPushButton *search_button = new QPushButton();
    QLabel *l_search = new QLabel();
    l_search->setText("Search/Filter");
    search_text->setPlaceholderText("Enter Text");
    search_button->setText("Search");

    //Add widgets to layout
    sl->addWidget(l_search);
    sl->addWidget(search_text);
    sl->addWidget(search_button);
}

QSlider * set_volume_layout(QHBoxLayout * vl , ThePlayer * player){
    QSlider *volume = new QSlider(Qt::Horizontal);
    volume->setMaximum(100);
    volume->setMinimum(0);
    volume->setValue(player->volume());
    QLabel *vol_label = new QLabel();
    vol_label->setText("Current Volume:");

    QLabel *vol_info = new QLabel();
    vol_info->setNum(player->volume());

    volume->connect(volume, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));
    vol_info->connect(volume, SIGNAL(valueChanged(int)), vol_info, SLOT(setNum(int)));

    vl->addWidget(vol_label);
    vl->addWidget(vol_info);
    return volume;
}

void set_playback_layout(QHBoxLayout * pl, ThePlayer * player){
    QPushButton *b_play_pause = new QPushButton();
    b_play_pause->connect(b_play_pause,SIGNAL(clicked()),player,SLOT(switchState()));
    b_play_pause->setText("Play/Pause");

    // Button to rewind video 5 seconds
    QPushButton *b_rewind = new QPushButton();
    b_rewind->setText("Rewind");
    b_rewind->connect(b_rewind, SIGNAL(clicked()), player, SLOT(rwnd()));
    pl->addWidget(b_rewind);
    // Button to fastforward video 5 seconds
    QPushButton *b_fastforward = new QPushButton();
    b_fastforward->setText("Fast Forward");
    b_fastforward->connect(b_fastforward, SIGNAL(clicked()), player, SLOT(fstfwrd()));
    //add play pause to the middle
    pl->addWidget(b_play_pause);
    pl->addWidget(b_fastforward);
}


std::vector<int> search_results(QString sQuery) {
    std::vector<int> indices;
    for (int i = 0; i < titles.size(); i++ ) {
        if (titles.at(i).contains(sQuery)) {
            indices.push_back(i);
        }
    }

    //if no videos match: repopulate indices
    //todo: create pop up
    if (indices.size() == 0){
        for (int i = 0; i < titles.size(); i++){
            indices.push_back(i);
        }
    }
    return indices;
}



int main(int argc, char *argv[]) {
    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    QFile styleSheetFile("./TomeoStyle.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    app.setStyleSheet(styleSheet);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

   // path = argv[0];
    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {

        const int result = QMessageBox::question(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! download, unzip, and add command line argument to \"quoted\" file location. Download videos from Tom's OneDrive?"),
                    QMessageBox::Yes |
                    QMessageBox::No );

        switch( result )
        {
        case QMessageBox::Yes:
          QDesktopServices::openUrl(QUrl("https://leeds365-my.sharepoint.com/:u:/g/personal/scstke_leeds_ac_uk/EcGntcL-K3JOiaZF4T_uaA4BHn6USbq2E55kF_BTfdpPag?e=n1qfuN"));
          break;
        default:
            break;
        }
        exit(-1);
    }


    // Create widget of search field and button to accompany it
    QHBoxLayout *search_layout = new QHBoxLayout();
    //set the layout of the search
    set_search_layout(search_layout);
    // Create a widget of the layout made above
    QWidget *search_widget = new QWidget();
    search_widget->setLayout(search_layout);

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // the cont QMediaPlayer whichrols the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // These buttons are now arranged vertically
    QVBoxLayout *layout = new QVBoxLayout();
    buttonWidget->setLayout(layout);


    // creates a button for each video
    //currently this creates 6 buttons (video g doesn't seem to work)


    // Iteration 2 - volume stuff
    // Volume slider to control volume of video playback
    // Label vol_info that updates relative to current volume set
    QHBoxLayout *vol_layout = new QHBoxLayout();
    QSlider * volume = set_volume_layout(vol_layout, player);
    QWidget *vol_layout_widget = new QWidget();
    vol_layout_widget->setLayout(vol_layout);

    std::vector<TheButtonInfo> searchVideos;
    std::vector<int> searchIndices = search_results("abc");


    if (searchIndices.size() != 0) {
        for(auto index : searchIndices) {
            TheButton *button = new TheButton(buttonWidget);
            button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
            buttons.push_back(button);
            searchVideos.push_back(videos.at(index));
            layout->addWidget(button);
            //adding a video description label
            QLabel * vidDesc = new QLabel();
            vidDesc->setText(titles.at(index) +"\t 16/11/2020\n");
            vidDesc->setAlignment(Qt::AlignCenter);
            layout->addWidget(vidDesc);
            button->init(&videos.at(index));
        }
    }



    // tell the player what buttons and videos are available
    player->setContent(&buttons, & searchVideos);

    // create the main window and layout
    QWidget window;

    // These are the two sides of the screen that are later combined in the 'top' layout
    QVBoxLayout *left = new QVBoxLayout(); //left side of screen
    QVBoxLayout *right = new QVBoxLayout(); // right side of screen
    QHBoxLayout *video_buttons = new QHBoxLayout();// buttons for media player

    //QSlider *p_slider = new QSlider();

    //p_slider->setMaximum()
    player->p_slider->setOrientation(Qt::Horizontal);
    //player->p_slider->setRange(0,19);
    player->p_slider->connect(player->p_slider,SIGNAL(sliderMoved(int)),player,SLOT(seek(int)));
    player->p_slider->connect(player,SIGNAL(posChanged(int)),player->p_slider,SLOT(setValue(int)));
    //p_slider->connect(player,SIGNAL(durChanged(int)),p_slider,SLOT(scaleSlider(int)));

    // syntax for connect(first_widget/object -> SIGNAL(signal_it_emits()) -> object_widget_to_change -> SLOT(function_to_do_something_with_obj/wid2())
    // Button to play/pause the video

    //sets the layut of the playback buttons
    set_playback_layout(video_buttons, player);

    // Widget for media player buttons
    QWidget *Player_w = new QWidget();
    Player_w->setLayout(video_buttons);

    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // This is the top layout, where the left and right side layouts will be combined
    QHBoxLayout *top = new QHBoxLayout();

    // Add the search bar, search button and the 4 videos to the left side layout
    left->addWidget(search_widget);

    //implementing scroll area
    QScrollArea * videoScroll = new QScrollArea();
    videoScroll->setWidget(buttonWidget);
    videoScroll->setAlignment(Qt::AlignHCenter);
    left->addWidget(videoScroll);

    // Add stretch means results stay near the top of the screen rather than spacing evenly
    //removing add stretch allows the scroll area to resize nicely
    //left->addStretch(1);
    left->addWidget(volume);
    left->addWidget(vol_layout_widget);

    // Right side of the screen is just the video widget for now

    right->addWidget(videoWidget);    
    right->addWidget(player->p_slider);
    right->addWidget(Player_w);

    // make these layouts into widgets
    QWidget *left_layout = new QWidget();
    QWidget *right_layout = new QWidget();

    left_layout->setLayout(left);
    right_layout->setLayout(right);

    // Taken from stack overflow link - I dont think this is actually needed.
    top->setContentsMargins( 0, 0, 0, 0 );
    top->setSpacing( 0 );

    // to create a 1:3 screen ratio of left to right, adding left and right to top level
    top->addWidget(left_layout, 25);
    top->addWidget(right_layout, 75);


    //layout for the rest of the page
    QWidget * topWidget = new QWidget();
    topWidget->setLayout(top);
    //final layout
    QVBoxLayout * screenLayout = new QVBoxLayout();
    screenLayout->addWidget(topWidget, 85);


    window.setLayout(screenLayout);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
