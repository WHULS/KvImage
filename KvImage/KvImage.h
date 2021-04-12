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
	void refreshLayout();  // ˢ�²���
	void putText(QString txt);  // ����Ϣ��չʾ����
	void initSidebar();  // ��ʼ�������
	void refreshSidebar(QList<ImageDir>& imgDirList);  // ˢ�²����

protected:
	void resizeEvent(QResizeEvent* evt);

private slots:
	void on_action_open_image_triggered();
	void on_action_open_directory_triggered();
	void onImageSelectChanged(const QModelIndex& curIdx, const QModelIndex& preIdx);

private:
	Ui::KvImageClass ui;

	// �Ƿ�ɼ�
	bool sideBarVisible,
		infoBarVisible;
	// �ߴ����
	int sideBarWidth,
		infoBarHeight;
	
	// �ؼ�
	ImageViewer* iViewer;
	QTreeView* mSideBar;
	QStandardItemModel* mSideBarModel;
	QTextBrowser* mInfoBar;

	// ͼ���ļ����б�
	QList<ImageDir> mImgDirList;
};
