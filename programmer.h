#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class Programmer
{
public:
	Programmer();

	QString id;
	QString description;
	QString portName;

	void uploadAllData();
};

#endif // PROGRAMMER_H
