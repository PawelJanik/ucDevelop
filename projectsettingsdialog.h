#ifndef PROJECTSETTINGSDIALOG_H
#define PROJECTSETTINGSDIALOG_H

#include <QDialog>
#include <QList>
#include <QFile>
#include <QTextStream>

#include "project.h"
#include "mcu.h"

namespace Ui {
class projectSettingsDialog;
}

class projectSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit projectSettingsDialog(QWidget *parent = 0);
	~projectSettingsDialog();

	Project project;

	void setProjectToEdit(Project projectToEdit);

public slots:
	void editProject();

private:
	Ui::projectSettingsDialog *ui;

	QList <MCU> mcuList;
	QList <Programmer> programmerList;
};

#endif // PROJECTSETTINGSDIALOG_H
