#pragma once
#include <QDir>
#include <QString>
#include <QStringList>
#include <QFileInfoList>
#include <QList>

class ImageDir
{
public:
	ImageDir(QString dirPath, QStringList filters = { "*.jpg", "*.png", "*.jpeg", "*.tif", "*.bmp" });

	QList<QString> getImageNameList();
	QList<QString> getImagePathList();
	int imageNum();
	QString dirPath();
	QString dirName();

private:
	QString mDirPath;
	QFileInfoList mImageList;
};

