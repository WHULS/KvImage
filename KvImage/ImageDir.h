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

	QList<QString> getImageNameList() const;
	QList<QString> getImagePathList() const;
	int imageNum() const;
	QString dirPath() const;
	QString dirName() const;

private:
	QString mDirPath;
	QFileInfoList mImageList;
};

