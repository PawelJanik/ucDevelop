#include "ucdevelop.h"
#include "ui_ucdevelop.h"

ucDevelop::ucDevelop(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ucDevelop)
{
	ui->setupUi(this);

	editor = KTextEditor::Editor::instance();

	actualOpenDoc = 0;

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(ui->actionNewProject, SIGNAL(triggered()), this, SLOT(newProject()));

	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(ui->actionOpenProject, SIGNAL(triggered()), this, SLOT(openProject()));

	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
	connect(ui->actionSaveProject, SIGNAL(triggered()), this, SLOT(saveProject()));

	connect(ui->actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));

	connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
	connect(ui->actionCut, SIGNAL(triggered()), this, SLOT(cut()));
	connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(paste()));

	connect(ui->actionBuild, SIGNAL(triggered()), this, SLOT(build()));
	connect(ui->actionUpload, SIGNAL(triggered()), this, SLOT(upload()));
	connect(ui->actionBuildAndUpload, SIGNAL(triggered()), this, SLOT(buildAndUpload()));

	connect(ui->actionCleanProject, SIGNAL(triggered()), this, SLOT(cleanProject()));
	connect(ui->actionRebuild, SIGNAL(triggered()), this, SLOT(rebuild()));

	connect(ui->clearOutputButton, SIGNAL(clicked(bool)), this, SLOT(clearOutput()));

	connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settings()));
	connect(ui->actionProjectSettings, SIGNAL(triggered()), this, SLOT(projectSettings()));

	connect(ui->actionAddFile, SIGNAL(triggered()), this, SLOT(addFile()));
	connect(ui->actionAddClass, SIGNAL(triggered()), this, SLOT(addClass()));
	connect(ui->actionImportFile, SIGNAL(triggered()), this, SLOT(importFile()));
	connect(ui->actionImportFolder, SIGNAL(triggered()), this, SLOT(importFolder()));

	connect(ui->actionCloseProject, SIGNAL(triggered()), this, SLOT(closeProject()));

	connect(ui->actionViewFile, SIGNAL(triggered()), this, SLOT(viewIsChanged()));
	connect(ui->actionViewProject, SIGNAL(triggered()), this, SLOT(viewIsChanged()));
	connect(ui->actionViewOutput, SIGNAL(triggered()), this, SLOT(viewIsChanged()));

	connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(help()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));

	connect(ui->fileWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(fileListClicked()));
	connect(ui->projectView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(projectViewDoubleClick(QModelIndex)));

	ui->fileWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->projectView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->fileWidget, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(fileWidgetContextMenu(QPoint)));
	connect(ui->projectView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(projectViewContextMenu(QPoint)));

}

ucDevelop::~ucDevelop()
{
	delete ui;
}

void ucDevelop::closeEvent(QCloseEvent *event)
{
	bool accept = true;

	for(int i = 0; i < doc.count(); i++)
	{
		int result = closeFileAt(i);
		if(result == QMessageBox::Cancel)
			accept = false;
	}

	if (accept)
		event->accept();
	else
		event->ignore();
}

void ucDevelop::fileWidgetContextMenu(const QPoint &pos)
{
	ui->statusBar->showMessage("File list context menu");

	QListWidgetItem * clickedItem = ui->fileWidget->itemAt(pos);

	if(clickedItem != 0)
	{
		ui->statusBar->showMessage(clickedItem->text());

		QMenu contextMenu;
		contextMenu.addAction(ui->actionCloseFile);
		contextMenu.addAction(ui->actionSave);

		contextMenu.exec(ui->fileWidget->mapToGlobal(pos));
	}
}

void ucDevelop::projectViewContextMenu(const QPoint &pos)
{
	ui->statusBar->showMessage("Project context menu");
}

void ucDevelop::newFile()
{
	ui->statusBar->showMessage("New file");

	doc.append(editor->createDocument(0));
	view = doc.last()->createView(this);
	takeCentralWidget();
	setCentralWidget(view);

	ui->fileWidget->addItem("no named");
	ui->fileWidget->setCurrentRow(doc.count());

	connect(doc.last(), SIGNAL(modifiedChanged(KTextEditor::Document *)), this, SLOT(documentChanged()));
	connect(doc.last(), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));

	ui->fileWidget->setCurrentRow((ui->fileWidget->count())-1);
}

