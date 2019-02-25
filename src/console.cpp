#include <console.h>
#include <ui_console.h>

Console::Console(QWidget *parent):
QMainWindow(parent),
ui(new Ui::Console)
{
   ui->setupUi(this);
}

Console::~Console()
{
    delete ui;
}

void Console::on_actionClear_triggered()
{
    clear();
}

void Console::clear()
{
    ui->textEdit->clear();    
}

void Console::append(const QString & rich_text)
{
    ui->textEdit->append(rich_text);
}

QSize Console::sizeHint() const
{
    return QSize(1600,400);
}
