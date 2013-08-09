#include "QTau/ui/noteEditorHandlers.h"
#include "QTau/ui/noteEditor.h"
#include "QTau/NoteEvents.h"

#include <QLineEdit>

const int CONST_NOTE_RESIZE_CURSOR_MARGIN = 6;


void qtauEdController::changeController(qtauEdController *c) { owner->changeController(c); }
void qtauEdController::eventHappened   (qtauEvent *e)        { owner->eventHappened   (e); }
void qtauEdController::recalcNoteRects()                     { owner->recalcNoteRects();   }
void qtauEdController::lazyUpdate()                          { owner->lazyUpdate();        }


void qtauEdController::onEvent(qtauEvent *e)
{
    reset();

    switch(e->type())
    {
    case ENoteEvents::add:
        onNoteAdd(static_cast<qtauEvent_NoteAddition*>(e));
        break;
    case ENoteEvents::text:
        onNoteText(static_cast<qtauEvent_NoteText*>(e));
        break;
    case ENoteEvents::move:
        onNoteMove(static_cast<qtauEvent_NoteMove*>(e));
        break;
    case ENoteEvents::resize:
        onNoteResize(static_cast<qtauEvent_NoteResize*>(e));
        break;
    case ENoteEvents::effect:
        onNoteEffect(static_cast<qtauEvent_NoteEffect*>(e));
        break;
    default:
        vsLog::e(QString("Editor controller got event of unknown type %1").arg(e->type()));
    }
}

void qtauEdController::deleteSelected()
{
    if (!notes->selected.isEmpty())
    {
        pointedNote = 0;
        qtauEvent_NoteAddition::noteAddVector v;

        foreach (const quint64 &id, notes->selected)
        {
            const qne::editorNote &n = notes->idMap[id];
            qtauEvent_NoteAddition::noteAddData d;
            d.id          = id;
            d.lyrics      = n.txt;
            d.pulseLength = n.pulseLength;
            d.pulseOffset = n.pulseOffset;
            d.keyNumber   = n.keyNumber;
            v.append(d);

            removeFromGrid(n.r.left(),  n.id);
            removeFromGrid(n.r.right(), n.id);
            notes->idMap.remove(id);
        }

        notes->selected.clear();
        lazyUpdate();

        qtauEvent_NoteAddition *event = new qtauEvent_NoteAddition(v, false);
        eventHappened(event);
    }
}

void qtauEdController::mouseDoubleClickEvent(QMouseEvent *event)
{
    unselectAll();

    if (event->button() == Qt::LeftButton && pointedNote && state->editingEnabled)
    {
        notes->selected.append(pointedNote->id);

        QRect r(pointedNote->r);
        r.setSize(QSize(100, setup->note.height())); // TODO: move constants from noteEditor in some common header

        if (state->viewport.contains(r))
            owner->scrollTo(r);

        owner->lazyUpdate();

        owner->changeController(new qtauEd_TextInput(this));
    }
}

void qtauEdController::mousePressEvent(QMouseEvent *event)
{
    if (state->rmbScrollEnabled && event->button() == Qt::RightButton)
    {
        rmbScrollStartPos = event->pos();
        rmbStartVpOffset  = state->viewport.topLeft();
        rmbDragging = true;
        owner->setCursor(Qt::ClosedHandCursor);
    }
    else if (event->button() == Qt::LeftButton)
    {
        absFirstClickPos = event->pos() + state->viewport.topLeft();

        if (state->editingEnabled && owner->cursor().shape() == Qt::SplitHCursor && pointedNote)
        {
            int msPosInNote = event->pos().x() + state->viewport.x() - pointedNote->r.x();
            bool left = (float)msPosInNote / pointedNote->r.width() < 0.5;
            owner->changeController(new qtauEd_ResizeNote(this, left));
        }
    }
}

