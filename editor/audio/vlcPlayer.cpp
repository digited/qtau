#include "editor/audio/File.h"
#include <vlc/vlc.h>
#include <QDebug>

// NOTE: unused currently, thus outdated

qtauPlayer::qtauPlayer(QObject *parent) :
    QObject(parent), _volume(100), _muted(false),
    _inst(0), _player(0), _media(0),
    _eventManager(0), _eventHandler(0)
{
    const char * const vlc_args[] = {
            "-I", "dummy",     // No special interface
            "--ignore-config", // Don't use VLC's config
            "--plugin-path=./plugins" };

    _inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

    _player       = libvlc_media_player_new          (_inst);
    _eventManager = libvlc_media_player_event_manager(_player);
}

qtauPlayer::~qtauPlayer()
{
    stop();
    libvlc_media_player_release(_player);
    libvlc_release(_inst);
}

//--------------------------------------------------------------

void qtauPlayer::SetEventHandler(VLCEventHandler event, void* pUserData)
{
    // TODO: <--
}

bool qtauPlayer::open(QString filename)
{
    bool result = false;

    if (filename.isEmpty())
    {
        stop();
        qDebug() << "AudioPlayer: empty file name, stopping playback";
    }
    else
    {
        _media = libvlc_media_new_path(_inst, filename.toUtf8().data());

        if (_media != 0)
        {
            _player = libvlc_media_player_new_from_media(_media);

            if (_player != 0)
                result = libvlc_media_player_play(_player) == 0;
        }
    }

    return result;
}

bool qtauPlayer::open(qtauAudio* a)
{
    bool result = false;

    if (a && !a->isEmpty())
    {
        //
    }
}

long qtauPlayer::length()
{
    long result = -1;

    if (_player != 0)
        result = libvlc_media_player_get_time(_player);

    return result;
}

long qtauPlayer::position()
{
    long result = -1;

    if (_player != 0)
        result = libvlc_media_player_get_position(_player);

    return result;
}

bool qtauPlayer::play(long offset)
{
    bool result = false;

    if (_player != 0)
    {
        if (offset > 0)
            libvlc_media_player_set_time(_player, offset);

        result = libvlc_media_player_play(_player);

        if (result != 0)
            qDebug() << "AudioPlayer: vlc media player refuses to play media with code" << result;
    }

    return result;
}

void qtauPlayer::pause()
{
    if (_player != 0)
        libvlc_media_player_pause(_player);
}

void qtauPlayer::stop()
{
    if (_player != 0)
        libvlc_media_player_stop(_player);
}

void qtauPlayer::volume(int v)
{
    if (v < 0)
        v = 0;
    else if (v > 100)
        v = 100;

    _volume = v;

    if (_player != 0)
        libvlc_audio_set_volume(_player, v);
}

void qtauPlayer::mute(bool m)
{
    if (m != _muted)
    {
        _muted = m;

        if (_player != 0)
            libvlc_audio_set_mute(_player, m);
    }
}

int  qtauPlayer::volume() { return _volume; }
bool qtauPlayer::muted()  { return _muted;  }
