#include <cmath>
#include <cstdlib>

#include "MySignal.h"

////// Macros ////////////////////////////////////////////////////////////////

#ifndef M_PI
# define M_PI  3.14159265358979323846
#endif

////// public ////////////////////////////////////////////////////////////////

MySignal::MySignal(const QString& name, const QString& unit,
                   const SignalData& x, const SignalData& y)
  : ISimPlotSeriesData()
  , _name()
  , _unit()
  , _x()
  , _y()
{
  if( !name.isEmpty()  &&  x.size() == y.size()  &&  x.size() > 1 ) {
    _name = name;
    _unit = unit;
    _x = x;
    _y = y;
  }
  updateRange();
}

MySignal::~MySignal()
{
  printf("%s\n", __FUNCSIG__);
  fflush(stdout);
}

QString MySignal::name() const
{
  return _name;
}

QString MySignal::unit() const
{
  return _unit;
}

qreal MySignal::valueX(const int i) const
{
  return _x[i];
}

qreal MySignal::valueY(const int i) const
{
  return _y[i];
}

void MySignal::values(QPointF *points, const int L, const int R) const
{
  for(int i = L; i <= R; i++) {
    *points++ = QPointF(_x[i], _y[i]);
  }
}

QPointF MySignal::value(const int i) const
{
  return QPointF(_x[i], _y[i]);
}

int MySignal::size() const
{
  return _x.size();
}

int MySignal::_count = 0;

MySignal *MySignal::generate(const QString& unit)
{
  const double      t0 = 0.0;
  const double       T = 5.0;
  const double      dt = 0.01;
  const int numSamples = (int)(T/dt) + 1;

  const double    amp = (double)(std::rand()%10 + 1); // rand()%10 == [0,9]
  const double   freq = (double)(std::rand()%3 + 1);  // rand()%3  == [0,2]
  const double tshift = (double)(std::rand()%11)*0.1; // rand()%11 == [0,10]

  SignalData x(numSamples), y(numSamples);
  for(int i = 0; i < numSamples; i++) {
    const double   t = t0 + (double)i*dt;
    const double phi = 2.0*M_PI*freq*t;

    x[i] = t + tshift;
    y[i] = amp*std::sin(phi);
  }

  return new MySignal(QStringLiteral("Signal ") + QString::number(++_count),
                      unit, x, y);
}

MySignal *MySignal::sine(const QString& unit)
{
  const double    freq = 1.0;
  const double      t0 = 0.0;
  const double      dt = 0.01;
  const int numSamples = 101;

  SignalData x(numSamples), y(numSamples);
  for(int i = 0; i < numSamples; i++) {
    const double   t = t0 + (double)i*dt;
    const double phi = 2.0*M_PI*freq*t;

    x[i] = t;
    y[i] = std::sin(phi);
  }

  return new MySignal(QStringLiteral("Signal ") + QString::number(++_count),
                      unit, x, y);
}
