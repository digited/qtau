#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QMap>

#include "tools/utauloid/ust.h"
#include "editor/NoteEvents.h"

class qtauAudioSource;


/** Work session that contains one voice setup (notes/lyrics/effects)
 and voicebank selection+setup to synthesize one song.
 */
class qtauSession : public qtauEventManager
{
    Q_OBJECT

public:
    explicit qtauSession(QObject *parent = 0);
    ~qtauSession();

    bool loadUST(QString fileName);

    void setSynthesizedVocal(qtauAudioSource &s);
    void setBackgroundAudio (qtauAudioSource &s);

    QStringList ustStrings(bool selectionOnly = false);
    QByteArray  ustBinary();
    const ust&  ustRef();

    QString documentName() { return docName; }
    QString documentFile() { return filePath; }

    void setDocName(const QString &name);
    void setFilePath(const QString &fp);

    bool isSessionEmpty()    const { return noteMap.isEmpty(); } /// returns true if doesn't contain any data
    bool isSessionModified() const { return isModified; }        /// if has changes from last save/load

    void setModified(bool m);
    void setSaved(); // if doc was saved at this point

    typedef enum {
        nothingToPlay = 0,
        stopped,
        playing,
        paused
    } EPlayerState;

    EPlayerState playerState() const { return playSt; }

    qtauAudioSource* getVocal() { return vocal; } // gotta be careful with pointers since they'll change on synth/drop
    qtauAudioSource* getMusic() { return music; }
    qtauAudioSource* getAudio(); // returns whatever is available, or a mixer if both are set

signals:
    void modifiedStatus(bool); /// if document is modified
    void undoStatus    (bool); /// if can undo last stored action
    void redoStatus    (bool); /// if can apply previously reverted action

    void dataReloaded();       /// when data is changed completely

    void vocalSet(); // when session gets synthesized audio from score
    void musicSet(); // when user adds bg (off-vocal?) music to play with synthesized vocals
    void playbackTick(qint64 mcsecs);
    void playbackFinished();

public slots:
    void onUIEvent(qtauEvent *);
    void onPlaybackTick(qint64 mcsecs);
    void onPlaybackFinished();

protected:
    bool parseUSTStrings(QStringList ustStrings);
    QString filePath;
    QString docName;
    bool    isModified;
    bool    hadSavePoint; // if was saved having a non-empty event stack

    qtauAudioSource *vocal;
    qtauAudioSource *music;

    QMap<qint64, ust_note> noteMap; // need to store copies until changing data structure to something better
    ust data; // TODO: nite vector inside is obviously unsuitable, needs changing to something else

    void applyEvent_NoteAdded  (const qtauEvent_NoteAddition &event);
    void applyEvent_NoteMoved  (const qtauEvent_NoteMove     &event);
    void applyEvent_NoteResized(const qtauEvent_NoteResize   &event);
    void applyEvent_NoteLyrics (const qtauEvent_NoteText     &event);
    void applyEvent_NoteEffects(const qtauEvent_NoteEffect   &event);

    void stackChanged();

    EPlayerState playSt;
};

#endif // SESSION_H
