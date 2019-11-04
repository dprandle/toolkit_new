#include <console.h>
#include <ui_console.h>

QtConsole::QtConsole(QWidget *parent):
QMainWindow(parent),
ui(new Ui::Console)
{
   ui->setupUi(this);
}

QtConsole::~QtConsole()
{
    delete ui;
}

void QtConsole::on_actionClear_triggered()
{
    clear();
}

void QtConsole::clear()
{
    ui->textEdit->clear();    
}

void QtConsole::append(const QString & rich_text)
{
    ui->textEdit->append(rich_text);
}

QSize QtConsole::sizeHint() const
{
    return QSize(1600,400);
}
