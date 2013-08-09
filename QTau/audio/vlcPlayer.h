#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>

struct libvlc_instance_t;
struct libvlc_media_player_t;
struct libvlc_media_t;
struct libvlc_event_manager_t;

typedef struct libvlc_event_t     VLCEvent;
typedef void   (*VLCEventHandler) (const VLCEvent *, void *);


class qtauPlayer : public QObject
{
    Q_OBJECT

    int  _volume;
    bool _muted;

    libvlc_instance_t      *_inst;
    libvlc_media_player_t  *_player;
    libvlc_media_t         *_media;
    libvlc_event_manager_t *_eventManager;

    VLCEventHandler        _eventHandler;

public:
    explicit qtauPlayer(QObject *parent = 0);
    ~qtauPlayer();

    void SetEventHandler(VLCEventHandler event, void* pUserData);

    bool open(QString filename);
    long length();              // in ms
    long position();            // current playback position, in ms

    bool play(long offset = 0); // offset in ms
    void pause();
    void stop();

    void volume(int v); // 0..100 percent
    void mute(bool m = true);
    int  volume();      // 0..100 percent
    bool muted();

signals:
    void statusChange(QString str);                     //emits time --:--
    void playingMetadata(QString str,QString filename); //emits stream title for radio station
    void stopped();                                     //emitted if end reached
};


#endif // AUDIOPLAYER_H
