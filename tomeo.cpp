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
#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>
#include <QAudio>


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
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
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

int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

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


    QLineEdit *search_text = new QLineEdit();
    QPushButton *search_button = new QPushButton(); // Button
    QLabel *l_search = new QLabel(); // Text inside
    l_search->setText("Search/Filter");
    search_text->setPlaceholderText("Enter Text");
    search_button->setText("Search");


    //Add widgets to layout
    search_layout->addWidget(l_search);
    search_layout->addWidget(search_text);
    search_layout->addWidget(search_button);

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





    // create the four buttons
    for ( int i = 0; i < 4; i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);

    // create the main window and layout
    QWidget window;

    // These are the two sides of the screen that are later combined in the 'top' layout
    QVBoxLayout *left = new QVBoxLayout(); //left side of screen
    QVBoxLayout *right = new QVBoxLayout(); // right side of screen
    QHBoxLayout *video_buttons = new QHBoxLayout();// buttons for media player



    // Button to play/pause the video
    QPushButton *b_play_pause = new QPushButton();
    b_play_pause->connect(b_play_pause,SIGNAL(clicked()),player,SLOT(switchState()));
    b_play_pause->setText("Play/Pause");
    video_buttons->addWidget(b_play_pause);


    // Button to rewind video 5 seconds
    QPushButton *b_rewind = new QPushButton();
    b_rewind->setText("Rewind");
    video_buttons->addWidget(b_rewind);
    // Button to fastforward video 5 seconds
    QPushButton *b_fastforward = new QPushButton();
    b_fastforward->setText("Fastforward");
    video_buttons->addWidget(b_fastforward);

    // Widget for media player buttons
    QWidget *Player_w = new QWidget();
    Player_w->setLayout(video_buttons);

    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // This is the top layout, where the left and right side layouts will be combined
    QHBoxLayout *top = new QHBoxLayout();

    // Add the search bar, search button and the 4 videos to the left side layout
    left->addWidget(search_widget);
    left->addWidget(buttonWidget);

    // Add stretch means results stay near the top of the screen rather than spacing evenly
    left->addStretch(1);

    // Right side of the screen is just the video widget for now

    right->addWidget(videoWidget);
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

    window.setLayout(top);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
