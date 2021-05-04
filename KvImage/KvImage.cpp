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
	QObject::connect(this->mSideBar->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &KvImage::onImageSelectChanged);
	QObject::connect(this->iViewer, SIGNAL(imageMouseMoveEvent(px)), this, SLOT(onImageMouseMoveEvent(px)));
	QObject::connect(this->iViewer, SIGNAL(drawNewPoint(const cv::Point2d&)), this, SLOT(onDrawNewPoint(const cv::Point2d&)));
	QObject::connect(this->iViewer, SIGNAL(drawNewLine(const cv::Vec4d &)), this, SLOT(onDrawNewLine(const cv::Vec4d&)));
	QObject::connect(this->iViewer, SIGNAL(drawNewRectangle(const cv::Rect2d &)), this, SLOT(onDrawNewRectangle(const cv::Rect2d &)));
	QObject::connect(this->iViewer, SIGNAL(drawNewRotateRectangle(const RotRect2D &)), this, SLOT(onDrawNewRotateRectangle(const RotRect2D &)));

	// 创建矢量窗口
	this->mVecWin = new VectorWindow();
	this->ui.action_vector_win->setCheckable(true);
	// 绑定窗口显示的事件（与菜单栏同步）
	QObject::connect(this->mVecWin, SIGNAL(vectorWindowShow(bool)), this, SLOT(onVectorWindowShow(bool)));
	// 绑定矢量选择事件	
	QObject::connect(this->mVecWin, SIGNAL(selectPoint(const int)), this, SLOT(onVecWinSelectPoint(const int)));
	QObject::connect(this->mVecWin, SIGNAL(selectLine(const int)), this, SLOT(onVecWinSelectLine(const int)));
	QObject::connect(this->mVecWin, SIGNAL(selectRectangle(const int)), this, SLOT(onVecWinSelectRectangle(const int)));
	QObject::connect(this->mVecWin, SIGNAL(selectRotateRectangle(const int)), this, SLOT(onVecWinSelectRotateRectangle(const int)));
	// 绑定矢量删除事件	
	QObject::connect(this->mVecWin, SIGNAL(deletePoint(const int)), this, SLOT(onVecWinDeletePoint(const int)));
	QObject::connect(this->mVecWin, SIGNAL(deleteLine(const int)), this, SLOT(onVecWinDeleteLine(const int)));
	QObject::connect(this->mVecWin, SIGNAL(deleteRectangle(const int)), this, SLOT(onVecWinDeleteRectangle(const int)));
	QObject::connect(this->mVecWin, SIGNAL(deleteRotateRectangle(const int)), this, SLOT(onVecWinDeleteRotateRectangle(const int)));
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
	Kv::VectorType vType = this->checkVectorSave();

	// 通过对话框获取文件路径
	QString caption = QString::fromLocal8Bit("选择一个文件打开");
	QString dir = "";  // 为空默认记忆上一次打开的路径
	QString filter("Image (*.png *.jpg *.jpeg *.tif *.bmp)");
	QString fileName = QFileDialog::getOpenFileName(
		this, caption, dir, filter
	);

	if (fileName.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("未选择图片路径");
		return;
	}

	cv::Mat img = cv::imread(fileName.toLocal8Bit().toStdString());
	switch (vType) {
	case Kv::VectorType::Point:
		this->iViewer->openImageWithPoints(img, this->mPts);
		break;
	case Kv::VectorType::Line:
		this->iViewer->openImageWithLines(img, this->mLines);
		break;
	case Kv::VectorType::Rectangle:
		this->iViewer->openImageWithRectangles(img, this->mRects);
		break;
	case Kv::VectorType::RotateRectangle:
		this->iViewer->openImageWithRotateRectangles(img, this->mRotRects);
		break;
	case Kv::VectorType::None:
	default:
		this->iViewer->openImage(img);
		break;
	}
	this->putText(QString::fromLocal8Bit("打开图像：%1 (width=%2, height=%3)")
		.arg(fileName).arg(img.cols).arg(img.rows));
	this->ui.statusBar->showMessage(QString("%1 (width = %2, height = %3)").arg(fileName).arg(img.cols).arg(img.rows));
}

