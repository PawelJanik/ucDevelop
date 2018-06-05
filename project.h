#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "mcu.h"
#include "programmer.h"

class Project
{
public:
	Project();

	QString name;
	QUrl path;
	MCU mcu;
	Programmer programmer;
	QStringList fileList;

	QString getMMCU();
	void setProjectData(QUrl projectUrl);

	QList <MCU> getMcuList();
	QList <Programmer> getProgrammerList();

	QString buildCommand;
	QString buildCommandAttribute;
	QString uploadCommand;
	QString uploadCommandAttribute;
	QString cleanCommand;

	QString projectType;
	QString projectManager;

	QString autorName;

	QString showProjectData();

private:
	QUrl projectFileUrl;
};

#endif // PROJECT_H
