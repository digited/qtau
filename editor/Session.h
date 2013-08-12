#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QMap>

#include "tools/utauloid/ust.h"
#include "editor/NoteEvents.h"


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

    QStringList ustStrings(bool selectionOnly = false);
    QByteArray  ustBinary();
    const ust&  ustRef();

    QString documentName() { return docName; }

    void setDocName(const QString &name);
    void setFilePath(const QString &fp);

    bool isSessionEmpty()    { return data.notes.isEmpty(); } /// returns true if doesn't contain any data
    bool isSessionModified() { return isModified; }           /// if has changes from last save/load

    void setModified(bool m) { isModified = m; }

signals:
    void modifiedStatus(bool); /// if document is modified
    void undoStatus    (bool); /// if can undo last stored action
    void redoStatus    (bool); /// if can apply previously reverted action

    void dataReloaded();       /// when data is changed completely

public slots:
    void onUIEvent(qtauEvent *);

protected:
    bool parseUSTStrings(QStringList ustStrings);
    QString filePath;
    QString docName;
    bool    isModified;

    QMap<qint64, ust_note> noteMap; // need to store copies until changing data structure to something better
    ust data; // TODO: nite vector inside is obviously unsuitable, needs changing to something else

    void applyEvent_NoteAdded  (const qtauEvent_NoteAddition &event);
    void applyEvent_NoteMoved  (const qtauEvent_NoteMove     &event);
    void applyEvent_NoteResized(const qtauEvent_NoteResize   &event);
    void applyEvent_NoteLyrics (const qtauEvent_NoteText     &event);
    void applyEvent_NoteEffects(const qtauEvent_NoteEffect   &event);

    void stackChanged();
};

#endif // SESSION_H