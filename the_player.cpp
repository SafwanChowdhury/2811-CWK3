//
// Created by twak on 11/11/2019.
//

#include "the_player.h"

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
}

// change the image and video for one button every one second
void ThePlayer::shuffle() {
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
//        setMedia(*i->url);
    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            play(); // starting playing again...
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();
}

void ThePlayer::seek(int seconds)
{
    this->setPosition(seconds * 1000);
}

void ThePlayer::positionChanged(qint64 position)
{
    p_slider->setValue(position/1000);
    emit posChanged(position/1000);
}

void ThePlayer::durationChanged(qint64 duration)
{
    emit durChanged(duration/1000);
}

int ThePlayer::getSlider()
{
    return p_slider->value();
}
void ThePlayer::fstfwrd(){
    qint64 v_position = this->position();
    v_position = v_position + 5000;
    this->setPosition(v_position);
}

void ThePlayer::rwnd(){
    qint64 v_position = this->position();
    v_position = v_position - 5000;
    this->setPosition(v_position);
}

void ThePlayer::switchState() {

    if(state() == StoppedState) play();

    else if(state() == PlayingState) pause();

    else if(state() == PausedState) play();

}