void qtauEdController::mouseMoveEvent(QMouseEvent *event)
{
    if (rmbDragging)
        owner->rmbScrollHappened(event->pos() - rmbScrollStartPos, rmbStartVpOffset);
    else
    {
        QPoint absPos(event->pos() + state->viewport.topLeft());
        QPoint delta = absPos - absFirstClickPos;

        pointedNote = noteInPoint(absPos);

        if (pointedNote)
        {
            if (event->buttons() & Qt::LeftButton)
            {
                if (abs(delta.x()) > 3 || abs(delta.y()) > 3) // mousepress exceeded accidental slide click margin
                {
                    if (!pointedNote->selected)
                    {
                        unselectAll();
                        notes->selected.append(pointedNote->id);
                    }

                    owner->changeController(new qtauEd_DragNotes(this));
                }
            }
            else if (state->editingEnabled)  // no mousepress, just moving cursor
            {
                // if above edge, show resize cursor
                if (absPos.x() - pointedNote->r.left()  <= CONST_NOTE_RESIZE_CURSOR_MARGIN ||
                    pointedNote->r.right() - absPos.x() <= CONST_NOTE_RESIZE_CURSOR_MARGIN)
                    owner->setCursor(Qt::SplitHCursor);
                else
                    owner->setCursor(Qt::ArrowCursor);
            }
            else // no button pressed, outside any notes
                if (owner->cursor().shape() != Qt::ArrowCursor)
                    owner->setCursor(Qt::ArrowCursor);
        }
        else
        {
            if (owner->cursor().shape() != Qt::ArrowCursor)
                owner->setCursor(Qt::ArrowCursor);

            if (!noteInPoint(absFirstClickPos))
                if (event->buttons() & Qt::LeftButton && (abs(delta.x()) > 3 || abs(delta.y()) > 3))
                {
                    if (state->editingEnabled) owner->changeController(new qtauEd_AddNote(this));
                    else                       owner->changeController(new qtauEd_SelectRect(this));
                }
        }
    }
}

void qtauEdController::mouseReleaseEvent(QMouseEvent *event)
{
    if (rmbDragging)
    {
        rmbDragging = false;
        owner->setCursor(Qt::ArrowCursor);
    }
    else if (pointedNote)
    {
        if (event->modifiers() & Qt::ControlModifier)   // control toggles selection
        {
            if (pointedNote->selected) // unselect
                notes->selected.remove(notes->selected.indexOf(pointedNote->id));
            else                       // select
                notes->selected.prepend(pointedNote->id);

            pointedNote->selected = !pointedNote->selected;
        }
        else if (event->modifiers() & Qt::ShiftModifier) // just add it to selection, if not already
        {
            if (!pointedNote->selected)
            {
                pointedNote->selected = true;
                notes->selected.prepend(pointedNote->id);
            }
        }
        else    // unselect everything except this note
        {
            unselectAll();
            notes->selected.append(pointedNote->id);
            pointedNote->selected = true;
        }
    }
    else
        unselectAll();

    absFirstClickPos = QPoint(-1,-1);
    owner->lazyUpdate();
}

void qtauEdController::onNoteAdd(qtauEvent_NoteAddition *event)
{
    foreach (const qtauEvent_NoteAddition::noteAddData &d, event->getAdded())
    {
        qne::editorNote n;
        n.id  = d.id;
        n.txt = d.lyrics;
        n.pulseOffset = d.pulseOffset;
        n.pulseLength = d.pulseLength;
        n.keyNumber   = d.keyNumber;

        idOffset = qMax(idOffset, d.id);
        notes->idMap[d.id] = n;
    }

    owner->recalcNoteRects();
    owner->lazyUpdate();
}

void qtauEdController::onNoteResize(qtauEvent_NoteResize *event) {}
void qtauEdController::onNoteMove(qtauEvent_NoteMove *event) {}
void qtauEdController::onNoteText(qtauEvent_NoteText *event) {}
void qtauEdController::onNoteEffect(qtauEvent_NoteEffect *event) {}


