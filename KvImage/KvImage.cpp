#include "KvImage.h"

KvImage::KvImage(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// 设置窗体大小
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect deskRect = desktopWidget->availableGeometry();  // 可用桌面大小
	int deskHeight, deskWidth;
	deskHeight = deskRect.height();
	deskWidth = deskRect.width();
	int winHeight, winWidth;
	winHeight = deskHeight * 0.8;
	winWidth = deskWidth * 0.8;
	this->setGeometry(deskWidth / 2 - winWidth / 2, deskHeight / 2 - winHeight / 2, winWidth, winHeight);

	// 初始化变量
	this->infoBarVisible = this->sideBarVisible = true;
	this->sideBarWidth = winWidth * 0.2;
	this->infoBarHeight = winHeight * 0.2;
	this->sideBarWidth = this->sideBarWidth > 500 ? 500 : this->sideBarWidth;
	this->infoBarHeight = this->infoBarHeight > 300 ? 300 : this->infoBarHeight;

	// 添加控件
	this->iViewer = new ImageViewer(ui.centralWidget);    // 图片显示窗口
	this->mSideBar = new QTreeView(ui.centralWidget);     // 侧边栏
	this->mInfoBar = new QTextBrowser(ui.centralWidget);  // 信息栏

	this->refreshLayout();

	this->mInfoBar->setOpenExternalLinks(true);

	this->putText("Start App");

	// 初始化侧边栏
	this->initSidebar();
	// 绑定信号和槽
	connect(this->mSideBar->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &KvImage::onImageSelectChanged);
	connect(this->iViewer, SIGNAL(imageMouseMoveEvent(px)), this, SLOT(onImageMouseMoveEvent(px)));
}

void KvImage::refreshLayout()
{
	qDebug() << "KvImage::refreshLayout()";

	// 获取中心控件的宽高
	QSize cSize = ui.centralWidget->size();
	int cw, ch;
	cw = cSize.width();
	ch = cSize.height();

	// 计算各个控件的矩形
	QRect imgRect,
		sideRect,
		infoRect;

	// 设置侧边栏
	sideRect.setX(0); sideRect.setY(0);
	sideRect.setWidth(this->sideBarVisible ? this->sideBarWidth : 0);
	sideRect.setHeight(ch);

	// 设置底部信息栏
	infoRect.setX(sideRect.width());
	if (this->infoBarVisible)
	{
		infoRect.setY(ch - this->infoBarHeight);
		infoRect.setHeight(this->infoBarHeight);
	}
	else
	{
		infoRect.setY(ch);
		infoRect.setHeight(0);
	}
	infoRect.setWidth(cw - sideRect.width());

	// 设置图片区域
	imgRect.setX(sideRect.width());
	imgRect.setY(0);
	imgRect.setWidth(cw - sideRect.width());
	imgRect.setHeight(ch - infoRect.height());

	// 应用矩形位置
	this->mSideBar->setGeometry(sideRect);
	this->mInfoBar->setGeometry(infoRect);
	this->iViewer->setGeometry(imgRect);
}


void KvImage::on_action_open_image_triggered()
{
	// 通过对话框获取文件路径
	QString caption = QString::fromLocal8Bit("选择一个文件打开");
	QString dir = "";  // 为空默认记忆上一次打开的路径
	QString filter("Image (*.png *.jpg *.jpeg *.tif *.bmp)");
	QString fileName = QFileDialog::getOpenFileName(
		this, caption, dir, filter
	);

	if (fileName.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("KvImage::on_action_open_image_triggered - 未选择图片路径");
		return;
	}

	cv::Mat img = cv::imread(fileName.toLocal8Bit().toStdString());
	this->iViewer->openImage(img);
	this->putText(QString::fromLocal8Bit("打开图像：%1 (width=%2, height=%3)")
		.arg(fileName).arg(img.cols).arg(img.rows));
	this->ui.statusBar->showMessage(QString("%1 (width = %2, height = %3)").arg(fileName).arg(img.cols).arg(img.rows));
}

void KvImage::resizeEvent(QResizeEvent* evt)
{
	this->refreshLayout();
}

