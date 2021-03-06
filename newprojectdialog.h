#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "project.h"
#include "mcu.h"
#include "programmer.h"

namespace Ui {
class newProjectDialog;
}

class newProjectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit newProjectDialog(QWidget *parent = 0);
	~newProjectDialog();

	Project project;

public slots:
	void selectTemplate();

	void createProject();

private:
	Ui::newProjectDialog *ui;

	QDir templateDir;
	QDir selectedTemplateDir;

	QFile * templateFile;

	void setMCUList();
	void setProgrammerList();

	QList <MCU> mcuList;
	QList <Programmer> programmerList;
};

#endif // NEWPROJECTDIALOG_H