qne::editorNote* qtauEdController::noteInPoint(const QPoint &p)
{
    qne::editorNote *result = 0;

    int barUnderCursor = p.x() / setup->barWidth;

    if (barUnderCursor < notes->grid.size())
        for (int i = 0; i < notes->grid[barUnderCursor].size(); ++i)
        {
            qne::editorNote &n = notes->idMap[notes->grid[barUnderCursor][i]];

            if (n.r.contains(p))
            {
                result = &n;
                break;
            }
        }

    return result;
}

//========================================================================

qtauEd_TextInput::qtauEd_TextInput(
    qtauNoteEditor &ne, noteSetup &ns, qne::editorNotes &nts, qne::editorState &st) :
    qtauEdController(ne, ns, nts, st) { init(); }

qtauEd_TextInput::qtauEd_TextInput(qtauEdController *c) : qtauEdController(c) { init(); }

qtauEd_TextInput::~qtauEd_TextInput() {}

void qtauEd_TextInput::init()
{
    editingNote = !notes->selected.isEmpty();

    if (editingNote)
    {
        if (!edit)
            edit = new QLineEdit(owner);

        connect(edit, SIGNAL(editingFinished()), SLOT(onEdited()));
        editedNote = pointedNote; // may change when clicked outside editbox, so need to store

        QRect r(pointedNote->r);
        r.moveTo(r.topLeft() - state->viewport.topLeft());

        edit->setGeometry(r);
        edit->setVisible(true);
        edit->setText(editedNote->txt);
        edit->setFocus();
        edit->selectAll();
    }
}

void qtauEd_TextInput::reset()
{
    onEdited();
}

void qtauEd_TextInput::onEdited()
{
    if (editingNote && editedNote)
    {
        editingNote = false;
        QString txt = edit->text();
        disconnect(edit, SIGNAL(editingFinished()), this, SLOT(onEdited()));
        edit->setVisible(false);

        if (txt != editedNote->txt)
        {
            qtauEvent_NoteText::noteTextData d;
            d.prevTxt = editedNote->txt;
            d.txt = txt;
            qtauEvent_NoteText::noteTextVector v;
            v.append(d);
            qtauEvent_NoteText *add = new qtauEvent_NoteText(v);

            editedNote->txt = txt;
            editedNote->cached = false;
            editedNote = 0;

            eventHappened(add);
        }

        lazyUpdate();
        changeController(new qtauEdController(this));
    }
}

//========================================================================

qtauEd_SelectRect::qtauEd_SelectRect(
        qtauNoteEditor &ne, noteSetup &ns, qne::editorNotes &nts, qne::editorState &st) :
        qtauEdController(ne, ns, nts, st) {}

qtauEd_SelectRect::qtauEd_SelectRect(qtauEdController *c) : qtauEdController(c) {}

qtauEd_SelectRect::~qtauEd_SelectRect() {}

void qtauEd_SelectRect::reset()
{
    // rect resetting should just deselect all notes and remove rect itself
    unselectAll();
    state->selectionRect.setRect(-1,-1,0,0);
    pointedNote = 0;
    lazyUpdate();
    changeController(new qtauEdController(this));
}

void qtauEd_SelectRect::mouseMoveEvent(QMouseEvent *event)
{
    QMap<quint64, bool> usedIDs;
    QVector<quint64> selected;

    int X = setup->barWidth  * 128; // TODO: move numBars in setup
    int Y = setup->octHeight * setup->numOctaves;

    QPoint pos = event->pos() + state->viewport.topLeft();
    QPoint tl(qMax(qMin(pos.x(), absFirstClickPos.x()), 0), qMax(qMin(pos.y(), absFirstClickPos.y()), 0));
    QPoint br(qMin(qMax(pos.x(), absFirstClickPos.x()), X), qMin(qMax(pos.y(), absFirstClickPos.y()), Y));

    state->selectionRect.setTopLeft(tl);
    state->selectionRect.setBottomRight(br);

    foreach (const quint64 &id, notes->selected)
    {
        usedIDs[id] = true;
        qne::editorNote &n = notes->idMap[id];
        bool intersects = state->selectionRect.intersects(n.r);

        if (intersects) selected.append(n.id);
        else            n.selected = false;
    }

    int stBar  = tl.x() / setup->barWidth;
    int endBar = br.x() / setup->barWidth;

    assert(endBar >= stBar);

    // check all notes in bars from first that has left edge of sel rect to last that has right edge
    if (stBar < notes->grid.size())
    {
        if (endBar >= notes->grid.size())
            endBar = notes->grid.size() - 1;

        for (int i = stBar; i <= endBar; ++i)
            for (int k = 0; k < notes->grid[i].size(); ++k)
            {
                quint64 id = notes->grid[i][k];

                if (!usedIDs.contains(notes->grid[i][k]))
                {
                    qne::editorNote &n = notes->idMap[id];

                    if (state->selectionRect.intersects(n.r))
                    {
                        selected.append(id);
                        n.selected = true;
                    }
                }
            }
    }

    notes->selected = selected;
    lazyUpdate();
}

