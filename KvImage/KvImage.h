#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_KvImage.h"
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include "Transform.h"
#include <QLabel>
#include <QResizeEvent>
#include <QLayout>
#include <QTreeView>
#include <QHeaderView>
#include <QTextBrowser>
#include <QDesktopWidget>
#include <QDateTime>
#include "ImageViewer.h"
#include <vector>
#include <QStandardItemModel>
#include "ImageDir.h"
#include <QItemSelection>

class KvImage : public QMainWindow
{
	Q_OBJECT

public:
	KvImage(QWidget* parent = Q_NULLPTR);
	void refreshLayout();  // 刷新布局
	void putText(QString txt);  // 在信息栏展示文字
	void initSidebar();  // 初始化侧边栏
	void refreshSidebar(QList<ImageDir>& imgDirList);  // 刷新侧边栏

protected:
	void resizeEvent(QResizeEvent* evt);

private slots:
	void on_action_open_image_triggered();
	void on_action_open_directory_triggered();
	void onImageSelectChanged(const QModelIndex& curIdx, const QModelIndex& preIdx);

private:
	Ui::KvImageClass ui;

	// 是否可见
	bool sideBarVisible,
		infoBarVisible;
	// 尺寸比例
	int sideBarWidth,
		infoBarHeight;
	
	// 控件
	ImageViewer* iViewer;
	QTreeView* mSideBar;
	QStandardItemModel* mSideBarModel;
	QTextBrowser* mInfoBar;

	// 图像文件夹列表
	QList<ImageDir> mImgDirList;
};
