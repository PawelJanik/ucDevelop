#include "projectsettingsdialog.h"
#include "ui_projectsettingsdialog.h"

projectSettingsDialog::projectSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::projectSettingsDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, SIGNAL(accepted()),this, SLOT(editProject()));

	mcuList = project.getMcuList();
	programmerList = project.getProgrammerList();

	for(int i = 0; i < mcuList.count(); i++)
	{
		QString s(mcuList.at(i).description);
		ui->mcuBox->addItem(s);
	}

	for(int i = 0; i < programmerList.count(); i++)
	{
		QString s = programmerList.at(i).description;
		ui->programmerBox->addItem(s);
	}
}

projectSettingsDialog::~projectSettingsDialog()
{
	delete ui;
}

void projectSettingsDialog::setProjectToEdit(Project projectToEdit)
{
	project = projectToEdit;

	ui->autorNameEdit->setText(project.autorName);

	int mcuIndex;
	for(int i = 0; i < mcuList.count(); i++)
	{
		if(mcuList.at(i).id == project.mcu.id)
		{
			mcuIndex = i;
			break;
		}
	}
	ui->mcuBox->setCurrentIndex(mcuIndex);

	int programmerIndex;
	for(int i = 0; i < programmerList.count(); i++)
	{
		if(programmerList.at(i).id == project.programmer.id)
		{
			programmerIndex = i;
			break;
		}
	}
	ui->programmerBox->setCurrentIndex(programmerIndex);

	int clockIndex = ui->clockBox->findText(project.mcu.clock);
	if(clockIndex >= 0)
	{
		ui->clockBox->setCurrentIndex(clockIndex);
	}
	else if(clockIndex == -1)
	{
		ui->clockBox->addItem(project.mcu.clock);
		ui->clockBox->setCurrentIndex(ui->clockBox->count()-1);
	}

	if(!project.programmer.portName.isEmpty())
	{
		ui->portBox->addItem(project.programmer.portName);
		ui->clockBox->setCurrentIndex(ui->clockBox->count()-1);
	}
}

void projectSettingsDialog::editProject()
{
	project.mcu = mcuList.at(ui->mcuBox->currentIndex());
	project.mcu.clock = ui->clockBox->currentText();

	project.programmer = programmerList.at(ui->programmerBox->currentIndex());

	project.programmer.portName = ui->portBox->currentText();

	project.autorName = ui->autorNameEdit->text();

	//----------------project file edit-----------------------------

	QFile projectFile(project.path.path() + "/" + project.name + ".ucdevelop");
	if(!projectFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream stream(&projectFile);
	QList <QString> stringList;
	while (!stream.atEnd())
		stringList.append(stream.readLine());

	projectFile.remove();

	if(!projectFile.open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	stream.setDevice(&projectFile);

	for(int i = 0; i < stringList.count(); i++)
	{
		if(stringList.at(i).contains("mcu="))
			stream << "mcu=" << project.mcu.id << "\n";
		else if(stringList.at(i).contains("clock="))
			stream << "clock=" << project.mcu.clock << "\n";
		else if(stringList.at(i).contains("programmer="))
			stream << "programmer=" << project.programmer.id<< "\n";
		else if(stringList.at(i).contains("programmerPort="))
			stream << "programmerPort=" << project.programmer.portName<< "\n";
		else if(stringList.at(i).contains("autorName="))
			stream << "autorName=" << project.autorName<< "\n";
		else
			stream << stringList.at(i) << "\n";
	}
	projectFile.close();

	//----------------makefile edit-----------------------------

	QFile makeFile(project.path.path() + "/makefile");
	if(!makeFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream makeFileStream(&makeFile);
	stringList.clear();
	while (!makeFileStream.atEnd())
		stringList.append(makeFileStream.readLine());

	makeFile.remove();

	if(!makeFile.open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	makeFileStream.setDevice(&makeFile);

	for(int i = 0; i < stringList.count(); i++)
	{
		if(stringList.at(i).contains("MCU ="))
			makeFileStream << "MCU = " << project.getMMCU() << "\n";
		else if(stringList.at(i).contains("CLK ="))
			makeFileStream << "CLK = " << project.mcu.clock << "\n";
		else if(stringList.at(i).contains("PRGRAMMER ="))
			makeFileStream << "PRGRAMMER = " << project.programmer.id<< "\n";
		else if(stringList.at(i).contains("PORT_PROGRAMMER ="))
		{
			if(ui->portBox->currentText().isEmpty())
				makeFileStream << "#PORT_PROGRAMMER = -P" << "\n";
			else
				makeFileStream << "PORT_PROGRAMMER = -P " << project.programmer.portName << "\n";
		}
		else
			makeFileStream << stringList.at(i) << "\n";
	}
	makeFile.close();

	this->accept();
}
