#include "project.h"

Project::Project()
{

}

QString Project::getMMCU()
{
	return mcu.description.toLower();
}

void Project::setProjectData(QUrl projectUrl)
{
	projectFileUrl = projectUrl;
	path = projectUrl.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash);

	QFile projectFile(projectUrl.path());
	if (!projectFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream projectFileStream(&projectFile);
	while (!projectFileStream.atEnd())
	{
		QString projectFileString = projectFileStream.readLine();

		if(projectFileString.contains("projectName=",Qt::CaseInsensitive))
		{
			name = projectFileString.remove("projectName=");
		}
		else if(projectFileString.contains("mcu=",Qt::CaseInsensitive))
		{
			mcu.id = projectFileString.remove("mcu=");
		}
		else if(projectFileString.contains("clock=",Qt::CaseInsensitive))
		{
			mcu.clock = projectFileString.remove("clock=");
		}
		else if(projectFileString.contains("programmer=",Qt::CaseInsensitive))
		{
			programmer.id = projectFileString.remove("programmer=");
		}
		else if(projectFileString.contains("projectType=",Qt::CaseInsensitive))
		{
			projectType = projectFileString.remove("projectType=");
		}
		else if(projectFileString.contains("projectManager=",Qt::CaseInsensitive))
		{
			projectManager = projectFileString.remove("projectManager=");
		}
		else if(projectFileString.contains("buildCommand=",Qt::CaseInsensitive))
		{
			buildCommand = projectFileString.remove("buildCommand=");
		}
		else if(projectFileString.contains("uploadCommand=",Qt::CaseInsensitive))
		{
			uploadCommand = projectFileString.remove("uploadCommand=");
		}
		else if(projectFileString.contains("cleanCommand=",Qt::CaseInsensitive))
		{
			cleanCommand = projectFileString.remove("cleanCommand=");
		}
		else if(projectFileString.contains("autorName=",Qt::CaseInsensitive))
		{
			autorName = projectFileString.remove("autorName=");
		}
		else if(projectFileString.contains("fileList",Qt::CaseInsensitive))
		{
			fileList.clear();

			projectFileString = projectFileStream.readLine();
			while (!projectFileString.contains(";",Qt::CaseInsensitive))
			{
				projectFileString.remove("\t");
				fileList.append(projectFileString);
				projectFileString = projectFileStream.readLine();
			}
		}
	}
	programmer.uploadAllData();
	mcu.uploadAllData();
}

QString Project::showProjectData()
{
	QString str("Project name: ");
	str.append(name).append("\n");
	str.append("Project path: ").append(path.path()).append("\n");
	str.append("MCU descriptio: ").append(mcu.description).append("\n");
	str.append("MCU id: ").append(mcu.id).append("\n");
	str.append("MCU signature: ").append(mcu.signature).append("\n");
	str.append("MCU clock: ").append(mcu.clock).append("\n");
	str.append("Programmer description: ").append(programmer.description).append("\n");
	str.append("Programmer id: ").append(programmer.id).append("\n");
	str.append("File list:\n");
	for(int i = 0; i < fileList.count(); i++)
		str.append("\t-").append(fileList.at(i)).append("\n");

	str.append("Build command:").append(buildCommand).append("\n");
	str.append("Build command attribute:").append(buildCommandAttribute).append("\n");
	str.append("Upload command:").append(uploadCommand).append("\n");
	str.append("Upload command attribute:").append(uploadCommandAttribute).append("\n");
	str.append("Clean command:").append(cleanCommand).append("\n");
	str.append("Autor name:").append(autorName).append("\n");

	return str;
}

