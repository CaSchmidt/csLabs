#include <QtWidgets/QApplication>

#include "wmainwindow.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  WMainWindow *window = new WMainWindow();
  window->show();

  const int result = app.exec();
  delete window;

  return result;
}