void ucDevelop::newProject()
{
	ui->statusBar->showMessage("New project");

	newProjectDialog * newprojectdialog = new newProjectDialog(this);

	int result = newprojectdialog->exec();

	if(result == 1)
	{
		QString projectFile(newprojectdialog->project.path.path().append("/").append(newprojectdialog->project.name).append(".ucdevelop"));
		openProjectUrl(projectFile);
	}
}

void ucDevelop::openFile()
{
	ui->statusBar->showMessage("Open file");

	QString fileName = QFileDialog::getOpenFileName(this,tr("Open file..."), "/home", tr("Source Files (*.c *.cpp *.h)"));

	openURL(fileName);
}

void ucDevelop::openURL(QString fileName)
{
	int isOpen = isOpenFile(fileName);

	if(!fileName.isNull() && (isOpen == (-1)))
	{
		QString stringURL = "file:";
		stringURL.append(fileName);
		QUrl url = QUrl(stringURL);

		doc.append(editor->createDocument(0));
		view = doc.last()->createView(this);
		takeCentralWidget();
		setCentralWidget(view);

		doc.last()->openUrl(url);

		ui->fileWidget->addItem(url.fileName());
		ui->fileWidget->setCurrentRow((ui->fileWidget->count())-1);

		connect(doc.last(), SIGNAL(modifiedChanged(KTextEditor::Document *)), this, SLOT(documentChanged()));
		connect(doc.last(), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));
	}
	else
	{
		actualOpenDoc = isOpen;
		ui->fileWidget->setCurrentRow(isOpen);
		view = doc.at(actualOpenDoc)->createView(this);
		takeCentralWidget();
		setCentralWidget(view);
	}
}

void ucDevelop::openProject()
{
	ui->statusBar->showMessage("Open project");

	QString filename = QFileDialog::getOpenFileName(this,tr("Open project..."), "/home", tr("Projects files (*.ucdevelop)"));

	openProjectUrl(QUrl(filename));
}

void ucDevelop::openProjectUrl(QUrl projectUrl)
{
	if(!projectUrl.isEmpty())
	{
		ui->statusBar->showMessage(QString("Open project from URL: ").append(projectUrl.path()));
		project.setProjectData(projectUrl);
		ui->output->setText(project.showProjectData());	//show data about  project

		projectFileModel = new QFileSystemModel;
		ui->projectView->setModel(projectFileModel);
		projectFileModel->setRootPath(project.path.path());
		ui->projectView->setRootIndex(projectFileModel->index(project.path.path()));
		ui->projectView->setHeaderHidden(true);

		ui->projectView->hideColumn(1);
		ui->projectView->hideColumn(2);
		ui->projectView->hideColumn(3);

		openURL(QString(project.path.path()).append("/").append(project.fileList.at(0)));

		ui->actionBuild->setEnabled(true);
		ui->actionUpload->setEnabled(true);
		ui->actionBuildAndUpload->setEnabled(true);
		ui->actionCleanProject->setEnabled(true);
		ui->actionRebuild->setEnabled(true);

		if(project.projectType.contains("C++",Qt::CaseInsensitive))
			ui->actionAddClass->setEnabled(true);

		ui->actionAddFile->setEnabled(true);
		ui->actionImportFile->setEnabled(true);
		ui->actionImportFolder->setEnabled(true);
		ui->actionProjectSettings->setEnabled(true);
		ui->actionCloseProject->setEnabled(true);
	}
}

int ucDevelop::isOpenFile(QString fileName)
{
	int result = -1;

	for(int i = 0; i < doc.count(); i++)
	{
		if(fileName == doc.at(i)->url().path())
		{
			result = i;
		}
	}

	return result;
}

void ucDevelop::save()
{
	ui->statusBar->showMessage("Save");

	if(!doc.isEmpty())
		doc.at(actualOpenDoc)->documentSave();
}

void ucDevelop::saveDocument(int index)
{
	ui->statusBar->showMessage("Save document");

	doc.at(index)->documentSave();
	disconnect(doc.at(actualOpenDoc), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));
	documentSavedAt(index);
	connect(doc.at(actualOpenDoc), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));
}

