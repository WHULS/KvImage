#include "KvImage.h"

KvImage::KvImage(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// ���ô����С
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect deskRect = desktopWidget->availableGeometry();  // ���������С
	int deskHeight, deskWidth;
	deskHeight = deskRect.height();
	deskWidth = deskRect.width();
	int winHeight, winWidth;
	winHeight = deskHeight * 0.8;
	winWidth = deskWidth * 0.8;
	this->setGeometry(deskWidth / 2 - winWidth / 2, deskHeight / 2 - winHeight / 2, winWidth, winHeight);

	// ��ʼ������
	this->infoBarVisible = this->sideBarVisible = true;
	this->sideBarWidth = winWidth * 0.2;
	this->infoBarHeight = winHeight * 0.2;
	this->sideBarWidth = this->sideBarWidth > 500 ? 500 : this->sideBarWidth;
	this->infoBarHeight = this->infoBarHeight > 300 ? 300 : this->infoBarHeight;

	// ��ӿؼ�
	this->iViewer = new ImageViewer(ui.centralWidget);    // ͼƬ��ʾ����
	this->mSideBar = new QTreeView(ui.centralWidget);     // �����
	this->mInfoBar = new QTextBrowser(ui.centralWidget);  // ��Ϣ��

	this->refreshLayout();

	this->mInfoBar->setOpenExternalLinks(true);

	this->putText("Start App");

	// ��ʼ�������
	this->initSidebar();
	// ���źźͲ�
	QObject::connect(this->mSideBar->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &KvImage::onImageSelectChanged);
	QObject::connect(this->iViewer, SIGNAL(imageMouseMoveEvent(px)), this, SLOT(onImageMouseMoveEvent(px)));
	QObject::connect(this->iViewer, SIGNAL(drawNewPoint(const cv::Point2d&)), this, SLOT(onDrawNewPoint(const cv::Point2d&)));
	QObject::connect(this->iViewer, SIGNAL(drawNewLine(const cv::Vec4d &)), this, SLOT(onDrawNewLine(const cv::Vec4d&)));
	QObject::connect(this->iViewer, SIGNAL(drawNewRectangle(const cv::Rect2d &)), this, SLOT(onDrawNewRectangle(const cv::Rect2d &)));
	QObject::connect(this->iViewer, SIGNAL(drawNewRotateRectangle(const RotRect2D &)), this, SLOT(onDrawNewRotateRectangle(const RotRect2D &)));

	// ����ʸ������
	this->mVecWin = new VectorWindow();
	this->ui.action_vector_win->setCheckable(true);
	// �󶨴�����ʾ���¼�����˵���ͬ����
	QObject::connect(this->mVecWin, SIGNAL(vectorWindowShow(bool)), this, SLOT(onVectorWindowShow(bool)));
	// ��ʸ��ѡ���¼�	
	QObject::connect(this->mVecWin, SIGNAL(selectPoint(const int)), this, SLOT(onVecWinSelectPoint(const int)));
	QObject::connect(this->mVecWin, SIGNAL(selectLine(const int)), this, SLOT(onVecWinSelectLine(const int)));
	QObject::connect(this->mVecWin, SIGNAL(selectRectangle(const int)), this, SLOT(onVecWinSelectRectangle(const int)));
	QObject::connect(this->mVecWin, SIGNAL(selectRotateRectangle(const int)), this, SLOT(onVecWinSelectRotateRectangle(const int)));
	// ��ʸ��ɾ���¼�	
	QObject::connect(this->mVecWin, SIGNAL(deletePoint(const int)), this, SLOT(onVecWinDeletePoint(const int)));
	QObject::connect(this->mVecWin, SIGNAL(deleteLine(const int)), this, SLOT(onVecWinDeleteLine(const int)));
	QObject::connect(this->mVecWin, SIGNAL(deleteRectangle(const int)), this, SLOT(onVecWinDeleteRectangle(const int)));
	QObject::connect(this->mVecWin, SIGNAL(deleteRotateRectangle(const int)), this, SLOT(onVecWinDeleteRotateRectangle(const int)));
}

void KvImage::refreshLayout()
{
	qDebug() << "KvImage::refreshLayout()";

	// ��ȡ���Ŀؼ��Ŀ��
	QSize cSize = ui.centralWidget->size();
	int cw, ch;
	cw = cSize.width();
	ch = cSize.height();

	// ��������ؼ��ľ���
	QRect imgRect,
		sideRect,
		infoRect;

	// ���ò����
	sideRect.setX(0); sideRect.setY(0);
	sideRect.setWidth(this->sideBarVisible ? this->sideBarWidth : 0);
	sideRect.setHeight(ch);

	// ���õײ���Ϣ��
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

	// ����ͼƬ����
	imgRect.setX(sideRect.width());
	imgRect.setY(0);
	imgRect.setWidth(cw - sideRect.width());
	imgRect.setHeight(ch - infoRect.height());

	// Ӧ�þ���λ��
	this->mSideBar->setGeometry(sideRect);
	this->mInfoBar->setGeometry(infoRect);
	this->iViewer->setGeometry(imgRect);
}


