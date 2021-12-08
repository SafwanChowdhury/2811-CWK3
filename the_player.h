//
// Created by twak on 11/11/2019.
//

#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H


#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QTimer>
#include <QSlider>				  

class ThePlayer : public QMediaPlayer {

Q_OBJECT

private:
    std::vector<TheButtonInfo>* infos;
    std::vector<TheButton*>* buttons;
    QTimer* mTimer;
    long updateCount = 0;
public:
    QSlider *p_slider = new QSlider();
    QString s_result;
    ThePlayer() : QMediaPlayer(NULL) {
        setVolume(10); // be slightly less annoying
        connect (this, SIGNAL (stateChanged(QMediaPlayer::State)), this, SLOT (playStateChanged(QMediaPlayer::State)) );

        mTimer = new QTimer(NULL);
        mTimer->setInterval(1); // 1000ms is one second between ...
        mTimer->start();
		connect(this,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
        connect(this,SIGNAL(durationChanged(qint64)),this,SLOT(durationChanged(qint64)));																				 																					 
        connect( mTimer, SIGNAL (timeout()), SLOT ( shuffle() ) ); // ...running shuffle method
    }

    // all buttons have been setup, store pointers here
    void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);
	int getSlider();				

private slots:

    // change the image and video for one button every one second
    void shuffle();

    void playStateChanged (QMediaPlayer::State ms);

public slots:

    // start playing this ButtonInfo
    void jumpTo (TheButtonInfo* button);
	void seek(int);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);			   					 
    void fstfwrd();
    void rwnd();
    void switchState();
    void search(const QString &text);
signals:
	void posChanged(int);
    void durChanged(int);
};

#endif //CW2_THE_PLAYER_H
