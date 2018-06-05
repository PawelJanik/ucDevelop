#include "ucdevelop.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ucDevelop w;
	w.show();

	return a.exec();
}