void ucDevelop::saveAs()
{
	if(!doc.isEmpty())
	{
		ui->statusBar->showMessage("Save as...");
		doc.at(actualOpenDoc)->documentSaveAs();
	}
}

void ucDevelop::saveProject()
{
	//save all
	ui->statusBar->showMessage("Save project");

	for(int i = 0; i < doc.count(); i++)
	{
		saveDocument(i);
	}
}

void ucDevelop::closeFile()
{
	if(!doc.isEmpty())
	{
		ui->statusBar->showMessage("Close file");

		closeFileAt(ui->fileWidget->currentRow());
	}
}

int ucDevelop::closeFileAt(int index)
{
	int result = documentIsClosing(index);

	if(result != QMessageBox::Cancel)
	{
		ui->fileWidget->takeItem(index);
		doc.removeAt(index);

		if(!doc.isEmpty())
		{
			actualOpenDoc = ui->fileWidget->currentRow();
			view = doc.at(actualOpenDoc)->createView(this);
			setCentralWidget(view);
		}

		if(ui->fileWidget->count() == 0)
		{
			takeCentralWidget();
			setCentralWidget(ui->centralWidget);
		}
	}

	return result;
}

int ucDevelop::documentIsClosing(int index)
{
	ui->statusBar->showMessage("Document is closing");

	int result = QMessageBox::Save;

	QString fileName = ui->fileWidget->item(index)->text();

	ui->statusBar->showMessage(fileName);

	if(fileName.contains("*",Qt::CaseInsensitive))
	{
		QMessageBox saveMessage;
		saveMessage.setText(QString("Document: ").append(doc.at(index)->documentName()).append( " has been modified."));
		saveMessage.setInformativeText("Do you want to save your changes?");
		saveMessage.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		saveMessage.setDefaultButton(QMessageBox::Save);
		result = saveMessage.exec();

		if(result == QMessageBox::Save)
			saveDocument(index);
	}

	return result;
}

void ucDevelop::copy()
{
	ui->statusBar->showMessage("Copy");
}

void ucDevelop::cut()
{
	ui->statusBar->showMessage("Cut");
}

void ucDevelop::paste()
{
	ui->statusBar->showMessage("Paste");
}

void ucDevelop::build()
{
	ui->statusBar->showMessage("Bulid");

	for(int i = 0; i < doc.count(); i++)
	{
		saveDocument(i);
	}

	buildWarningCounter = 0;
	buildErrorCounter = 0;

	clearOutput();
	ui->outputLed->setColor(Qt::gray);
	ui->outputLabel->setText("Program is building...");

	buildProcess = new QProcess(0);
	buildProcess->setWorkingDirectory(project.path.path());

	connect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(readProcessData()));
	connect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessData()));

	QStringList arg;
	if(!project.buildCommand.isEmpty())
		arg << project.buildCommand;

	if(!project.buildCommandAttribute.isEmpty())
		arg << project.buildCommandAttribute;

	//if(project.projectManager == "cmake")			//for cmake
	//	buildProcess->start("cmake", arg);

	//buildProcess->waitForFinished();

	buildProcess->start("make", arg);

	buildProcess->waitForFinished();

	delete buildProcess;

	buildWarningCounter += outputString.count("warning",Qt::CaseInsensitive);

	buildErrorCounter += outputString.count("error",Qt::CaseInsensitive);

	if((buildErrorCounter == 0) && (buildWarningCounter == 0))
	{
		ui->outputLed->setColor(Qt::green);
		ui->outputLabel->setText("Program was build.");
		isBuild = true;
	}
	else if((buildErrorCounter == 0) && (buildWarningCounter > 0))
	{
		ui->outputLed->setColor(Qt::yellow);
		ui->outputLabel->setText(QString("Program was built with ")
					 .append(QString::number(buildWarningCounter))
					 .append(" warnings."));
		isBuild = true;
	}
	else if((buildErrorCounter > 0) && (buildWarningCounter == 0))
	{
		ui->outputLed->setColor(Qt::red);
		ui->outputLabel->setText(QString("Program has ")
					 .append(QString::number(buildErrorCounter))
					 .append(" errors."));
		isBuild = false;
	}
	else if((buildErrorCounter > 0) && (buildWarningCounter > 0))
	{
		ui->outputLed->setColor(Qt::red);
		ui->outputLabel->setText(QString("Program has ")
					 .append(QString::number(buildErrorCounter))
					 .append(" errors, and ")
					 .append(QString::number(buildWarningCounter))
					 .append(" warnings."));
		isBuild = false;
	}
}

