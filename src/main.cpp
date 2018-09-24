#include <QApplication>
#include <Urho3D/Core/Context.h>
#include <toolkit.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Urho3D::Context * context = new Urho3D::Context();
    Toolkit tk(context);
	tk.init();
    return a.exec();
}
