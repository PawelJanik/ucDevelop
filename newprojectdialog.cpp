#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

newProjectDialog::newProjectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::newProjectDialog)
{
	ui->setupUi(this);

	templateDir.current();
	templateDir.cd("template");

	ui->templateComboBox->addItems(templateDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot));

	connect(ui->templateComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectTemplate()));

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(createProject()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	selectTemplate();
	setProgrammerList();
	setMCUList();
}

newProjectDialog::~newProjectDialog()
{
	delete ui;
}

void newProjectDialog::selectTemplate()
{
	selectedTemplateDir = templateDir;
	selectedTemplateDir.cd(ui->templateComboBox->currentText());

	QString filePath = selectedTemplateDir.path();
	filePath.append("/");
	filePath.append(ui->templateComboBox->currentText());
	filePath.append(".ucdeveloptemplate");

	templateFile = new QFile(filePath);

	if (!templateFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox msgBox;
		msgBox.setText("check the template files");
		msgBox.exec();
		return;
	}

	QTextStream templateFileStream(templateFile);

	while(!templateFileStream.atEnd())
	{
		QString stringLine = templateFileStream.readLine();

		if(stringLine.contains("templateDescription=",Qt::CaseInsensitive))
		{
			stringLine.remove("templateDescription=",Qt::CaseInsensitive);
			ui->descriptionTextEdit->setText(stringLine);
		}
		else if(stringLine.contains("fileList",Qt::CaseInsensitive))
		{
			project.fileList.clear();

			stringLine = templateFileStream.readLine();
			while (!stringLine.contains(";",Qt::CaseInsensitive))
			{
				stringLine.remove("\t");
				project.fileList.append(stringLine);
				stringLine = templateFileStream.readLine();
			}
		}
	}
	templateFile->close();
}

void newProjectDialog::setMCUList()
{
	mcuList = project.getMcuList();

	for(int i = 0; i < mcuList.count(); i++)
	{
		QString s(mcuList.at(i).description);
		s.append(" - ");
		s.append(mcuList.at(i).id);
		s.append(" - ");
		s.append(mcuList.at(i).signature);
		s.append(" - ");
		s.append(mcuList.at(i).parent);

		ui->mcuComboBox->addItem(s);
	}

}

void newProjectDialog::setProgrammerList()
{
	programmerList = project.getProgrammerList();

	for(int i = 0; i < programmerList.count(); i++)
	{
		QString s = programmerList.at(i).description;
		s.append(" - ");
		s.append(programmerList.at(i).id);
		ui->programmerComboBox->addItem(s);
	}
}

void newProjectDialog::createProject()
{
	QMessageBox msgBox;
	bool err = false;

	if(ui->projectNameEdit->text().isEmpty())
	{
		msgBox.setText("Enter the name of the project");
		msgBox.exec();
		err = true;
	}
	else
		project.name = ui->projectNameEdit->text();

	project.mcu = mcuList.at(ui->mcuComboBox->currentIndex());
	project.programmer = programmerList.at(ui->programmerComboBox->currentIndex());
	project.mcu.clock = ui->clockEdit->text();
	project.programmer.portName = ui->portEdit->text();
	if(ui->pathToProjectRequister->url().isEmpty())
	{
		msgBox.setText("Select project file");
		msgBox.exec();
		err = true;
	}
	else
		project.path = QUrl(QString(ui->pathToProjectRequister->text()).append("/").append(project.name));

	if(err == false)
	{
		QDir projectDir(ui->pathToProjectRequister->text());
		projectDir.mkdir(ui->projectNameEdit->text());
		projectDir.cd(ui->projectNameEdit->text());

		QStringList templateListFile = selectedTemplateDir.entryList(QDir::Files);

		for(int i = 0; i < templateListFile.count(); i++)
		{
			QString templateFileUrl = selectedTemplateDir.path();
			templateFileUrl.append("/");
			templateFileUrl.append(templateListFile.at(i));

			QString projectFileUrl = projectDir.path();
			projectFileUrl.append("/");

			if(templateListFile.at(i).contains(".ucdeveloptemplate"))
				projectFileUrl.append(project.name).append(".ucdevelop");
			else
				projectFileUrl.append(templateListFile.at(i));

			QFile::copy(templateFileUrl,projectFileUrl);

			ui->descriptionTextEdit->setText(templateFileUrl);
		}

		project.fileList.append(QString(project.name).append(".ucdevelop"));

		for(int i = 0; i < project.fileList.count(); i++)
		{
			QFile file(QString(projectDir.path().append("/").append(project.fileList.at(i))));

			if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
				return;

			QTextStream stream(&file);
			QString fileString = stream.readAll();

			file.remove();

			fileString.replace("[ucdevelopVersion]", ucdevelopVersion,Qt::CaseInsensitive);
			fileString.replace("[projectName]", project.name,Qt::CaseInsensitive);
			fileString.replace("[mcu]", project.mcu.id,Qt::CaseInsensitive);
			fileString.replace("[mmcu]", project.getMMCU(),Qt::CaseInsensitive);
			fileString.replace("[programmer]", project.programmer.id,Qt::CaseInsensitive);
			fileString.replace("[programmerPort]", project.programmer.portName,Qt::CaseInsensitive);
			if(!project.programmer.portName.isEmpty())
				fileString.replace("#PORT_PROGRAMMER = -P", QString("PORT_PROGRAMMER = -P ").append(project.programmer.portName),Qt::CaseInsensitive);
			fileString.replace("[clock]", project.mcu.clock,Qt::CaseInsensitive);

			if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
				return;
			stream.setDevice(&file);
			stream << fileString;
		}
	}

	if(err == false)
		this->accept();
}