void KvImage::on_action_open_image_triggered()
{
	Kv::VectorType vType = this->checkVectorSave();

	// ͨ���Ի����ȡ�ļ�·��
	QString caption = QString::fromLocal8Bit("ѡ��һ���ļ���");
	QString dir = "";  // Ϊ��Ĭ�ϼ�����һ�δ򿪵�·��
	QString filter("Image (*.png *.jpg *.jpeg *.tif *.bmp)");
	QString fileName = QFileDialog::getOpenFileName(
		this, caption, dir, filter
	);

	if (fileName.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("δѡ��ͼƬ·��");
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
	this->putText(QString::fromLocal8Bit("��ͼ��%1 (width=%2, height=%3)")
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

	// ����ԭ���¼����Զ��������
	evt->ignore();
}

void KvImage::on_action_open_directory_triggered()
{
	QString dirPath = QFileDialog::getExistingDirectory(this,
		QString::fromLocal8Bit("���ļ���"));

	if (dirPath.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("δѡ���ļ���");
		return;
	}

	for (int i = 0; i < this->mImgDirList.size(); i++)
	{
		if (dirPath == this->mImgDirList[i].dirPath())
		{
			qWarning() << QString::fromLocal8Bit("%1 �Ѵ�").arg(dirPath);
			return;
		}
	}

	this->mImgDirList.append(ImageDir(dirPath));

	this->refreshSidebar(this->mImgDirList);
	this->putText(QString::fromLocal8Bit("���ļ��� - %1").arg(dirPath));
}

void KvImage::onImageSelectChanged(const QModelIndex& curIdx, const QModelIndex& preIdx)
{
	// ���û�и��ڵ㣬˵���Ǹ��ڵ�
	QModelIndex parentIdx = curIdx.parent();
	if (!parentIdx.isValid())
	{
		return;
	}

	Kv::VectorType vType = this->checkVectorSave();

	// ��ȡͼƬ·��
	QStandardItem* item = ((QStandardItemModel*)this->mSideBar->model())->itemFromIndex(curIdx);
	QString imgPath = QString("%1/%2")
		.arg(this->mImgDirList[parentIdx.row()].dirPath())
		.arg(item->text());
	
	qDebug() << QString::fromLocal8Bit("ѡ��ͼ��: %1")
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
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("���ȴ�һ��ͼ��"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawPoint();

		// ������������Ϊ����ѡ��
		foreach (QAction *action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;
			
			if (action == this->ui.action_draw_point) continue;

			action->setEnabled(false);
		}

		// �޸�����
		this->ui.action_draw_point->setText(QString::fromLocal8Bit("ֹͣ����"));

		// ���üӴ���ʽ
		QFont font = this->ui.action_draw_point->font();
		font.setBold(true);
		this->ui.action_draw_point->setFont(font);

		// ��ʼ��ʸ������
		this->mVecWin->initShowPoints();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingPoint())
	{
		// ���ʸ��
		this->checkVectorSave();

		this->iViewer->stopDrawPoint();

		// �ָ������Ŀ�ѡ��״̬
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_point) continue;

			action->setEnabled(true);
		}

		// �ָ���������
		this->ui.action_draw_point->setText(QString::fromLocal8Bit("����"));

		// �ָ�������ʽ
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
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("���ȴ�һ��ͼ��"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawLine();

		// ������������Ϊ����ѡ��
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_line) continue;

			action->setEnabled(false);
		}

		// �޸�����
		this->ui.action_draw_line->setText(QString::fromLocal8Bit("ֹͣ����ֱ��"));

		// ���üӴ���ʽ
		QFont font = this->ui.action_draw_line->font();
		font.setBold(true);
		this->ui.action_draw_line->setFont(font);

		// ��ʼ��ʸ������
		this->mVecWin->initShowLines();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingLine())
	{
		// ���ʸ��
		this->checkVectorSave();

		this->iViewer->stopDrawLine();

		// �ָ������Ŀ�ѡ��״̬
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_line) continue;

			action->setEnabled(true);
		}

		// �ָ���������
		this->ui.action_draw_line->setText(QString::fromLocal8Bit("����ֱ��"));

		// �ָ�������ʽ
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
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("���ȴ�һ��ͼ��"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawRectangle();

		// ������������Ϊ����ѡ��
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rectangle) continue;

			action->setEnabled(false);
		}

		// �޸�����
		this->ui.action_draw_rectangle->setText(QString::fromLocal8Bit("ֹͣ���ƾ���"));

		// ���üӴ���ʽ
		QFont font = this->ui.action_draw_rectangle->font();
		font.setBold(true);
		this->ui.action_draw_rectangle->setFont(font);

		// ��ʼ��ʸ������
		this->mVecWin->initShowRectangles();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingRectangle())
	{
		// ���ʸ��
		this->checkVectorSave();

		this->iViewer->stopDrawRectangle();

		// �ָ������Ŀ�ѡ��״̬
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rectangle) continue;

			action->setEnabled(true);
		}

		// �ָ���������
		this->ui.action_draw_rectangle->setText(QString::fromLocal8Bit("���ƾ���"));

		// �ָ�������ʽ
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
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("���ȴ�һ��ͼ��"),
			QMessageBox::Button::Ok);

		return;
	}
	if (!this->iViewer->isDrawing())
	{
		this->iViewer->startDrawRotateRectangle();

		// ������������Ϊ����ѡ��
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rotate_rectangle) continue;

			action->setEnabled(false);
		}

		// �޸�����
		this->ui.action_draw_rotate_rectangle->setText(QString::fromLocal8Bit("ֹͣ������ת����"));

		// ���üӴ���ʽ
		QFont font = this->ui.action_draw_rotate_rectangle->font();
		font.setBold(true);
		this->ui.action_draw_rotate_rectangle->setFont(font);

		// ��ʼ��ʸ������
		this->mVecWin->initShowRotateRectangles();
		this->mVecWin->show();
	}
	else if (this->iViewer->isDrawingRotateRectangle())
	{
		// ���ʸ��
		this->checkVectorSave();

		this->iViewer->stopDrawRotateRectangle();

		// �ָ������Ŀ�ѡ��״̬
		foreach(QAction * action, this->ui.menu_2->actions())
		{
			if (action->isSeparator()) continue;

			if (action == this->ui.action_draw_rotate_rectangle) continue;

			action->setEnabled(true);
		}

		// �ָ���������
		this->ui.action_draw_rotate_rectangle->setText(QString::fromLocal8Bit("������ת����"));

		// �ָ�������ʽ
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
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"))  // ʱ��
		.arg(txt);  // ��Ϣ����

	this->mInfoBar->textCursor().insertText(logStr);
	this->mInfoBar->moveCursor(QTextCursor::End);  // �������ײ�
}

