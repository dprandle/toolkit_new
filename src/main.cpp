#include <QApplication>
#include <toolkit.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    Toolkit tk;
	tk.init();
    return a.exec();
}