void ucDevelop::upload()
{
	ui->statusBar->showMessage("Upload");

	if((buildErrorCounter == 0) && (isBuild == true))
	{
		clearOutput();
		uploadFalied = 0;
		ui->outputLed->setColor(Qt::gray);
		ui->outputLabel->setText("Program is uploading...");

		buildProcess = new QProcess(0);
		buildProcess->setWorkingDirectory(project.path.path());

		connect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(readProcessData()));
		connect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessData()));

		QStringList arg;
		if(!project.uploadCommand.isEmpty())
			arg << project.uploadCommand;

		if(!project.uploadCommandAttribute.isEmpty())
		{
			arg << " ";
			arg << project.uploadCommandAttribute;
		}

		buildProcess->start("make", arg);

		buildProcess->waitForFinished();

		delete buildProcess;

		if(outputString.contains("avrdude: AVR device not responding",Qt::CaseInsensitive))
		{
			uploadFalied++;
			ui->outputLed->setColor(Qt::red);
			ui->outputLabel->setText("Check the device connection or program configuration.");
		}

		if(uploadFalied == 0)
		{
			ui->outputLed->setColor(Qt::green);
			ui->outputLabel->setText("Program was uploaded.");
		}
	}
	else
	{
		build();
		if(buildErrorCounter == 0)
			upload();
	}
}

void ucDevelop::buildAndUpload()
{
	ui->statusBar->showMessage("Build and Upload");

	build();

	if(buildErrorCounter == 0)
		upload();
}

void ucDevelop::cleanProject()
{
	ui->statusBar->showMessage("Clean project");

	buildProcess = new QProcess(0);
	buildProcess->setWorkingDirectory(project.path.path());

	connect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(readProcessData()));
	connect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessData()));

	QStringList arg;
	arg << project.cleanCommand;

	buildProcess->start("make", arg);

	buildProcess->waitForFinished();

	delete buildProcess;

	ui->outputLed->setColor(Qt::green);
	ui->outputLabel->setText("Project is clean.");
}

void ucDevelop::rebuild()
{
	ui->statusBar->showMessage("Rebuild");

	cleanProject();
	build();
}

void ucDevelop::readProcessData()
{
	ui->statusBar->showMessage("Read process data");

	outputString.append(buildProcess->readAllStandardError());
	outputString.append(buildProcess->readAllStandardOutput());

	ui->output->setText(outputString);
	ui->output->verticalScrollBar()->setValue(ui->output->verticalScrollBar()->maximum());
}

void ucDevelop::clearOutput()
{
	ui->output->clear();
	outputString.clear();
}

void ucDevelop::settings()
{
	ui->statusBar->showMessage("Settings");
}

void ucDevelop::projectSettings()
{
	projectSettingsDialog * projectsettingsdialog = new projectSettingsDialog(this);

	projectsettingsdialog->setProjectToEdit(project);

	int result = projectsettingsdialog->exec();

	if(result == 1)
	{
		project = projectsettingsdialog->project;
	}
}

void ucDevelop::addFile()
{
	ui->statusBar->showMessage("Add file");

	bool ok;
	QString fileName = QInputDialog::getText(this, tr("Add new file..."),
						 tr("File name:"), QLineEdit::Normal,
						 "", &ok);
	if(project.fileList.contains(fileName, Qt::CaseSensitive))
	{
		QMessageBox msgBox;
		msgBox.setText("File " + fileName + " already exists. Enter a different name.");
		msgBox.exec();
		addFile();
	}
	else if (ok && !fileName.isEmpty())
	{
		ui->statusBar->showMessage(fileName);
		project.addFile(fileName);
	}
}