void KvImage::initSidebar()
{
	// ����� ��Ԫ����������
	this->mSideBar->setEditTriggers(QTreeView::NoEditTriggers);   //��Ԫ���ܱ༭
	this->mSideBar->setSelectionBehavior(QTreeView::SelectRows);  //һ��ѡ������
	this->mSideBar->setSelectionMode(QTreeView::SingleSelection); //��ѡ�������������о���һ��ѡ����
	this->mSideBar->setAlternatingRowColors(true);                //ÿ���һ����ɫ��һ��������qssʱ��������Ч
	this->mSideBar->setFocusPolicy(Qt::NoFocus);                  //ȥ������Ƶ���Ԫ����ʱ�����߿�
	this->mSideBar->header()->hide();

	// ��ʼ�������ģ��
	this->mSideBarModel = new QStandardItemModel(this->mSideBar);
	
	// Ӧ��ģ��
	this->mSideBar->setModel(this->mSideBarModel);
}

void KvImage::refreshSidebar(QList<ImageDir>& imgDirList)
{
	if (!this->mSideBarModel)
	{
		this->initSidebar();
	}

	// �������
	this->mSideBarModel->clear();

	// �����������
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

	// չ��������ӵ�һ��
	QModelIndex rootIndex = this->mSideBar->rootIndex();
	QModelIndex lastRowIndex = this->mSideBarModel->index(i - 1, 0, rootIndex);
	this->mSideBar->expand(lastRowIndex);
}

void KvImage::initDrawStatus()
{
	if (this->iViewer->isDrawing())
	{
		this->iViewer->stopDraw();
		
		this->ui.action_draw_point->setText(QString::fromLocal8Bit("����"));
		this->ui.action_draw_line->setText(QString::fromLocal8Bit("����ֱ��"));
		this->ui.action_draw_rectangle->setText(QString::fromLocal8Bit("���ƾ���"));
		this->ui.action_draw_rotate_rectangle->setText(QString::fromLocal8Bit("������ת����"));

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

	// �����ʸ�����Ƿ񱣴���
	if (!this->mVecWin->isVecEmpty())
	{
		int isSave = QMessageBox::warning(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("�Ƿ񱣴浱ǰ�����"),
			QMessageBox::StandardButton::Cancel, QMessageBox::StandardButton::Ok);

		if (isSave == QMessageBox::StandardButton::Ok)
		{
			// TODO: Save results
			return this->mVecWin->vectorType();
		}
		else
		{
			// ����õ��߼�������ǰͼ���ʸ����Ҫ�̳е���һ��ͼ��������
			// Ŀǰ���Ǹ���ʸ�������ж��Ƿ񱣴棬������KvImage�������е�ʸ��������
			// ��Ҫ��˵�ɡ�
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
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("�Ƿ��˳���"),
			QMessageBox::Cancel, QMessageBox::Ok);

		if (ok != QMessageBox::Ok)
		{
			return;
		}
	}

	QApplication::quit();
}
