#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>

#include "mcu.h"
#include "programmer.h"
#include "global.h"

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

	void addFile(QString fileName, QString text = "");
	void addClass(QString className);

	void importFile(QUrl url);
	void importFolder(QUrl url);

private:
	QUrl projectFileUrl;
	void addFileToProject(QString fileName);
};

#endif // PROJECT_H
