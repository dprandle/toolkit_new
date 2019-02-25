#pragma once
#include <QMainWindow>

namespace Ui {
class Asset_View;
}

class Asset_View : public QMainWindow
{
   Q_OBJECT
   
  public:
   
   explicit Asset_View(QWidget *parent = 0);
   ~Asset_View();
   
  private:
   Ui::Asset_View * ui;
};