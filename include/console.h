#pragma once
#include <QMainWindow>

namespace Ui {
class Console;
}

class Console : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit Console(QWidget *parent = 0);
   ~Console();

   void clear();

   void append(const QString & rich_text);

   QSize sizeHint() const;

public slots:

   void on_actionClear_triggered();   
   
  private:
   Ui::Console * ui;
};