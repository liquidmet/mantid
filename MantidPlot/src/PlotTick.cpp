#include "PlotTick.h"

void drawInwardTicksList(QPainter *painter, const QwtScaleMap &map,
                         VerticalAxis axis, const QwtValueList &ticks,
                         HorizontalTickLength tickLength,
                         std::pair<int, int> xBounds) {
  for (const auto &tick : ticks) {
    auto x = map.transform(tick);
    if (x > xBounds.first && x < xBounds.second)
      drawInwardTick(painter, axis, atValue(x), tickLength);
  }
}

void drawInwardTicksList(QPainter *painter, const QwtScaleMap &map,
                         HorizontalAxis axis, const QwtValueList &ticks,
                         VerticalTickLength tickLength,
                         std::pair<int, int> yBounds) {
  for (const auto &tick : ticks) {
    auto y = map.transform(tick);
    if (y > yBounds.first && y < yBounds.second)
      drawInwardTick(painter, axis, atValue(y), tickLength);
  }
}

void drawInwardTick(QPainter *painter, VerticalAxis axis, ValueOnAxis value,
                    HorizontalTickLength tickLength) {
  QwtPainter::drawLine(painter, axis.distanceFromLeft(), value.at(),
                       axis.distanceFromLeft() + tickLength.lengthAsXOffset(),
                       value.at());
}

void drawInwardTick(QPainter *painter, HorizontalAxis axis, ValueOnAxis value,
                    VerticalTickLength tickLength) {
  QwtPainter::drawLine(painter, value.at(), axis.distanceFromTop(), value.at(),
                       axis.distanceFromTop() + tickLength.lengthAsYOffset());
}

HorizontalAxis::HorizontalAxis(int distanceFromTop)
    : m_distanceFromTop{distanceFromTop} {}

int HorizontalAxis::distanceFromTop() const { return m_distanceFromTop; }

HorizontalAxis onHorizontalAxis(int distanceFromTop) {
  return HorizontalAxis(distanceFromTop);
}

VerticalAxis::VerticalAxis(int distanceFromLeft)
    : m_distanceFromLeft{distanceFromLeft} {};

int VerticalAxis::distanceFromLeft() const { return m_distanceFromLeft; }

VerticalAxis onVerticalAxis(int distanceFromLeft) {
  return VerticalAxis(distanceFromLeft);
}

ValueOnAxis::ValueOnAxis(int atValue) : m_atValue(atValue) {}
int ValueOnAxis::at() const { return m_atValue; }

ValueOnAxis atValue(int value) { return ValueOnAxis(value); }

VerticalTickLength::VerticalTickLength(int tickLength, Direction direction)
    : m_lengthAsYOffset{direction == Direction::UP ? -tickLength
                                                   : tickLength} {};

int VerticalTickLength::lengthAsYOffset() const { return m_lengthAsYOffset; }

VerticalTickLength petrudingUpBy(int length) {
  return VerticalTickLength(length, VerticalTickLength::Direction::UP);
}

VerticalTickLength petrudingDownBy(int length) {
  return VerticalTickLength(length, VerticalTickLength::Direction::DOWN);
}

HorizontalTickLength::HorizontalTickLength(int tickLength, Direction direction)
    : m_lengthAsXOffset{direction == Direction::LEFT ? -tickLength
                                                     : tickLength} {};

int HorizontalTickLength::lengthAsXOffset() const { return m_lengthAsXOffset; }

HorizontalTickLength petrudingLeftBy(int length) {
  return HorizontalTickLength(length, HorizontalTickLength::Direction::LEFT);
}

HorizontalTickLength petrudingRightBy(int length) {
  return HorizontalTickLength(length, HorizontalTickLength::Direction::RIGHT);
}

TickPlotter::TickPlotter(QPainter *painter, const QwtScaleMap &scaleMap,
                         QwtValueList const &minimumTicks,
                         QwtValueList const &mediumTicks,
                         QwtValueList const &maximumTicks,
                         int minimumTickLength, int majorTickLength)
    : m_painter{painter}, m_scaleMap{scaleMap}, m_minimumTicks{minimumTicks},
      m_mediumTicks{mediumTicks}, m_maximumTicks{maximumTicks},
      m_minimumTickLength{minimumTickLength}, m_majorTickLength{
                                                  majorTickLength} {}

void TickPlotter::operator()(HorizontalAxis axis,
                             VerticalTickLength::Direction tickDirection,
                             std::pair<int, int> xBounds) {
  if (m_minimumTickLength != 0) {
    drawInwardTicksList(m_painter, m_scaleMap, axis, m_minimumTicks,
                        VerticalTickLength(tickDirection, m_minimumTickLength),
                        xBounds);
    drawInwardTicksList(m_painter, m_scaleMap, axis, m_mediumTicks,
                        VerticalTickLength(tickDirection, m_minimumTickLength),
                        xBounds);
  }
  if (m_majorTickLength != 0) {
    drawInwardTicksList(m_painter, m_scaleMap, axis, m_majorTicks,
                        VerticalTickLength(tickDirection, m_majorTickLength),
                        xBounds);
  }
};

void TickPlotter::operator()(VerticalAxis axis,
                             HorizontalTickLength::Direction tickDirection,
                             std::pair<int, int> yBounds) {
  if (m_minimumTickLength != 0) {
    drawInwardTicksList(
        m_painter, m_scaleMap, axis, minTickList,
        HorizontalTickLength(tickDirection, m_minimumTickLength), yBounds);
    drawInwardTicksList(
        m_painter, m_scaleMap, axis, ticks,
        HorizontalTickLength(tickDirection, m_minimumTickLength), yBounds);
  }
  if (m_majorTickLength != 0) {
    drawInwardTicksList(m_painter, m_scaleMap, axis, ticks,
                        VerticalTickLength(tickDirection, m_majorTickLength),
                        yBounds);
  }
}
}
;
