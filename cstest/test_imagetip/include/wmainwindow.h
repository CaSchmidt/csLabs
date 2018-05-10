#ifndef __WMAINWINDOW_H__
#define __WMAINWINDOW_H__

#include <QtWidgets/QMainWindow>

namespace Ui {
  class WMainWindow;
}

class WMainWindow : public QMainWindow {
  Q_OBJECT
public:
  WMainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
  ~WMainWindow();

protected:
  bool eventFilter(QObject *object, QEvent *event);

private slots:
  void openFile();

private:
  Ui::WMainWindow *ui;
  QImage _image;
};

#endif // __WMAINWINDOW_H__