QList <MCU> Project::getMcuList()
{
	QList <MCU> List;
	QFile confFile("/etc/avrdude.conf");

	if (!confFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox msgBox;
		msgBox.setText("Unable to find file: /etc/avrdude.conf");
		msgBox.exec();
//		return;
	}

	QTextStream confFileStream(&confFile);

	while(!confFileStream.atEnd())
	{
		QString stringLine = confFileStream.readLine();

		if(stringLine.contains("#",Qt::CaseInsensitive))
			continue;
		else if(stringLine.contains("part",Qt::CaseInsensitive))
		{
			MCU mcu;

			stringLine = stringLine.trimmed();

			if(stringLine.contains("parent ",Qt::CaseInsensitive))
			{
				stringLine.remove(" ");
				stringLine.remove("part");
				stringLine.remove("parent");
				stringLine.remove("\"");
				stringLine.remove(";");

				mcu.parent = stringLine;
			}
			while (!stringLine.contains(";",Qt::CaseInsensitive) || stringLine.length() > 1)
			{
				stringLine = confFileStream.readLine();

				stringLine = stringLine.trimmed();

				if(stringLine.contains("id",Qt::CaseInsensitive) && mcu.id.isNull())
				{
					stringLine.remove(" ");
					stringLine.remove("	");
					stringLine.remove("id");
					stringLine.remove("=");
					stringLine.remove("\"");
					stringLine.remove(";");

					mcu.id = stringLine;
				}
				else if(stringLine.contains("desc",Qt::CaseInsensitive) && mcu.description.isNull())
				{
					stringLine.remove(" ");
					stringLine.remove("	");
					stringLine.remove("desc");
					stringLine.remove("=");
					stringLine.remove("\"");
					stringLine.remove(";");

					mcu.description = stringLine;
				}
				else if(stringLine.contains("signature",Qt::CaseInsensitive) && mcu.signature.isNull())
				{
					stringLine.remove("signature");
					stringLine.remove("=");
					stringLine.remove(";");
					stringLine = stringLine.trimmed();

					mcu.signature = stringLine;
				}

			}
			if(mcu.id.at(0) != ".")
				List.append(mcu);
		}
	}
	return List;
}

QList <Programmer> Project::getProgrammerList()
{
	QList <Programmer> List;
	QFile confFile("/etc/avrdude.conf");

	if (!confFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox msgBox;
		msgBox.setText("Unable to find file: /etc/avrdude.conf");
		msgBox.exec();
	//	return;
	}

	QTextStream confFileStream(&confFile);

	while(!confFileStream.atEnd())
	{
		QString stringLine = confFileStream.readLine();

		if(stringLine.contains("#",Qt::CaseInsensitive))
			continue;
		else if(stringLine.contains("programmer",Qt::CaseInsensitive))
		{
			Programmer programmer;
			while (!stringLine.contains(";",Qt::CaseInsensitive) || stringLine.length() > 1)
			{
				stringLine = confFileStream.readLine();

				if(stringLine.contains("#",Qt::CaseInsensitive))
					continue;

				if(stringLine.contains(" id ",Qt::CaseInsensitive))
				{
					stringLine.remove(" ");
					stringLine.remove("id=");
					stringLine.remove("\"");
					stringLine.remove(";");

					programmer.id = stringLine;
				}
				else if(stringLine.contains("desc",Qt::CaseInsensitive))
				{
					stringLine.remove("desc");
					stringLine.remove("=");
					stringLine.remove("\"");
					stringLine.remove(";");
					stringLine = stringLine.trimmed();
					programmer.description = stringLine;
				}

			}
			if(!programmer.description.isEmpty() && !programmer.description.isEmpty())
				List.append(programmer);
		}
	}

	return List;
}

