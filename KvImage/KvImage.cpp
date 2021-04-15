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
	connect(this->mSideBar->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &KvImage::onImageSelectChanged);
	connect(this->iViewer, SIGNAL(imageMouseMoveEvent(px)), this, SLOT(onImageMouseMoveEvent(px)));
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
	// ͨ���Ի����ȡ�ļ�·��
	QString caption = QString::fromLocal8Bit("ѡ��һ���ļ���");
	QString dir = "";  // Ϊ��Ĭ�ϼ�����һ�δ򿪵�·��
	QString filter("Image (*.png *.jpg *.jpeg *.tif *.bmp)");
	QString fileName = QFileDialog::getOpenFileName(
		this, caption, dir, filter
	);

	if (fileName.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("KvImage::on_action_open_image_triggered - δѡ��ͼƬ·��");
		return;
	}

	cv::Mat img = cv::imread(fileName.toLocal8Bit().toStdString());
	this->iViewer->openImage(img);
	this->putText(QString::fromLocal8Bit("��ͼ��%1 (width=%2, height=%3)")
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
		QString::fromLocal8Bit("���ļ���"));

	if (dirPath.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("KvImage::on_action_open_directory_triggered() - δѡ���ļ���");
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

	// ��ȡͼƬ·��
	QStandardItem* item = ((QStandardItemModel*)this->mSideBar->model())->itemFromIndex(curIdx);
	QString imgPath = QString("%1/%2")
		.arg(this->mImgDirList[parentIdx.row()].dirPath())
		.arg(item->text());
	
	qDebug() << QString::fromLocal8Bit("KvImage::onImageSelectChanged() - ѡ��ͼ��: %1")
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
	}
	else if (this->iViewer->isDrawingPoint())
	{
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
	}
	else if (this->iViewer->isDrawingLine())
	{
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
	}
	else if (this->iViewer->isDrawingRectangle())
	{
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
	}
	else if (this->iViewer->isDrawingRotateRectangle())
	{
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

void KvImage::putText(QString txt)
{
	if (!this->mInfoBar) return;

	QString logStr = QString("(%1) - %2\n")
		.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd"))  // ʱ��
		.arg(txt);  // ��Ϣ����

	this->mInfoBar->textCursor().insertText(logStr);
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