void KvImage::on_action_open_directory_triggered()
{
	QString dirPath = QFileDialog::getExistingDirectory(this,
		QString::fromLocal8Bit("打开文件夹"));

	if (dirPath.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("KvImage::on_action_open_directory_triggered() - 未选择文件夹");
		return;
	}

	for (int i = 0; i < this->mImgDirList.size(); i++)
	{
		if (dirPath == this->mImgDirList[i].dirPath())
		{
			qWarning() << QString::fromLocal8Bit("%1 已打开").arg(dirPath);
			return;
		}
	}

	this->mImgDirList.append(ImageDir(dirPath));

	this->refreshSidebar(this->mImgDirList);
	this->putText(QString::fromLocal8Bit("打开文件夹 - %1").arg(dirPath));
}

void KvImage::onImageSelectChanged(const QModelIndex& curIdx, const QModelIndex& preIdx)
{
	// 如果没有父节点，说明是根节点
	QModelIndex parentIdx = curIdx.parent();
	if (!parentIdx.isValid())
	{
		return;
	}

	// 获取图片路径
	QStandardItem* item = ((QStandardItemModel*)this->mSideBar->model())->itemFromIndex(curIdx);
	QString imgPath = QString("%1/%2")
		.arg(this->mImgDirList[parentIdx.row()].dirPath())
		.arg(item->text());
	
	qDebug() << QString::fromLocal8Bit("KvImage::onImageSelectChanged() - 选择图像: %1")
		.arg(imgPath);

	cv::Mat img = cv::imread(imgPath.toLocal8Bit().toStdString());
	this->iViewer->openImage(img);
	this->ui.statusBar->showMessage(QString("%1 (width = %2, height = %3)").arg(imgPath).arg(img.cols).arg(img.rows));
}

void KvImage::onImageMouseMoveEvent(px p)
{
	if (ui.statusBar)
	{
		ui.statusBar->showMessage(QString("x=%1,y=%2,color=(%3)")
			.arg(p.x).arg(p.y)
			.arg(p.channels == 3 ? QString("r=%1,g=%2,b=%3").arg(p.r).arg(p.g).arg(p.b) : QString("%1").arg(p.r)));
	}
}

void KvImage::on_action_draw_point_triggered()
{
	if (!this->iViewer->hasImage())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请先打开一幅图像"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawPoint();

		// 将其他的设置为不可选中
		foreach (QAction *action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;
			
			if (action == this->ui.action_draw_point) continue;

			action->setEnabled(false);
		}

		// 修改文字
		this->ui.action_draw_point->setText(QString::fromLocal8Bit("停止画点"));

		// 设置加粗样式
		QFont font = this->ui.action_draw_point->font();
		font.setBold(true);
		this->ui.action_draw_point->setFont(font);
	}
	else if (this->iViewer->isDrawingPoint())
	{
		this->iViewer->stopDrawPoint();

		// 恢复其他的可选中状态
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_point) continue;

			action->setEnabled(true);
		}

		// 恢复文字内容
		this->ui.action_draw_point->setText(QString::fromLocal8Bit("画点"));

		// 恢复字体样式
		QFont font = this->ui.action_draw_point->font();
		font.setBold(false);
		this->ui.action_draw_point->setFont(font);
	}
}

void KvImage::on_action_draw_line_triggered()
{
	if (!this->iViewer->hasImage())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请先打开一幅图像"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawLine();

		// 将其他的设置为不可选中
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_line) continue;

			action->setEnabled(false);
		}

		// 修改文字
		this->ui.action_draw_line->setText(QString::fromLocal8Bit("停止绘制直线"));

		// 设置加粗样式
		QFont font = this->ui.action_draw_line->font();
		font.setBold(true);
		this->ui.action_draw_line->setFont(font);
	}
	else if (this->iViewer->isDrawingLine())
	{
		this->iViewer->stopDrawLine();

		// 恢复其他的可选中状态
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_line) continue;

			action->setEnabled(true);
		}

		// 恢复文字内容
		this->ui.action_draw_line->setText(QString::fromLocal8Bit("绘制直线"));

		// 恢复字体样式
		QFont font = this->ui.action_draw_line->font();
		font.setBold(false);
		this->ui.action_draw_line->setFont(font);
	}
}

