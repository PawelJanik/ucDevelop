#ifndef PROJECTSETTINGSDIALOG_H
#define PROJECTSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class projectSettingsDialog;
}

class projectSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit projectSettingsDialog(QWidget *parent = 0);
	~projectSettingsDialog();

private:
	Ui::projectSettingsDialog *ui;
};

#endif // PROJECTSETTINGSDIALOG_H
