#pragma once
#include <QDockWidget>


class Custom_Dock_Widget : public QDockWidget
{
   Q_OBJECT
   
  public:
   
   explicit Custom_Dock_Widget(QWidget *parent = 0);

   ~Custom_Dock_Widget();

   bool manually_floating();

protected:
  
    void changeEvent(QEvent *event);
  
private:
    bool floating_;
};