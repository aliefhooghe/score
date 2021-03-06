// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "MidiView.hpp"

#include <Midi/MidiStyle.hpp>

#include <score/widgets/GraphicsItem.hpp>

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPainter>

#include <wobjectimpl.h>
W_OBJECT_IMPL(Midi::View)
namespace Midi
{

static const MidiStyle style;
View::View(QGraphicsItem* parent) : Process::LayerView{parent}
{
  this->setAcceptHoverEvents(true);
  this->setAcceptDrops(true);
  this->setFlag(QGraphicsItem::ItemIsFocusable, true);
  this->setFlag(QGraphicsItem::ItemClipsToShape, true);
}

View::~View()
{
}

void View::heightChanged(qreal h)
{
  QImage bg(1920, h, QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&bg);
  auto p = &painter;

  p->setRenderHint(QPainter::Antialiasing, false);
  p->setPen(style.darkPen);
  //    1 3   6 8 10
  //   0 2 4 5 7 9  11
  const auto rect = boundingRect();
  const auto note_height = rect.height() / (visibleCount());

  const auto for_white_notes = [&](auto fun) {
    for (int i = m_min; i <= m_max; i++)
      switch (i % 12)
      {
        case 0:
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
        case 11:
          fun(i);
          break;
      }
  };
  const auto for_black_notes = [&](auto fun) {
    for (int i = m_min; i <= m_max; i++)
      switch (i % 12)
      {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
          fun(i);
          break;
      }
  };

  p->setPen(style.darkerBrush.color());
  p->setBrush(style.darkerBrush);
  p->setPen(style.darkPen);

  if (canEdit())
  {
    if (auto v = getView(*this))
    {
      const qreal width = std::max(v->width(), 800) * 2;

      {
        QRectF* white_rects
            = (QRectF*)alloca((sizeof(QRectF) * visibleCount()));
        int max_white = 0;
        const auto draw_bg_white = [&](int i) {
          white_rects[max_white++]
              = QRectF{0, rect.height() + note_height * (m_min - i - 1) - 1,
                       width, note_height};
        };
        for_white_notes(draw_bg_white);
        p->setBrush(style.lightBrush);
        p->drawRects(white_rects, max_white);
      }

      {
        QRectF* black_rects
            = (QRectF*)alloca((sizeof(QRectF) * visibleCount()));
        int max_black = 0;
        const auto draw_bg_black = [&](int i) {
          black_rects[max_black++]
              = QRectF{0, rect.height() + note_height * (m_min - i - 1) - 1,
                       width, note_height};
        };
        for_black_notes(draw_bg_black);

        p->setBrush(style.darkBrush);
        p->drawRects(black_rects, max_black);
      }

      if (note_height > 10)
      {
        static constexpr const char* texts[]{
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        const auto draw_text = [&](int i) {
          p->drawText(
              QRectF{2., rect.height() + note_height * (m_min - i - 1) - 1,
                     width, note_height},
              texts[i % 12], QTextOption{Qt::AlignVCenter});
        };

        p->setPen(style.darkerBrush.color());
        for_white_notes(draw_text);
        for_black_notes(draw_text);
      }
    }
  }

  m_bgCache = QPixmap::fromImage(bg);
}

void View::widthChanged(qreal w)
{
}

void View::setDefaultWidth(double w)
{
  m_defaultW = w;
  update();
  for (auto cld : childItems())
    cld->update();
}

void View::setRange(int min, int max)
{
  m_min = min;
  m_max = max;
  update();
}

bool View::canEdit() const
{
  const auto rect = boundingRect();
  const auto note_height = rect.height() / (visibleCount());
  return note_height > 5;
}

void View::paint_impl(QPainter* p) const
{
  if (auto v = getView(*this))
  {
    const auto view_left = v->mapToScene(0, 0);
    const auto left = std::max(0., this->mapFromScene(view_left).x());

    p->drawPixmap(left, 0, m_bgCache);
  }
  if (!m_selectArea.isEmpty())
  {
    p->setBrush(style.transparentBrush);
    p->setPen(style.selectionPen);
    p->drawPath(m_selectArea);
  }
}

void View::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  askContextMenu(event->screenPos(), event->scenePos());
  event->accept();
}

void View::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
  if (canEdit())
  {
    pressed(ev->scenePos());
  }
  ev->accept();
}

void View::mouseMoveEvent(QGraphicsSceneMouseEvent* ev)
{
  if (canEdit())
  {
    QPainterPath p;
    p.addRect(QRectF{ev->buttonDownPos(Qt::LeftButton), ev->pos()});
    this->scene()->setSelectionArea(mapToScene(p));

    m_selectArea = p;
    update();
  }
  ev->accept();
}

void View::mouseReleaseEvent(QGraphicsSceneMouseEvent* ev)
{
  if (canEdit())
  {
    m_selectArea = {};
    update();
  }
  ev->accept();
}

void View::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* ev)
{
  if (canEdit())
  {
    doubleClicked(ev->pos());
  }
  ev->accept();
}

void View::keyPressEvent(QKeyEvent* ev)
{
  if (ev->key() == Qt::Key_Backspace || ev->key() == Qt::Key_Delete)
  {
    deleteRequested();
  }

  ev->accept();
}

void View::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  dropReceived(event->pos(), *event->mimeData());
  event->accept();
}

NoteData View::noteAtPos(QPointF point) const
{
  const auto rect = boundingRect();

  NoteData n;
  n.m_start = std::max(0., point.x() / m_defaultW);
  n.m_duration = 0.1;
  n.m_pitch = qBound(
      m_min,
      1
          + int(m_max
                - (qMin(rect.bottom(), qMax(point.y(), rect.top()))
                   / rect.height())
                      * visibleCount()),
      m_max);

  n.m_velocity = 127.;
  return n;
}

int View::visibleCount() const
{
  return m_max - m_min + 1;
}
}
