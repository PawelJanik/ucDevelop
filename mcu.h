#ifndef MCU_H
#define MCU_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class MCU
{
public:
	MCU();

	QString id;
	QString description;
	QString signature;

	QString parent;

	QString clock;

	int flashSize;
	int eepromSize;
	int lfuseSize;
	int hfuseSize;
	int efuseSize;
	int lockSize;

	void uploadAllData();
};

#endif // MCU_H