void KvImage::on_action_draw_rectangle_triggered()
{
	if (!this->iViewer->hasImage())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请先打开一幅图像"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawRectangle();

		// 将其他的设置为不可选中
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rectangle) continue;

			action->setEnabled(false);
		}

		// 修改文字
		this->ui.action_draw_rectangle->setText(QString::fromLocal8Bit("停止绘制矩形"));

		// 设置加粗样式
		QFont font = this->ui.action_draw_rectangle->font();
		font.setBold(true);
		this->ui.action_draw_rectangle->setFont(font);
	}
	else if (this->iViewer->isDrawingRectangle())
	{
		this->iViewer->stopDrawRectangle();

		// 恢复其他的可选中状态
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rectangle) continue;

			action->setEnabled(true);
		}

		// 恢复文字内容
		this->ui.action_draw_rectangle->setText(QString::fromLocal8Bit("绘制矩形"));

		// 恢复字体样式
		QFont font = this->ui.action_draw_rectangle->font();
		font.setBold(false);
		this->ui.action_draw_rectangle->setFont(font);
	}
}

void KvImage::on_action_draw_rotate_rectangle_triggered()
{
	if (!this->iViewer->hasImage())
	{
		QMessageBox::information(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("请先打开一幅图像"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawRotateRectangle();

		// 将其他的设置为不可选中
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rotate_rectangle) continue;

			action->setEnabled(false);
		}

		// 修改文字
		this->ui.action_draw_rotate_rectangle->setText(QString::fromLocal8Bit("停止绘制旋转矩形"));

		// 设置加粗样式
		QFont font = this->ui.action_draw_rotate_rectangle->font();
		font.setBold(true);
		this->ui.action_draw_rotate_rectangle->setFont(font);
	}
	else if (this->iViewer->isDrawingRotateRectangle())
	{
		this->iViewer->stopDrawRotateRectangle();

		// 恢复其他的可选中状态
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rotate_rectangle) continue;

			action->setEnabled(true);
		}

		// 恢复文字内容
		this->ui.action_draw_rotate_rectangle->setText(QString::fromLocal8Bit("绘制旋转矩形"));

		// 恢复字体样式
		QFont font = this->ui.action_draw_rotate_rectangle->font();
		font.setBold(false);
		this->ui.action_draw_rotate_rectangle->setFont(font);
	}
}

void KvImage::putText(QString txt)
{
	if (!this->mInfoBar) return;

	QString logStr = QString("(%1) - %2\n")
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"))  // 时间
		.arg(txt);  // 消息内容

	this->mInfoBar->textCursor().insertText(logStr);
}

void KvImage::initSidebar()
{
	// 侧边栏 单元格属性设置
	this->mSideBar->setEditTriggers(QTreeView::NoEditTriggers);   //单元格不能编辑
	this->mSideBar->setSelectionBehavior(QTreeView::SelectRows);  //一次选中整行
	this->mSideBar->setSelectionMode(QTreeView::SingleSelection); //单选，配合上面的整行就是一次选单行
	this->mSideBar->setAlternatingRowColors(true);                //每间隔一行颜色不一样，当有qss时该属性无效
	this->mSideBar->setFocusPolicy(Qt::NoFocus);                  //去掉鼠标移到单元格上时的虚线框
	this->mSideBar->header()->hide();

	// 初始化侧边栏模型
	this->mSideBarModel = new QStandardItemModel(this->mSideBar);
	
	// 应用模型
	this->mSideBar->setModel(this->mSideBarModel);
}

void KvImage::refreshSidebar(QList<ImageDir>& imgDirList)
{
	if (!this->mSideBarModel)
	{
		this->initSidebar();
	}

	// 清空数据
	this->mSideBarModel->clear();

	// 重新添加数据
	int i;
	for (i = 0; i < imgDirList.size(); i++)
	{
		QStandardItem* dirItem = new QStandardItem(imgDirList[i].dirName());
		this->mSideBarModel->appendRow(dirItem);

		QStringList imgNameList = imgDirList[i].getImageNameList();
		for (int j = 0; j < imgNameList.size(); j++)
		{
			dirItem->appendRow(new QStandardItem(imgNameList.at(j)));
		}
	}

	// 展开最新添加的一行
	QModelIndex rootIndex = this->mSideBar->rootIndex();
	QModelIndex lastRowIndex = this->mSideBarModel->index(i - 1, 0, rootIndex);
	this->mSideBar->expand(lastRowIndex);
}
