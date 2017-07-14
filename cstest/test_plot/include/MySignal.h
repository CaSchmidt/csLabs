#ifndef __MYSIGNAL_H__
#define __MYSIGNAL_H__

#include <QtCore/QVector>

#include <SimPlot/ISimPlotSeriesData.h>

typedef QVector<double> SignalData;

class MySignal : public ISimPlotSeriesData {
public:
  MySignal(const QString& name, const QString& unit,
           const SignalData& x, const SignalData& y);
  ~MySignal();

  QString name() const;
  QString unit() const;

  qreal valueX(const int i) const;
  qreal valueY(const int i) const;

  QPointF value(const int i) const;
  int size() const;

  SimPlotRange rangeX() const;
  SimPlotRange rangeY() const;

  static MySignal *generate(const QString& unit);
  static MySignal *sine(const QString& unit);

private:
  static int _count;
  QString _name;
  QString _unit;
  SignalData _x;
  SignalData _y;
  SimPlotRange _rx;
  SimPlotRange _ry;
};

#endif // __MYSIGNAL_H__
