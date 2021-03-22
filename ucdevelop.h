#ifndef AVRGUI_H
#define AVRGUI_H

#include <QMainWindow>

//#include <KTextEditor/Document>
//#include <KTextEditor/Editor>
//#include <KTextEditor/View>

#include <QTextEdit>

#include <QFileDialog>
#include <QFileSystemModel>
#include <QDir>
#include <QModelIndex>

#include <QMessageBox>
#include <QInputDialog>

#include <QProcess>
#include <QScrollBar>

#include "project.h"
#include "projectsettingsdialog.h"
#include "newprojectdialog.h"

namespace Ui {
class ucDevelop;
}

class ucDevelop : public QMainWindow
{
	Q_OBJECT

public:
	explicit ucDevelop(QWidget *parent = 0);
	~ucDevelop();

protected:
	void closeEvent(QCloseEvent * event) override;

public slots:
	void newFile();
	void newProject();

	void openFile();
	void openProject();

	void save();
	void save(int index);
	void saveAs(int index);
	void saveProject();

	void closeFile();

	void copy();
	void cut();
	void paste();

	void build();
	void upload();
	void buildAndUpload();

	void cleanProject();
	void rebuild();

	void readProcessData();

	void settings();
	void projectSettings();

	void addFile();
	void addClass();
	void importFile();
	void importFolder();

	void closeProject();

	void viewIsChanged();

	void help();
	void about();

	void fileListClicked();
	void documentChanged();

	void projectViewDoubleClick(QModelIndex index);

	void clearOutput();

	void fileWidgetContextMenu(const QPoint &pos);
	void projectViewContextMenu(const QPoint &pos);

private:
	Ui::ucDevelop *ui;

	QTextEdit * editor;
	QList<QTextDocument *> doc;

	int actualOpenDoc;

	void openURL(QString fileName);
	void openProjectUrl(QUrl projectUrl);
	int isOpenFile(QString fileName);

	int documentIsClosing(int index);
	int closeFileAt(int index);

	Project project;
	QFileSystemModel * projectFileModel;

	QProcess * buildProcess;
	QString outputString;

	int buildWarningCounter;
	int buildErrorCounter;
	int uploadFalied;
	bool isBuild;
};

#endif // AVRGUI_H