void KvImage::resizeEvent(QResizeEvent* evt)
{
	this->refreshLayout();
}

void KvImage::closeEvent(QCloseEvent* evt)
{
	this->exitApp();

	// 忽视原有事件，自定义控制流
	evt->ignore();
}

void KvImage::on_action_open_directory_triggered()
{
	QString dirPath = QFileDialog::getExistingDirectory(this,
		QString::fromLocal8Bit("打开文件夹"));

	if (dirPath.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("未选择文件夹");
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

	Kv::VectorType vType = this->checkVectorSave();

	// 获取图片路径
	QStandardItem* item = ((QStandardItemModel*)this->mSideBar->model())->itemFromIndex(curIdx);
	QString imgPath = QString("%1/%2")
		.arg(this->mImgDirList[parentIdx.row()].dirPath())
		.arg(item->text());
	
	qDebug() << QString::fromLocal8Bit("选择图像: %1")
		.arg(imgPath);

	cv::Mat img = cv::imread(imgPath.toLocal8Bit().toStdString());
	switch (vType) {
	case Kv::VectorType::Point:
		this->iViewer->openImageWithPoints(img, this->mPts);
		break;
	case Kv::VectorType::Line:
		this->iViewer->openImageWithLines(img, this->mLines);
		break;
	case Kv::VectorType::Rectangle:
		this->iViewer->openImageWithRectangles(img, this->mRects);
		break;
	case Kv::VectorType::RotateRectangle:
		this->iViewer->openImageWithRotateRectangles(img, this->mRotRects);
		break;
	case Kv::VectorType::None:
	default:
		this->iViewer->openImage(img);
		break;
	}
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

void KvImage::on_action_exit_triggered()
{
	this->exitApp();
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

		// 初始化矢量窗口
		this->mVecWin->initShowPoints();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingPoint())
	{
		// 检查矢量
		this->checkVectorSave();

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

		// 初始化矢量窗口
		this->mVecWin->initShowLines();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingLine())
	{
		// 检查矢量
		this->checkVectorSave();

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

		// 初始化矢量窗口
		this->mVecWin->initShowRectangles();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingRectangle())
	{
		// 检查矢量
		this->checkVectorSave();

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

		// 初始化矢量窗口
		this->mVecWin->initShowRotateRectangles();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingRotateRectangle())
	{
		// 检查矢量
		this->checkVectorSave();

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

void KvImage::onDrawNewPoint(const cv::Point2d& pt)
{
	this->addPoint(pt);
	this->putText(QString("Add Point: x=%1, y=%2").arg(pt.x).arg(pt.y));
}

void KvImage::onDrawNewLine(const cv::Vec4d& line)
{
	this->addLine(line);
	this->putText(QString("Add Line: (x1, y1)=(%1, %2), (x2, y2)=(%3, %4)")
		.arg(line[0]).arg(line[1])
		.arg(line[2]).arg(line[3]));
}

void KvImage::onDrawNewRectangle(const cv::Rect2d& rect)
{
	this->addRectangle(rect);
	this->putText(QString("Add Rectangle: x=%1, y=%2, width=%3, height=%4")
		.arg(rect.x).arg(rect.y)
		.arg(rect.width).arg(rect.height));
}

void KvImage::onDrawNewRotateRectangle(const RotRect2D& rotRect)
{
	this->addRotateRectangle(rotRect);
	this->putText(QString("Add Rotate Rectangle: (x1, y1)=(%1, %2), (x2, y2)=(%3, %4), (x3, y3)=(%5, %6), (x4, y4)=(%7, %8)")
		.arg(rotRect.pt1().x).arg(rotRect.pt1().y)
		.arg(rotRect.pt2().x).arg(rotRect.pt2().y)
		.arg(rotRect.pt3().x).arg(rotRect.pt3().y)
		.arg(rotRect.pt4().x).arg(rotRect.pt4().y));
}

void KvImage::on_action_vector_win_toggled(bool isShow)
{
	if (this->mVecWin == Q_NULLPTR)
	{
		return;
	}
	if (isShow)
	{
		this->mVecWin->show();
	}
	else
	{
		this->mVecWin->hide();
	}
}

void KvImage::onVectorWindowShow(bool isShow)
{
	this->ui.action_vector_win->setChecked(isShow);
}

void KvImage::onVecWinSelectPoint(const int idx)
{
	if (idx >= this->mPts.size())
	{
		qWarning() << "Out of range";
		return;
	}
	this->iViewer->zoomToPoint(idx);
}

void KvImage::onVecWinSelectLine(const int idx)
{
	if (idx >= this->mLines.size())
	{
		qWarning() << "Out of range";
		return;
	}
	this->iViewer->zoomToLine(idx);
}

void KvImage::onVecWinSelectRectangle(const int idx)
{
	if (idx >= this->mRects.size())
	{
		qWarning() << "Out of range";
		return;
	}
	this->iViewer->zoomToRectangle(idx);
}

void KvImage::onVecWinSelectRotateRectangle(const int idx)
{
	if (idx >= this->mRotRects.size())
	{
		qWarning() << "Out of range";
		return;
	}
	this->iViewer->zoomToRotateRectangle(idx);
}

void KvImage::onVecWinDeletePoint(const int idx)
{
	this->deletePoint(idx);
	this->iViewer->deletePoint(idx);
}

void KvImage::onVecWinDeleteLine(const int idx)
{
	this->deleteLine(idx);
	this->iViewer->deleteLine(idx);
}

void KvImage::onVecWinDeleteRectangle(const int idx)
{
	this->deleteRectangle(idx);
	this->iViewer->deleteRectangle(idx);
}

void KvImage::onVecWinDeleteRotateRectangle(const int idx)
{
	this->deleteRotateRectangle(idx);
	this->iViewer->deleteRotateRectangle(idx);
}

void KvImage::putText(QString txt)
{
	if (!this->mInfoBar) return;

	QString logStr = QString("(%1) - %2\n")
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"))  // 时间
		.arg(txt);  // 消息内容

	this->mInfoBar->textCursor().insertText(logStr);
	this->mInfoBar->moveCursor(QTextCursor::End);  // 滚动到底部
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

void KvImage::initDrawStatus()
{
	if (this->iViewer->isDrawing())
	{
		this->iViewer->stopDraw();
		
		this->ui.action_draw_point->setText(QString::fromLocal8Bit("画点"));
		this->ui.action_draw_line->setText(QString::fromLocal8Bit("绘制直线"));
		this->ui.action_draw_rectangle->setText(QString::fromLocal8Bit("绘制矩形"));
		this->ui.action_draw_rotate_rectangle->setText(QString::fromLocal8Bit("绘制旋转矩形"));

		QFont font = this->ui.action_draw_point->font();
		font.setBold(false);
		this->ui.action_draw_point->setFont(font);
		this->ui.action_draw_line->setFont(font);
		this->ui.action_draw_rectangle->setFont(font);
		this->ui.action_draw_rotate_rectangle->setFont(font);

		this->ui.action_draw_point->setEnabled(true);
		this->ui.action_draw_line->setEnabled(true);
		this->ui.action_draw_rectangle->setEnabled(true);
		this->ui.action_draw_rotate_rectangle->setEnabled(true);
	}
}

Kv::VectorType KvImage::checkVectorSave()
{
	if (this->mVecWin == Q_NULLPTR)
	{
		return Kv::VectorType::None;
	}

	// 如果有矢量，是否保存结果
	if (!this->mVecWin->isVecEmpty())
	{
		int isSave = QMessageBox::warning(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("是否保存当前结果？"),
			QMessageBox::StandardButton::Cancel, QMessageBox::StandardButton::Ok);

		if (isSave == QMessageBox::StandardButton::Ok)
		{
			// TODO: Save results
			return this->mVecWin->vectorType();
		}
		else
		{
			// 想个好的逻辑：处理当前图像的矢量需要继承到下一个图像的情况。
			// 目前就是根据矢量窗口判断是否保存，不考虑KvImage窗口类中的矢量数量。
			// 需要再说吧。
			switch (this->mVecWin->vectorType())
			{
			case Kv::VectorType::Point:
				this->mPts.clear();
				break;
			case Kv::VectorType::Line:
				this->mLines.clear();
				break;
			case Kv::VectorType::Rectangle:
				this->mRects.clear();
				break;
			case Kv::VectorType::RotateRectangle:
				this->mRotRects.clear();
				break;
			case Kv::VectorType::None:
			default:
				break;
			}
			this->mVecWin->clearTableContents();
		}
	}

	return Kv::VectorType::None;
}

void KvImage::addPoint(const cv::Point2d& pt)
{
	this->mPts.append(pt);
	this->mVecWin->appendPoint(pt);
}

void KvImage::addLine(const cv::Vec4d& line)
{
	this->mLines.append(line);
	this->mVecWin->appendLine(line);
}

void KvImage::addRectangle(const cv::Rect2d& rect)
{
	this->mRects.append(rect);
	this->mVecWin->appendRectangle(rect);
}

void KvImage::addRotateRectangle(const RotRect2D& rotRect)
{
	this->mRotRects.append(rotRect);
	this->mVecWin->appendRotateRectangle(rotRect);
}


bool KvImage::deletePoint(const int idx)
{
	if (idx >= this->mPts.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mPts.erase(this->mPts.begin() + idx);
	return true;
}

bool KvImage::deletePoint(const cv::Point2d& pt)
{
	QList<cv::Point2d>::iterator pos = std::find(this->mPts.begin(), this->mPts.end(), pt);
	if (pos == this->mPts.end())
	{
		return false;
	}
	this->mPts.erase(pos);
	return true;
}

bool KvImage::deleteLine(const int idx)
{
	if (idx >= this->mLines.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mLines.erase(this->mLines.begin() + idx);
	return true;
}

bool KvImage::deleteLine(const cv::Vec4d& line)
{
	QList<cv::Vec4d>::iterator pos = std::find(this->mLines.begin(), this->mLines.end(), line);
	if (pos == this->mLines.end())
	{
		return false;
	}
	this->mLines.erase(pos);
	return true;
}

bool KvImage::deleteRectangle(const int idx)
{
	if (idx >= this->mRects.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mRects.erase(this->mRects.begin() + idx);
	return true;
}

bool KvImage::deleteRectangle(const cv::Rect2d& rect)
{
	QList<cv::Rect2d>::iterator pos = std::find(this->mRects.begin(), this->mRects.end(), rect);
	if (pos == this->mRects.end())
	{
		return false;
	}
	this->mRects.erase(pos);
	return true;
}

bool KvImage::deleteRotateRectangle(const int idx)
{
	if (idx >= this->mRotRects.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mRotRects.erase(this->mRotRects.begin() + idx);
	return true;
}

bool KvImage::deleteRotateRectangle(const RotRect2D& rotRect)
{
	QList<RotRect2D>::iterator pos = std::find(this->mRotRects.begin(), this->mRotRects.end(), rotRect);
	if (pos == this->mRotRects.end())
	{
		return false;
	}
	this->mRotRects.erase(pos);
	return true;
}

void KvImage::exitApp(bool exitDirectly)
{
	if (!exitDirectly)
	{
		int ok = QMessageBox::warning(this,
			QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("是否退出？"),
			QMessageBox::Cancel, QMessageBox::Ok);

		if (ok != QMessageBox::Ok)
		{
			return;
		}
	}

	QApplication::quit();
}