void qtauEd_SelectRect::mouseReleaseEvent(QMouseEvent *event)
{
    state->selectionRect = QRect(-1,-1,0,0); // disable
    changeController(new qtauEdController(this));
    lazyUpdate();
}

//========================================================================

qtauEd_DragNotes::qtauEd_DragNotes(
        qtauNoteEditor &ne, noteSetup &ns, qne::editorNotes &nts, qne::editorState &st) :
        qtauEdController(ne, ns, nts, st) { init(); }

qtauEd_DragNotes::qtauEd_DragNotes(qtauEdController *c) : qtauEdController(c) { init(); }

qtauEd_DragNotes::~qtauEd_DragNotes() {}

void qtauEd_DragNotes::reset()
{
    // drag reset should move all selected notes back to original position
    foreach (const quint64 id, notes->selected)
    {
        qne::editorNote &n = notes->idMap[id];

        removeFromGrid(n.r.left(), n.id);
        removeFromGrid(n.r.right(), n.id);

        n.r.moveTo(n.dragSt);

        addToGrid(n.r.left(), n.id);
        addToGrid(n.r.right(), n.id);
    }

    pointedNote = 0;
    state->snapLine = -1;
    lazyUpdate();
    changeController(new qtauEdController(this));
}

void qtauEd_DragNotes::init()
{
    if (!pointedNote)
    {
        vsLog::e("Can't drug notes without note under cursor");
        changeController(new qtauEdController(this));
        return;
    }

    mainMovedNote = pointedNote;

    if (!mainMovedNote->selected)
    {
        mainMovedNote->selected = true;
        notes->selected.append(mainMovedNote->id);
    }

    foreach (const quint64 &id, notes->selected)
    {
        qne::editorNote &n = notes->idMap[id];
        n.dragSt = n.r.topLeft();
    }
}

void qtauEd_DragNotes::mouseMoveEvent(QMouseEvent *event)
{
    QPoint dynDelta = (event->pos() + state->viewport.topLeft()) - absFirstClickPos;
    QPoint desiredPos = mainMovedNote->dragSt + dynDelta;

    int minOffset = (setup->note.width() * 4) / setup->quantize;

    desiredPos.setX(qMax(0, qMin(snap(desiredPos.x(), minOffset), setup->barWidth * 128)));
    desiredPos.setY(qMax(0, qMin(snap(desiredPos.y(), setup->note.height()),
                                 setup->octHeight * setup->numOctaves)));

    QPoint snappedDelta = desiredPos - mainMovedNote->dragSt;

    foreach (const quint64 &id, notes->selected)
    {
        qne::editorNote &n = notes->idMap[id];

        removeFromGrid(n.r.left(),  n.id);
        removeFromGrid(n.r.right(), n.id);

        n.r.moveTo(n.dragSt + snappedDelta);
        updateModelData(n);

        addToGrid(n.r.left(),  n.id);
        addToGrid(n.r.right(), n.id);
    }

    state->snapLine = desiredPos.x();
    lazyUpdate();
}

