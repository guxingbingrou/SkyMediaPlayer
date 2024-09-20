//
// Created by zou on 2024/9/20.
//

#ifndef SKYMEDIAPLAYER_MEDIAPLAYEROBSERVER_H
#define SKYMEDIAPLAYER_MEDIAPLAYEROBSERVER_H

class MediaPlayerObserver{
public:
    virtual void OnSizeChanged(int width, int height) = 0;
};

#endif //SKYMEDIAPLAYER_MEDIAPLAYEROBSERVER_H
