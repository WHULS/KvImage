#include "ImageDir.h"

ImageDir::ImageDir(QString dirPath, QStringList filters)
{
	this->mDirPath = dirPath;

	QDir* dir = new QDir(dirPath);
	this->mImageList = dir->entryInfoList(filters);
}

QList<QString> ImageDir::getImageNameList()
{
	if (this->mDirPath.isEmpty() || this->mImageList.isEmpty())
	{
		return QList<QString>();
	}

	QList<QString> nameList;
	for (int i = 0; i < this->mImageList.size(); i++)
	{
		nameList << this->mImageList.at(i).fileName();
	}
	return nameList;
}

QList<QString> ImageDir::getImagePathList()
{
	if (this->mDirPath.isEmpty() || this->mImageList.isEmpty())
	{
		return QList<QString>();
	}

	QList<QString> pathList;
	for (int i = 0; i < this->mImageList.size(); i++)
	{
		pathList << this->mImageList.at(i).absoluteFilePath();
	}
	return pathList;
}

int ImageDir::imageNum()
{
	if (this->mDirPath.isEmpty() || this->mImageList.isEmpty())
	{
		return 0;
	}

	return this->mImageList.size();
}

QString ImageDir::dirPath()
{
	return this->mDirPath;
}

QString ImageDir::dirName()
{
	QStringList dirPathSplitList = this->mDirPath.split("/");
	if (dirPathSplitList.size() == 0)
	{
		return QString();
	}

	return dirPathSplitList.last();
}
