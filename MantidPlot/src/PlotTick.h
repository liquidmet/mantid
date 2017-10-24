#ifndef PLOT_TICK_H
#define PLOT_TICK_H
#include <qwt_painter.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_map.h>
#include <qwt_scale_widget.h>
#include <qwt_text_label.h>

class HorizontalAxis {
public:
  explicit HorizontalAxis(int distanceFromTop);
  int distanceFromTop() const;

private:
  int m_distanceFromTop;
};

HorizontalAxis onHorizontalAxis(int distanceFromTop);

class VerticalAxis {
public:
  explicit VerticalAxis(int distanceFromLeft);
  int distanceFromLeft() const;

private:
  int m_distanceFromLeft;
};

VerticalAxis onVerticalAxis(int distanceFromLeft);

class ValueOnAxis {
public:
  explicit ValueOnAxis(int atValue);
  int at() const;

private:
  int m_atValue;
};

ValueOnAxis atValue(int value);

class VerticalTickLength {
public:
  enum Direction { UP, DOWN };
  explicit VerticalTickLength(int tickLength, Direction direction);
  int lengthAsYOffset() const;

private:
  int m_lengthAsYOffset;
};

VerticalTickLength petrudingUpBy(int length);
VerticalTickLength petrudingDownBy(int length);

class HorizontalTickLength {
public:
  enum Direction { LEFT, RIGHT };

  explicit HorizontalTickLength(int tickLength, Direction direction);
  int lengthAsXOffset() const;

private:
  int m_lengthAsXOffset;
};

HorizontalTickLength petrudingLeftBy(int length);
HorizontalTickLength petrudingRightBy(int length);

void drawInwardTicksList(QPainter *painter, const QwtScaleMap &map,
                         VerticalAxis axis, const QwtValueList &ticks,
                         HorizontalTickLength tickLength, std::pair<int, int> valueBounds);

void drawInwardTicksList(QPainter *painter, const QwtScaleMap &map,
                         HorizontalAxis axis, const QwtValueList &ticks,
                         VerticalTickLength tickLength, std::pair<int, int> valueBounds);

void drawInwardTick(QPainter *painter, VerticalAxis axis, ValueOnAxis value,
                    HorizontalTickLength tickLength);

void drawInwardTick(QPainter *painter, HorizontalAxis axis, ValueOnAxis value,
                    VerticalTickLength tickLength);

#endif // PLOT_TICK_H
