#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <QtWidgets/QApplication>

#include <SimPlot/SimPlotTheme.h>
#include <SimPlot/SimPlotWidget.h>

#include "MySignal.h"

void test_findLeft(const ISimPlotSeriesData *data)
{
  printf("LEFT: #%d\n", data->size());
  for(int i = 0; i < data->size(); i++) {
    const qreal x = data->valueX(i);
    const int hit = data->findLeft(x);
    printf("x[%3d] = %.3f -> hit = %3d %s\n",
           i, x, hit, hit != i ? "ERROR" : "");
  }
  printf("LEFT-MID\n");
  for(int i = 0; i < data->size()-1; i++) {
    const qreal x = (data->valueX(i) + data->valueX(i+1))/2.0;
    const int hit = data->findLeft(x);
    printf("x[%3d] = %.3f -> hit = %3d %s\n",
           i, x, hit, hit != i ? "ERROR" : "");
  }
  printf("LEFT-OUT\n");
  const qreal  outLeft = data->valueX(0)                - 1;
  printf("findLeft(%.3f) -> %3d\n", outLeft, data->findLeft(outLeft));
  const qreal outRight = data->valueX(data->size() - 1) + 1;
  printf("findLeft(%.3f) -> %3d\n", outRight, data->findLeft(outRight));
  fflush(stdout);
}

void test_findRight(const ISimPlotSeriesData *data)
{
  printf("RIGHT: #%d\n", data->size());
  for(int i = 0; i < data->size(); i++) {
    const qreal x = data->valueX(i);
    const int hit = data->findRight(x);
    printf("x[%3d] = %.3f -> hit = %3d %s\n",
           i, x, hit, hit != i ? "ERROR" : "");
  }
  printf("RIGHT-MID\n");
  for(int i = 1; i < data->size(); i++) {
    const qreal x = (data->valueX(i-1) + data->valueX(i))/2.0;
    const int hit = data->findRight(x);
    printf("x[%3d] = %.3f -> hit = %3d %s\n",
           i, x, hit, hit != i ? "ERROR" : "");
  }
  printf("RIGHT-OUT\n");
  const qreal  outLeft = data->valueX(0)                - 1;
  printf("findRight(%.3f) -> %3d\n", outLeft, data->findRight(outLeft));
  const qreal outRight = data->valueX(data->size() - 1) + 1;
  printf("findRight(%.3f) -> %3d\n", outRight, data->findRight(outRight));
  fflush(stdout);
}

int main(int argc, char **argv)
{
  std::srand(std::time(0));

  QApplication app(argc, argv);

  SimPlotWidget *plot = new SimPlotWidget();
  plot->setXTitle(SimPlotTheme::titleString(QStringLiteral("Time"),
                                            QStringLiteral("s")));
  plot->show();
  plot->resize(640, 480);

  plot->insert(MySignal::generate(QStringLiteral("V")));
  plot->insert(MySignal::generate(QStringLiteral("V")));

  ISimPlotSeriesData *sine = MySignal::sine(QString());
  // test_findLeft(sine);
  // test_findRight(sine);
  plot->insert(sine);

  QFont font = plot->font();
  font.setPointSize(10);
  plot->setFont(font);

  // plot->remove(QStringLiteral("Signal 1"));
  // plot->remove(QStringLiteral("Signal 2"));
  // plot->remove(QStringLiteral("Signal 3"));

  // plot->setActiveSeries(QStringLiteral("Signal 3"));

  const int result = app.exec();
  delete plot;

  return result;
}
