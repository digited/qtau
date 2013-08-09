#include "QTau/Session.h"
#include "QTau/Utils.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>


qtauSession::qtauSession(QObject *parent) :
    qtauEventManager(parent), docName(tr("Untitled")), isModified(false)
{
    data.clear();
}

qtauSession::~qtauSession()
{
    //
}

qtauEvent_NoteAddition *util_makeAddNotesEvent(const ust &u)
{
    qtauEvent_NoteAddition::noteAddVector changeset;

    for (int i = 0; i < u.notes.size(); ++i)
    {
        const ust_note &n = u.notes[i];
        qtauEvent_NoteAddition::noteAddData d;
        d.id     = i+1;
        d.lyrics = n.lyric;

        d.pulseLength = n.pulseLength;
        d.pulseOffset = n.pulseOffset;
        d.keyNumber   = n.keyNumber;

        changeset.append(d);
    }

    return new qtauEvent_NoteAddition(changeset);
}

//-------------------------------------------

bool qtauSession::loadUST(QString fileName)
{
    bool result = false;
    QFile ustFile(fileName);

    if (ustFile.open(QFile::ReadOnly))
    {
        QStringList ustStrings;
        QTextStream reader(&ustFile);

        while (!reader.atEnd())
            ustStrings << reader.readLine();

        if (!ustStrings.isEmpty())
        {
            ust tmp_u = ustFromStrings(ustStrings);

            if (tmp_u.notes.size() > 0)
            {
                vsLog::s("Successfully loaded " + fileName);

                clearHistory(); // or make a delete event, settings change event, filepath change event
                data = tmp_u;
                docName = fileName;
                qtauEvent_NoteAddition *loadNotesChangeset = util_makeAddNotesEvent(data);

                emit dataReloaded();
                emit onEvent(loadNotesChangeset);

                delete loadNotesChangeset;
                result = true;
            }
            else
                vsLog::e("Could not get any notes from " + fileName);
        }
        else
            vsLog::e("Could not read text lines from " + fileName);

        ustFile.close();
    }
    else
        vsLog::e("Could not open " + fileName);

    return result;
}


QStringList qtauSession::ustStrings(bool selectionOnly) { return ustToStrings(data); }
QByteArray  qtauSession::ustBinary()                    { return ustToBytes(data); }
const ust&  qtauSession::ustRef() const                 { return data; }

void qtauSession::setDocName(const QString &name)
{
    if (name.isEmpty())
        vsLog::e("Shouldn't set empty doc name for session! Ignoring...");
    else
        docName = name;
}

void qtauSession::setFilePath(const QString &fp)
{
    if (fp.isEmpty())
        vsLog::e("Shouldn't set empty filepath for session! Ignoring...");
    else
        filePath = fp;
}

//----- inner data functions -----------------------------
void qtauSession::applyEvent_NoteAdded(const qtauEvent_NoteAddition &event)
{
    const qtauEvent_NoteAddition::noteAddVector &changeset = event.getAdded();

    if (event.isForward())
    {
        foreach (const qtauEvent_NoteAddition::noteAddData &change, changeset)
        {
            data.notes.append(ust_note(change.id, change.lyrics,
                                       change.pulseOffset, change.pulseLength, change.keyNumber));
            noteMap[change.id] = &data.notes.last();
        }
    }
    else
        foreach (const qtauEvent_NoteAddition::noteAddData &change, changeset)
        {
            noteMap.remove(change.id);
            data.removeID(change.id);
        }
}


void qtauSession::applyEvent_NoteResized(const qtauEvent_NoteResize &event)
{
    const qtauEvent_NoteResize::noteResizeVector &changeset = event.getResized();

    foreach (const qtauEvent_NoteResize::noteResizeData &change, changeset)
    {
        ust_note *n = noteMap[change.id];

        if (event.isForward())
        {
            n->pulseOffset = change.offset;
            n->pulseLength = change.length;
        }
        else
        {
            n->pulseOffset = change.prevOffset;
            n->pulseLength = change.prevLength;
        }
    }
}


void qtauSession::applyEvent_NoteMoved(const qtauEvent_NoteMove &event)
{
    const qtauEvent_NoteMove::noteMoveVector &changeset = event.getMoved();

    foreach (const qtauEvent_NoteMove::noteMoveData &change, changeset)
    {
        ust_note *n = noteMap[change.id];

        if (event.isForward())
        {
            n->pulseOffset += change.pulseOffDelta;
            n->keyNumber   =  change.keyNumber;
        }
        else
        {
            n->pulseOffset -= change.pulseOffDelta;
            n->keyNumber   =  change.prevKeyNumber;
        }
    }
}


void qtauSession::applyEvent_NoteLyrics(const qtauEvent_NoteText &event)
{
    const qtauEvent_NoteText::noteTextVector &changeset = event.getText();

    foreach (const qtauEvent_NoteText::noteTextData &change, changeset)
        if (event.isForward()) noteMap[change.id]->lyric = change.txt;
        else                   noteMap[change.id]->lyric = change.prevTxt;
}


void qtauSession::applyEvent_NoteEffects(const qtauEvent_NoteEffect &event)
{
    // TODO: or not to do, that is the question
}

//--------- dispatcher -----------------------------
void qtauSession::onUIEvent(qtauEvent *e)
{
    if (e)
    {
        switch (e->type())
        {
        case ENoteEvents::add:
        {
            qtauEvent_NoteAddition *ne = static_cast<qtauEvent_NoteAddition*>(e);

            if (ne)
            {
                applyEvent_NoteAdded(*ne);
                storeEvent(ne);
            }
            else
                vsLog::e("Session could not convert UI event to noteAdd");

            break;
        }
        case ENoteEvents::move:
        {
            qtauEvent_NoteMove *ne = static_cast<qtauEvent_NoteMove*>(e);

            if (ne)
            {
                applyEvent_NoteMoved(*ne);
                storeEvent(ne);
            }
            else
                vsLog::e("Session could not convert UI event to noteMove");

            break;
        }
        case ENoteEvents::resize:
        {
            qtauEvent_NoteResize *ne = static_cast<qtauEvent_NoteResize*>(e);

            if (e)
            {
                applyEvent_NoteResized(*ne);
                storeEvent(ne);
            }
            else
                vsLog::e("Session could not convert UI event to noteResize");

            break;
        }
        case ENoteEvents::text:
        {
            qtauEvent_NoteText *ne = static_cast<qtauEvent_NoteText*>(e);

            if (ne)
            {
                applyEvent_NoteLyrics(*ne);
                storeEvent(ne);
            }
            else
                vsLog::e("Session could not convert UI event to noteText");

            break;
        }
        case ENoteEvents::effect:
        {
            qtauEvent_NoteEffect *ne = static_cast<qtauEvent_NoteEffect*>(e);

            if (ne)
            {
                applyEvent_NoteEffects(*ne);
                storeEvent(ne);
            }
            else
                vsLog::e("Session could not convert UI event to noteEffect");

            break;
        }
        default:
            vsLog::e(QString("Session received unknown event type from UI").arg(e->type()));
        }

        delete e; // NOTE: it is copied on storing here, so someone somewhere should delete it
    }
    else
        vsLog::e("Session receved a null event from UI");
}

void qtauSession::stackChanged()
{
    emit undoStatus(canUndo());
    emit redoStatus(canRedo());
    emit modifiedStatus(isSessionModified());
}
