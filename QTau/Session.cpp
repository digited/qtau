#include "QTau/Session.h"
#include "QTau/Utils.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>


qtauSession::qtauSession(QObject *parent) :
    qtauEventManager(parent)
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


void qtauSession::applyEvent_NoteMoved(const qtauEvent_NoteMove &event)
{
    const qtauEvent_NoteMove::noteMoveVector &changeset = event.getMoved();

    foreach (const qtauEvent_NoteMove::noteMoveData &change, changeset)
    {
        ust_note *n = noteMap[change.id];

        if (event.isForward())
        {
            n->pulseOffset += change.pulseOffset;
            n->keyNumber   =  change.keyNumber;
        }
        else
        {
            n->pulseOffset -= change.pulseOffset;
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
    // TODO:
}

//--------- callbacks -----------------------------
void qtauSession::onUIEvent(qtauEvent *e)
{
    if (e)
    {
        switch (e->type())
        {
        case ENoteEvents::add:
        {
            qtauEvent_NoteAddition *e = static_cast<qtauEvent_NoteAddition*>(e);

            if (e) onNoteAdded(*e);
            else   vsLog::e("Session could not convert UI event to noteAdd");

            break;
        }
        case ENoteEvents::move:
        {
            qtauEvent_NoteMove *e = static_cast<qtauEvent_NoteMove*>(e);

            if (e) onNoteMoved(*e);
            else   vsLog::e("Session could not convert UI event to noteMove");

            break;
        }
        case ENoteEvents::resize:
        {
            qtauEvent_NoteResize *e = static_cast<qtauEvent_NoteResize*>(e);

            if (e) onNoteResized(*e);
            else   vsLog::e("Session could not convert UI event to noteResize");

            break;
        }
        case ENoteEvents::text:
        {
            qtauEvent_NoteText *e = static_cast<qtauEvent_NoteText*>(e);

            if (e) onNoteLyrics(*e);
            else   vsLog::e("Session could not convert UI event to noteText");

            break;
        }
        case ENoteEvents::effect:
        {
            qtauEvent_NoteEffect *e = static_cast<qtauEvent_NoteEffect*>(e);

            if (e) onNoteEffects(*e);
            else   vsLog::e("Session could not convert UI event to noteEffect");

            break;
        }
        default:
            vsLog::e(QString("Session received unknown event type from UI").arg(e->type()));
        }
    }
    else
        vsLog::e("Session receved a null event from UI");
}


void qtauSession::onNoteAdded(const qtauEvent_NoteAddition &event)
{
    if (!event.isForward())
    {
        vsLog::e("Session got reverse NoteAdded event, skipping");
        return;
    }

    const qtauEvent_NoteAddition::noteAddVector &changeset = event.getAdded();

    if (!changeset.isEmpty())
    {
        bool valid = true;
        QVector<ust_note>   changeData;
        QMap<quint64, bool> newIDs;

        // need to check changeset for sanity, it'll be too late after it has been applied to data
        foreach (const qtauEvent_NoteAddition::noteAddData &change, changeset)
        {
            // TODO: checking id maps may not work with reverse events!
            valid &= change.pulseOffset >= 0     && change.pulseOffset < 10000000 &&
                     change.pulseLength  > 0     && change.pulseLength < 100000   &&
                     change.keyNumber > 0        && change.id > 0                 &&
                     !noteMap.contains(change.id)&& !change.lyrics.isEmpty();

            if (valid)
                newIDs[change.id] = true;
            else
            {
                vsLog::e("Session onNoteAdded got invalid changeset data: " + change.toString());
                break;
            }
        }

        if (valid)
        {
            applyEvent_NoteAdded(event); // apply to data
            storeEvent(&event);             // add to stack
        }
    }
    else
        vsLog::e("Session got empty noteadd changeset, ignoring...");
}


void qtauSession::onNoteDeleted(const qtauEvent_NoteAddition &event)
{
    if (event.isForward())
    {
        vsLog::e("Session got forward NoteDeleted event, skipping");
        return;
    }

    const qtauEvent_NoteAddition::noteAddVector &changeset = event.getAdded();

    if (!changeset.isEmpty())
    {
        bool valid = true;
        QMap<quint64, bool> removedIDs;

        // sanity check
        foreach (const qtauEvent_NoteAddition::noteAddData &change, changeset)
        {
            valid &= (noteMap.contains(change.id) && !removedIDs.contains(change.id));

            if (valid)
                removedIDs[change.id] = true;
            else
            {
                vsLog::e("Session onNoteDeleted got invalid changeset data: " + change.toString());
                break;
            }
        }

        if (valid)
        {
/* there are two ways of storing delete event
    1. check if event dataset is precisely equal to data - offset/length/lyrics/effects etc
    2. check only ID, then just drop dataset and fill new one with what's in data for corresponding IDs

    currently using the third way - no checks and hope for the best
*/
            applyEvent_NoteAdded(event);
            storeEvent(&event);
        }
    }
    else
        vsLog::e("Session got empty notedelete changeset, ignoring...");
}

void qtauSession::onNoteMoved(const qtauEvent_NoteMove &event)
{
    if (!event.isForward())
    {
        vsLog::e("Session got reverse NoteMoved event, ignoring");
        return;
    }

    const qtauEvent_NoteMove::noteMoveVector &changeset = event.getMoved();

    if (!changeset.isEmpty())
    {
        bool valid = true;
        QMap<quint64, bool> movedIDs;

        // sanity check
        foreach (const qtauEvent_NoteMove::noteMoveData &change, changeset)
        {
            valid &= (noteMap.contains(change.id) && !movedIDs.contains(change.id));

            if (valid)
                movedIDs[change.id] = true;
            else
            {
                vsLog::e("Session onNoteMoved got invalid changeset data: " + change.toString());
                break;
            }
        }

        if (valid)
        {
            applyEvent_NoteMoved(event);
            storeEvent(&event);
        }
    }
    else
        vsLog::e("Session got empty notemove changeset, ignoring...");
}

void qtauSession::onNoteLyrics(const qtauEvent_NoteText &event)
{
    if (!event.isForward())
    {
        vsLog::e("Session got reverse NoteLyrics event, ignoring");
        return;
    }

    const qtauEvent_NoteText::noteTextVector &changeset = event.getText();

    if (!changeset.isEmpty())
    {
        bool valid = true;
        QMap<quint64, bool> textIDs;

        // sanity check
        foreach (const qtauEvent_NoteText::noteTextData &change, changeset)
        {
            valid &= (noteMap.contains(change.id) && !textIDs.contains(change.id));

            if (valid)
                textIDs[change.id] = true;
            else
            {
                vsLog::e("Session onNoteLyrics got invalid changeset data: " + change.toString());
                break;
            }
        }

        if (valid)
        {
            applyEvent_NoteLyrics(event);
            storeEvent(&event);
        }
    }
    else
        vsLog::e("Session got empty notelyrics changeset, ignoring...");
}

void qtauSession::onNoteEffects(const qtauEvent_NoteEffect &event)
{
    vsLog::d("Session shouldn't receive noteEffect events yet, how did that happen?..");
}
