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
#include "px.h"
#include <QMessageBox>
#include "VectorWindow.h"
#include "KvNamespace.h"


class KvImage : public QMainWindow
{
	Q_OBJECT

public:
	KvImage(QWidget* parent = Q_NULLPTR);
	// 刷新布局
	void refreshLayout();
	// 在信息栏展示文字
	void putText(QString txt);
	// 初始化侧边栏
	void initSidebar();
	// 刷新侧边栏
	void refreshSidebar(QList<ImageDir>& imgDirList);
	// 初始化绘图状态（清空绘图信息、更新菜单选项）
	void initDrawStatus();
	// 检查矢量判断是否保存（返回值：是否保存矢量）
	Kv::VectorType checkVectorSave();

	void addPoint(const cv::Point2d& pt);
	void addLine(const cv::Vec4d& line);
	void addRectangle(const cv::Rect2d& rect);
	void addRotateRectangle(const RotRect2D& rotRect);
	// 删除矢量
	bool deletePoint(const int idx);
	bool deletePoint(const cv::Point2d& pt);
	bool deleteLine(const int idx);
	bool deleteLine(const cv::Vec4d& line);
	bool deleteRectangle(const int idx);
	bool deleteRectangle(const cv::Rect2d& rect);
	bool deleteRotateRectangle(const int idx);
	bool deleteRotateRectangle(const RotRect2D& rotRect);

	void exitApp(bool exitDirectly = false);

protected:
	void resizeEvent(QResizeEvent* evt);
	void closeEvent(QCloseEvent* evt);

private slots:
	void on_action_open_image_triggered();
	void on_action_open_directory_triggered();
	void onImageSelectChanged(const QModelIndex& curIdx, const QModelIndex& preIdx);
	void onImageMouseMoveEvent(px p);
	void on_action_exit_triggered();
	void on_action_draw_point_triggered();
	void on_action_draw_line_triggered();
	void on_action_draw_rectangle_triggered();
	void on_action_draw_rotate_rectangle_triggered();
	void onDrawNewPoint(const cv::Point2d& pt);
	void onDrawNewLine(const cv::Vec4d& line);
	void onDrawNewRectangle(const cv::Rect2d& rect);
	void onDrawNewRotateRectangle(const RotRect2D& rotRect);
	void on_action_vector_win_toggled(bool isShow);
	void onVectorWindowShow(bool isShow);
	void onVecWinSelectPoint(const int idx);
	void onVecWinSelectLine(const int idx);
	void onVecWinSelectRectangle(const int idx);
	void onVecWinSelectRotateRectangle(const int idx);
	void onVecWinDeletePoint(const int idx);
	void onVecWinDeleteLine(const int idx);
	void onVecWinDeleteRectangle(const int idx);
	void onVecWinDeleteRotateRectangle(const int idx);

private:
	Ui::KvImageClass ui;

	// 是否可见
	bool sideBarVisible,
		infoBarVisible;
	// 尺寸比例
	int sideBarWidth,
		infoBarHeight;
	
	// 控件
	ImageViewer* iViewer = Q_NULLPTR;
	QTreeView* mSideBar = Q_NULLPTR;
	QStandardItemModel* mSideBarModel = Q_NULLPTR;
	QTextBrowser* mInfoBar = Q_NULLPTR;

	// 图像文件夹列表
	QList<ImageDir> mImgDirList;

	// 矢量窗口和矢量
	VectorWindow* mVecWin = Q_NULLPTR;
	QList<cv::Point2d> mPts;
	QList<cv::Vec4d> mLines;
	QList<cv::Rect2d> mRects;
	QList<RotRect2D> mRotRects;
};