void qtauEd_DragNotes::mouseReleaseEvent(QMouseEvent *event)
{
    state->snapLine = -1;

    QPoint pSt  = mainMovedNote->r.topLeft();
    QPoint pEnd = mainMovedNote->dragSt;

    if (pSt.x() != pEnd.x() && pSt.y() != pEnd.y())
    {
        // some movement was applied, so generate move event
        qtauEvent_NoteMove::noteMoveVector v;

        foreach (const quint64 &id, notes->selected)
        {
            qne::editorNote &n = notes->idMap[id];
            qtauEvent_NoteMove::noteMoveData d;
            d.id = n.id;
            d.pulseOffset = n.pulseOffset;
            d.keyNumber   = n.keyNumber;
            v.append(d);
        }

        qtauEvent_NoteMove *event = new qtauEvent_NoteMove(v);
        eventHappened(event);
    }

    lazyUpdate(); // need to remove that snap line
    changeController(new qtauEdController(this));
}

//========================================================================

qtauEd_ResizeNote::qtauEd_ResizeNote(
        qtauNoteEditor &ne, noteSetup &ns, qne::editorNotes &nts, qne::editorState &st, bool left) :
    qtauEdController(ne, ns, nts, st), toLeft(left) { init(); }

qtauEd_ResizeNote::qtauEd_ResizeNote(qtauEdController *c, bool left) : qtauEdController(c), toLeft(left)
{
    init();
}

qtauEd_ResizeNote::~qtauEd_ResizeNote() {}

void qtauEd_ResizeNote::reset()
{
    // reset() should just revert resized note to its original size, since resize event may mess with
    //   undo/redo stack if reset is called because of event
    pointedNote->r.setRect(originalRect);
    pointedNote = 0;
    state->snapLine = -1;
    lazyUpdate();
    changeController(new qtauEdController(this));
}

void qtauEd_ResizeNote::init()
{
    if (!pointedNote)
    {
        vsLog::e("Note resize controller is created without note to resize");
        changeController(new qtauEdController(this));
        return;
    }

    // min note width is 1/64 -> 1 px at max zoom, when 1/4 note width == 16 px
    minNoteWidth = (setup->note.width() * 4) / setup->length;

    editedNote = pointedNote;
    originalRect = pointedNote->r;
}

void qtauEd_ResizeNote::mouseMoveEvent(QMouseEvent *event)
{
    float pixelsToPulses = 480.0 / (float)setup->note.width(); // TODO: maybe shouldn't calc it every time?
    state->snapLine = snap(event->pos().x() + state->viewport.x(), minNoteWidth,
                           (toLeft ? editedNote->r.right() + 1 : editedNote->r.left()));

    removeFromGrid(editedNote->r.left(),  editedNote->id);
    removeFromGrid(editedNote->r.right(), editedNote->id);

    if (toLeft)
    {
        // calc new left coord, with magical +1's, because no code can work properly without a bit of magic
        state->snapLine = qMax(0, qMin(state->snapLine, editedNote->r.right() - minNoteWidth + 1));
        editedNote->r.setLeft(state->snapLine);
        editedNote->pulseOffset = editedNote->r.x() * pixelsToPulses + 0.001;
    }
    else
    {
        // calc new right coord
        state->snapLine = qMin(setup->barWidth * 128, // TODO: move that 128 somewhere already!
                               qMax(state->snapLine, editedNote->r.left() + minNoteWidth));
        editedNote->r.setRight(state->snapLine - 1);
    }

    addToGrid(editedNote->r.left(),  editedNote->id);
    addToGrid(editedNote->r.right(), editedNote->id);

    editedNote->pulseLength = editedNote->r.width() * pixelsToPulses + 0.001;
    lazyUpdate();
}

void qtauEd_ResizeNote::mouseReleaseEvent(QMouseEvent *event)
{
    state->snapLine = -1;
    owner->setCursor(Qt::ArrowCursor);

    if (editedNote->r.width() != originalRect.width())
    {
        qtauEvent_NoteResize::noteResizeData d;
        d.id         = editedNote->id;
        d.offset     = editedNote->r.x();
        d.length     = editedNote->r.width();
        d.prevOffset = originalRect.x();
        d.prevLength = originalRect.width();

        qtauEvent_NoteResize::noteResizeVector v;
        v.append(d);
        qtauEvent_NoteResize *noteResize = new qtauEvent_NoteResize(v);

        eventHappened(noteResize);
    }

    lazyUpdate();
    changeController(new qtauEdController(this));
}

