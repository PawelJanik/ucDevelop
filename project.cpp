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

	name = "znawa";

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
		else if(projectFileString.contains("programmer=",Qt::CaseInsensitive))
		{
			programmer.id = projectFileString.remove("programmer=");
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
	str.append("Programmer description: ").append(programmer.description).append("\n");
	str.append("Programmer id: ").append(programmer.id).append("\n");
	str.append("File list:\n");
	for(int i = 0; i < fileList.count(); i++)
		str.append("\t-").append(fileList.at(i)).append("\n");

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
