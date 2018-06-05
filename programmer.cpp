#include "programmer.h"

Programmer::Programmer()
{

}

void Programmer::uploadAllData()
{
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

		if(stringLine.contains(QString("\"").append(id).append("\""),Qt::CaseInsensitive))
		{
			while (!stringLine.contains(";",Qt::CaseInsensitive) || stringLine.length() > 1)
			{
				stringLine = confFileStream.readLine();

				stringLine = stringLine.trimmed();

				if(stringLine.contains("desc",Qt::CaseInsensitive) && description.isEmpty())
				{
					stringLine.remove(" ");
					stringLine.remove("	");
					stringLine.remove("desc");
					stringLine.remove("=");
					stringLine.remove("\"");
					stringLine.remove(";");

					description = stringLine;
				}

			}
		}
	}
}