void Project::addFile(QString fileName, QString text)
{
	//---------create new file
	QFile newFile(QString(path.path()).append("/").append(fileName));
	QFile newFileTemplate(QDir::current().path() + "/template/generalTemplate/newFile.template");

	if (!newFile.open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	if (!newFileTemplate.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream newFileStream(&newFile);
	QTextStream newFileTemplateStream(&newFileTemplate);

	QString templateString = newFileTemplateStream.readAll();

	//templateString.replace("[ucdevelopVersion]", ucdevelopVersion,Qt::CaseInsensitive);
	//templateString.replace("[projectName]", name,Qt::CaseInsensitive);
	//templateString.replace("[mcu]", mcu.id,Qt::CaseInsensitive);
	//templateString.replace("[mmcu]", getMMCU(),Qt::CaseInsensitive);
	//templateString.replace("[programmer]", programmer.id,Qt::CaseInsensitive);
	//templateString.replace("[programmerPort]", programmer.portName,Qt::CaseInsensitive);
	//templateString.replace("[clock]", mcu.clock,Qt::CaseInsensitive);
	//templateString.replace("[autorName]", autorName,Qt::CaseInsensitive);

	templateString = changeProjectTagOnData(templateString);

	newFileStream << templateString;
	newFileStream << text;
	newFile.close();

	fileList.append(fileName);		//add file to file list

	addFileToProject(fileName);
}

void Project::addClass(QString className)
{
	//---------create new source file
	QFile newClassSourceFile(path.path() + "/" + className.toLower() + ".cpp");
	QFile newClassSourceFileTemplate(QDir::current().path() + "/template/generalTemplate/newClass.cpptemplate");

	if (!newClassSourceFile.open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	if (!newClassSourceFileTemplate.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream newClassSourceFileStream(&newClassSourceFile);
	QTextStream newClassSourceFileTemplateStream(&newClassSourceFileTemplate);

	QString sourceFiletemplateString = newClassSourceFileTemplateStream.readAll();

	//sourceFiletemplateString.replace("[ucdevelopVersion]", ucdevelopVersion,Qt::CaseInsensitive);
	//sourceFiletemplateString.replace("[projectName]", name,Qt::CaseInsensitive);
	//sourceFiletemplateString.replace("[mcu]", mcu.id,Qt::CaseInsensitive);
	//sourceFiletemplateString.replace("[mmcu]", getMMCU(),Qt::CaseInsensitive);
	//sourceFiletemplateString.replace("[programmer]", programmer.id,Qt::CaseInsensitive);
	//sourceFiletemplateString.replace("[programmerPort]", programmer.portName,Qt::CaseInsensitive);
	//sourceFiletemplateString.replace("[clock]", mcu.clock,Qt::CaseInsensitive);
	//sourceFiletemplateString.replace("[autorName]", autorName,Qt::CaseInsensitive);
	sourceFiletemplateString.replace("[className]", className,Qt::CaseInsensitive);
	sourceFiletemplateString.replace("[classHeaderName]", className.toLower(),Qt::CaseInsensitive);

	sourceFiletemplateString = changeProjectTagOnData(sourceFiletemplateString);

	newClassSourceFileStream << sourceFiletemplateString;

	newClassSourceFile.close();
	newClassSourceFileTemplate.close();

	fileList.append(className.toLower() + ".cpp");		//add file to file list

	//---------create new header file
	QFile newClassHeaderFile(path.path() + "/" + className.toLower() + ".h");
	QFile newClassHeaderFileTemplate(QDir::current().path() + "/template/generalTemplate/newClass.htemplate");

	if (!newClassHeaderFile.open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	if (!newClassHeaderFileTemplate.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream newClassHeaderFileStream(&newClassHeaderFile);
	QTextStream newClassHeaderFileTemplateStream(&newClassHeaderFileTemplate);

	QString headerFiletemplateString = newClassHeaderFileTemplateStream.readAll();

	//headerFiletemplateString.replace("[ucdevelopVersion]", ucdevelopVersion,Qt::CaseInsensitive);
	//headerFiletemplateString.replace("[projectName]", name,Qt::CaseInsensitive);
	//headerFiletemplateString.replace("[mcu]", mcu.id,Qt::CaseInsensitive);
	//headerFiletemplateString.replace("[mmcu]", getMMCU(),Qt::CaseInsensitive);
	//headerFiletemplateString.replace("[programmer]", programmer.id,Qt::CaseInsensitive);
	//headerFiletemplateString.replace("[programmerPort]", programmer.portName,Qt::CaseInsensitive);
	//headerFiletemplateString.replace("[clock]", mcu.clock,Qt::CaseInsensitive);
	//headerFiletemplateString.replace("[autorName]", autorName,Qt::CaseInsensitive);
	headerFiletemplateString.replace("[className]", className,Qt::CaseInsensitive);

	headerFiletemplateString = changeProjectTagOnData(headerFiletemplateString);

	newClassHeaderFileStream << headerFiletemplateString;

	newClassHeaderFile.close();
	newClassHeaderFileTemplate.close();

	fileList.append(className.toLower() + ".h");		//add file to file list

	addFileToProject(className.toLower() + ".cpp");
	addFileToProject(className.toLower() + ".h");
}

void Project::importFile(QUrl url)
{
	QString fileName = url.fileName();

	QFile::copy(url.path(),path.path() + "/" + fileName);

	addFileToProject(fileName);
}

void Project::importFolder(QUrl importDirUrl, QUrl url)
{
	QDir dir(url.path());

	dir.mkdir(importDirUrl.fileName());
	dir.cd(importDirUrl.fileName());

	QDir dirToCopy(importDirUrl.path());

	importFolderRecursive(dirToCopy,dir);
}

void Project::importFolderRecursive(QDir importDir, QDir dir)
{
	QStringList dirList = importDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for(int i = 0; i < dirList.count(); i++)
	{
		QDir copyFrom(importDir.path() + "/" + dirList.at(i));
		QDir copyTo(dir.path() + "/" + dirList.at(i));

		dir.mkdir(dirList.at(i));
		dir.cd(dirList.at(i));

		importFolderRecursive(copyFrom,copyTo);

		dir.cdUp();
	}

	QStringList fileList = importDir.entryList(QDir::Files);

	for(int i = 0; i < fileList.count(); i++)
	{
		QFile::copy(importDir.path() + "/" + fileList.at(i), dir.path() + "/" + fileList.at(i));

		addFileToProject(dir.path().remove(path.path() + "/") + "/" + fileList.at(i));
	}
}

void Project::addFileToProject(QString fileName)
{
	//----------add new file to project file
	QFile projectFile(path.path().append("/").append(name).append(".ucdevelop"));
	if (!projectFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream projectFileStream(&projectFile);
	QString projectFileString = projectFileStream.readAll();
	projectFileString.replace(";",QString("\t").append(fileName).append("\n;"));

	projectFile.remove();

	if (!projectFile.open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	projectFileStream.setDevice(&projectFile);
	projectFileStream << projectFileString;
	projectFile.close();

	//----------add new file to make/(cmake) file
	if(projectManager == "make" && (fileName.contains(".c",Qt::CaseInsensitive) || fileName.contains(".cpp",Qt::CaseInsensitive)))
	{
		QFile makeFile(path.path().append("/").append("makefile"));
		if (!makeFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		QTextStream makeFileStream(&makeFile);
		QString makeFileString = makeFileStream.readAll();
		makeFileString.replace("SRC =",QString("SRC = ").append(fileName));

		makeFile.remove();

		if (!makeFile.open(QIODevice::ReadWrite | QIODevice::Text))
			return;

		makeFileStream.setDevice(&makeFile);
		makeFileStream << makeFileString;
		makeFile.close();
	}
	else if(projectManager == "cmake" && (fileName.contains(".c",Qt::CaseInsensitive) || fileName.contains(".cpp",Qt::CaseInsensitive)))
	{
		//instruction to add file to cmake
	}
}

QString Project::changeProjectTagOnData(QString str)
{
	str.replace("[ucdevelopVersion]", ucdevelopVersion,Qt::CaseInsensitive);
	str.replace("[projectName]", name,Qt::CaseInsensitive);
	str.replace("[mcu]", mcu.id,Qt::CaseInsensitive);
	str.replace("[mmcu]", getMMCU(),Qt::CaseInsensitive);
	str.replace("[programmer]", programmer.id,Qt::CaseInsensitive);
	str.replace("[programmerPort]", programmer.portName,Qt::CaseInsensitive);
	if(!programmer.portName.isEmpty())
		str.replace("#PORT_PROGRAMMER = -P", QString("PORT_PROGRAMMER = -P ").append(programmer.portName),Qt::CaseInsensitive);
	str.replace("[clock]", mcu.clock,Qt::CaseInsensitive);
	str.replace("[autorName]", autorName,Qt::CaseInsensitive);

	return str;
}
