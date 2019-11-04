#pragma once
#include <QMainWindow>

namespace Ui {
class Console;
}

class QtConsole : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit QtConsole(QWidget *parent = 0);
   ~QtConsole();

   void clear();

   void append(const QString & rich_text);

   QSize sizeHint() const;

public slots:

   void on_actionClear_triggered();   
   
  private:
   Ui::Console * ui;
};