void ucDevelop::addClass()
{
	ui->statusBar->showMessage("Add class");

	bool ok;
	QString className = QInputDialog::getText(this, tr("Add new class..."),
						 tr("Class name:"), QLineEdit::Normal,
						 "", &ok);
	if(project.fileList.contains(className.toLower() + ".h", Qt::CaseSensitive)
		|| project.fileList.contains(className.toLower() + ".cpp", Qt::CaseSensitive))
	{
		QMessageBox msgBox;
		msgBox.setText("File " + className + " already exists. Enter a different name.");
		msgBox.exec();
		addClass();
	}
	else if (ok && className.isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Class name is empty!");
		msgBox.exec();
		addClass();
	}
	else if (ok && !className.isEmpty())
	{
		ui->statusBar->showMessage(className);
		project.addClass(className);
	}
}

void ucDevelop::importFile()
{
	ui->statusBar->showMessage("Import file");

	QString filename = QFileDialog::getOpenFileName(this,tr("Import file..."), "/home", tr("Source/Header (*.cpp *.c *.h)"));

	if(!filename.isEmpty())
		project.importFile(QUrl(filename));
}

void ucDevelop::importFolder()
{
	ui->statusBar->showMessage("Import folder");

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	project.importFolder(QUrl(dir), project.path);
	ui->statusBar->showMessage(dir);
}

void ucDevelop::closeProject()
{
	ui->statusBar->showMessage("Close project");


	ui->actionBuild->setEnabled(false);
	ui->actionUpload->setEnabled(false);
	ui->actionBuildAndUpload->setEnabled(false);
	ui->actionCleanProject->setEnabled(false);
	ui->actionRebuild->setEnabled(false);
	ui->actionAddClass->setEnabled(false);
	ui->actionAddFile->setEnabled(false);
	ui->actionImportFile->setEnabled(false);
	ui->actionImportFolder->setEnabled(false);
	ui->actionProjectSettings->setEnabled(false);
	ui->actionCloseProject->setEnabled(false);
	delete projectFileModel;
}

void ucDevelop::viewIsChanged()
{
	ui->statusBar->showMessage("Change view");
}

void ucDevelop::help()
{
	ui->statusBar->showMessage("Help");
}

void ucDevelop::about()
{
	ui->statusBar->showMessage("About");
}

void ucDevelop::fileListClicked()
{
	disconnect(doc.at(actualOpenDoc), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));

	actualOpenDoc = ui->fileWidget->currentRow();
	view = doc.at(actualOpenDoc)->createView(this);
	takeCentralWidget();
	setCentralWidget(view);

	ui->statusBar->showMessage("File list was clicked");

	connect(doc.at(actualOpenDoc), SIGNAL(documentSavedOrUploaded(KTextEditor::Document *,bool)), this, SLOT(documentSaved()));
}

void ucDevelop::documentChanged()
{
	ui->statusBar->showMessage("Document was changed");
	doc.at(actualOpenDoc)->setModified(false);

	ui->fileWidget->takeItem(actualOpenDoc);
	QString filename = doc.at(actualOpenDoc)->url().fileName();
	if(filename.isEmpty() == true)
		filename = "nonamed";
	filename.append("*");
	ui->fileWidget->insertItem(actualOpenDoc,filename);
	ui->fileWidget->setCurrentRow(actualOpenDoc);

	isBuild = false;
}

void ucDevelop::documentSaved()
{
	ui->statusBar->showMessage("Document was saved");

	ui->fileWidget->takeItem(actualOpenDoc);
	ui->fileWidget->insertItem(actualOpenDoc,doc.at(actualOpenDoc)->url().fileName());
	ui->fileWidget->setCurrentRow(actualOpenDoc);
}

void ucDevelop::documentSavedAt(int index)
{
	ui->statusBar->showMessage("Document was saved");

	ui->fileWidget->takeItem(index);
	ui->fileWidget->insertItem(index,doc.at(index)->url().fileName());
	ui->fileWidget->setCurrentRow(actualOpenDoc);

}

void ucDevelop::projectViewDoubleClick(QModelIndex index)
{
	ui->statusBar->showMessage(QString("Project tree was double clicked. Path: ").append(projectFileModel->filePath(index)));

	openURL(projectFileModel->filePath(index));
}