//========================================================================

qtauEd_AddNote::qtauEd_AddNote(
        qtauNoteEditor &ne, noteSetup &ns, qne::editorNotes &nts, qne::editorState &st) :
        qtauEdController(ne, ns, nts, st) { createNote(); }

qtauEd_AddNote::qtauEd_AddNote(qtauEdController *c) : qtauEdController(c) { createNote(); }

qtauEd_AddNote::~qtauEd_AddNote() {}

void qtauEd_AddNote::reset()
{
    // resetting adding should remove still dragged new note completely, since its add event on mouseUp
    //   may mess with undo/redo stack if reason for reset is new event
    removeFromGrid(editedNote->r.left(), editedNote->id);
    removeFromGrid(editedNote->r.right(), editedNote->id);

    if (editedNote->selected)
    {
        int i = notes->selected.indexOf(editedNote->id);

        if (i >= 0)
            notes->selected.remove(i);
    }

    notes->idMap.remove(editedNote->id);
    pointedNote = 0;
    state->snapLine = -1;
    lazyUpdate();
    changeController(new qtauEdController(this));
}

// won't overload resizenote's init() because using virtual funcs from constructors in C++ is a bad idea
void qtauEd_AddNote::createNote()
{
    minOffset = (setup->note.width() * 4) / setup->quantize;  // using quantizing to place note

    int hUnits = absFirstClickPos.y() / setup->note.height();
    int vOff = hUnits * setup->note.height(); // floor to prev note offset

    int   units     = absFirstClickPos.x() / minOffset;
    float percent   = (float)(absFirstClickPos.x() % minOffset) / (float)minOffset;

    int hOff = (percent >= 0.5) ? (units + 1) * minOffset : units * minOffset;

    minOffset = (setup->note.width() * 4) / setup->length; // using musical note length (1/64..1/4) for size

    qne::editorNote n;
    n.r.setRect(hOff, vOff, minOffset, setup->note.height());
    n.id  = ++idOffset;
    n.txt = "a";

    updateModelData(n);
    addToGrid(n.r.left(), n.id);
    notes->idMap[n.id] = n;

    editedNote  = &notes->idMap[n.id];
    pointedNote = editedNote;
}

void qtauEd_AddNote::mouseMoveEvent(QMouseEvent *event)
{
    // remove right coord from grid
    removeFromGrid(editedNote->r.left(),  editedNote->id);
    removeFromGrid(editedNote->r.right(), editedNote->id);

    // calc new right coord
    state->snapLine = snap(event->pos().x() + state->viewport.x(), minOffset, editedNote->r.x());
    state->snapLine = qMin(setup->barWidth * 128, // TODO: move that 128 somewhere already!
                           qMax(state->snapLine, editedNote->r.left() + minOffset));
    editedNote->r.setRight(state->snapLine - 1);
    updateModelData(*editedNote);

    addToGrid(editedNote->r.left(),  editedNote->id);
    addToGrid(editedNote->r.right(), editedNote->id);

    lazyUpdate();
}

void qtauEd_AddNote::mouseReleaseEvent(QMouseEvent *event)
{
    state->snapLine = -1;

    qtauEvent_NoteAddition::noteAddData d;
    d.id     = editedNote->id;
    d.lyrics = editedNote->txt;

    d.pulseOffset = editedNote->pulseOffset;
    d.pulseLength = editedNote->pulseLength;
    d.keyNumber   = editedNote->keyNumber;

    qtauEvent_NoteAddition::noteAddVector v;
    v.append(d);
    qtauEvent_NoteAddition *noteAdd = new qtauEvent_NoteAddition(v);

    eventHappened(noteAdd);

    lazyUpdate();
    changeController(new qtauEdController(this));
}
