#include "projectsettingsdialog.h"
#include "ui_projectsettingsdialog.h"

projectSettingsDialog::projectSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::projectSettingsDialog)
{
	ui->setupUi(this);
}

projectSettingsDialog::~projectSettingsDialog()
{
	delete ui;
}
