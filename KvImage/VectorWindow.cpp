#include "VectorWindow.h"

VectorWindow::VectorWindow(QWidget* parent) : QWidget(parent)
{
	// ��ʼ������
	this->mCurCol = this->mCurRow = 0;

	////////// ��ʼ������
	// ���ô����С��λ��
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect deskRect = desktopWidget->availableGeometry();  // ���������С
	int deskHeight, deskWidth;
	deskHeight = deskRect.height();
	deskWidth = deskRect.width();
	int winHeight, winWidth;
	winHeight = deskHeight * 0.5;
	winWidth = deskWidth * 0.2;
	int marginRight;
	marginRight = 100;
	this->setGeometry(deskWidth - winWidth - marginRight, deskHeight / 2 - winHeight / 2, winWidth, winHeight);

	// �������
	this->setWindowTitle(QString::fromLocal8Bit("ʸ������"));

	// �����ͳ�ʼ������
	this->mLayout = new QGridLayout(this);
	this->setLayout(this->mLayout);

	// ���������ӵ�����
	this->mTable = new QTableWidget(this); 
	this->mTable->setContextMenuPolicy(Qt::CustomContextMenu);
	this->mLayout->addWidget(this->mTable, 0, 0, 1, 5);  // �У��У��п��п�
	this->initTable();

	// �Ƿ��ð�ť
	QRadioButton* winTopBtn = new QRadioButton(this);
	winTopBtn->setText(QString::fromLocal8Bit("���ô���"));
	QObject::connect(winTopBtn, SIGNAL(clicked(bool)), this, SLOT(onToggleTopHint(bool)));

	this->mLayout->addWidget(winTopBtn, 1, 2, 1, 1);  // �У��У��п��п�
	if (!winTopBtn->isChecked()) winTopBtn->click();

	// ���¼�
	QObject::connect(this->mTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onItemDoubleClicked(int, int)));
	QObject::connect(this->mTable, SIGNAL(cellClicked(int, int)), this, SLOT(onCellClicked(int, int)));
	QObject::connect(this->mTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTableRightClicked(QPoint)));
}

void VectorWindow::initTable(QTableWidget* table)
{
	if (table == Q_NULLPTR)
	{
		if (this->mTable != Q_NULLPTR)
		{
			table = this->mTable;
		}
		else
		{
			qWarning() << QString::fromLocal8Bit("����ʼ��ʧ��");
			return;
		}
	}

	//���� ��� �� �б��� �Ķ��뷽ʽ
	table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	
	// ���üӴ�
	QFont headerFont = table->horizontalHeader()->font();
	headerFont.setBold(true);
	table->horizontalHeader()->setFont(headerFont);

	// �Զ��������һ�еĿ��ʹ���ͱ����ұ߽����
	table->horizontalHeader()->setStretchLastSection(true);
	// ���ñ���ߵ����ߴ����
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

	//���ñ���ѡ��ʽ
	table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

	//���ñ༭��ʽ
	table->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
}

void VectorWindow::initShowPoints()
{
	this->clearTable();

	this->mTable->setColumnCount(4);  // id,x,y,����
	QStringList rowHeaderList;
	rowHeaderList << "ID" << "x" << "y" << QString::fromLocal8Bit("����");
	this->mTable->setHorizontalHeaderLabels(rowHeaderList);
	this->mTable->setColumnWidth(3, 150);

	this->mVecType = Kv::VectorType::Point;
}

void VectorWindow::initShowLines()
{
	this->clearTable();

	this->mTable->setColumnCount(6);  // id,x1,y1,x2,y2,����
	QStringList rowHeaderList;
	rowHeaderList << "ID" << "x1" << "y1" << "x2" << "y2" << QString::fromLocal8Bit("����");
	this->mTable->setHorizontalHeaderLabels(rowHeaderList);
	this->mTable->setColumnWidth(5, 150);

	this->mVecType = Kv::VectorType::Line;
}

void VectorWindow::initShowRectangles()
{
	this->clearTable();

	this->mTable->setColumnCount(6);  // id,x,y,width,height,����
	QStringList rowHeaderList;
	rowHeaderList << "ID" << "x" << "y" << "width" << "height" << QString::fromLocal8Bit("����");
	this->mTable->setHorizontalHeaderLabels(rowHeaderList);
	this->mTable->setColumnWidth(5, 150);

	this->mVecType = Kv::VectorType::Rectangle;
}

void VectorWindow::initShowRotateRectangles()
{
	this->clearTable();

	this->mTable->setColumnCount(10);  // id,x1,y1,x2,y2,x3,y3,x4,y4,����
	QStringList rowHeaderList;
	rowHeaderList << "ID" << "x1" << "y1" << "x2" << "y2" << "x3" << "y3" << "x4" << "y4" << QString::fromLocal8Bit("����");
	this->mTable->setHorizontalHeaderLabels(rowHeaderList);
	this->mTable->setColumnWidth(9, 150);

	this->mVecType = Kv::VectorType::RotateRectangle;
}

bool VectorWindow::showPoints(const std::vector<cv::Point2d>& pts)
{
	return this->showPoints(QList<cv::Point2d>::fromVector(QVector<cv::Point2d>::fromStdVector(pts)));
}

bool VectorWindow::showLines(const std::vector<cv::Vec4d>& lines)
{
	return this->showLines(QList<cv::Vec4d>::fromVector(QVector<cv::Vec4d>::fromStdVector(lines)));
}

bool VectorWindow::showRectangles(const std::vector<cv::Rect2d>& rects)
{
	return this->showRectangles(QList<cv::Rect2d>::fromVector(QVector<cv::Rect2d>::fromStdVector(rects)));
}

bool VectorWindow::showRotateRectangles(const std::vector<RotRect2D>& rotRects)
{
	return this->showRotateRectangles(QList<RotRect2D>::fromVector(QVector<RotRect2D>::fromStdVector(rotRects)));
}

bool VectorWindow::showPoints(const QList<cv::Point2d>& pts)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	// ��ʼ����ʾ���
	this->initShowPoints();

	if (pts.size() == 0)
	{
		qWarning() << QString::fromLocal8Bit("������Ϊ0");
		return false;
	}

	foreach(cv::Point2d pt, pts)
	{
		this->appendPoint(pt);
	}

	return true;
}

bool VectorWindow::showLines(const QList<cv::Vec4d>& lines)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	// ��ʼ����ʾ���
	this->initShowLines();

	if (lines.size() == 0)
	{
		qWarning() << QString::fromLocal8Bit("ֱ������Ϊ0");
		return false;
	}

	foreach(cv::Vec4d line, lines)
	{
		this->appendLine(line);
	}

	return true;
}

bool VectorWindow::showRectangles(const QList<cv::Rect2d>& rects)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	// ��ʼ����ʾ���
	this->initShowRectangles();

	if (rects.size() == 0)
	{
		qWarning() << QString::fromLocal8Bit("��������Ϊ0");
		return false;
	}

	foreach(cv::Rect2d rect, rects)
	{
		this->appendRectangle(rect);
	}

	return true;
}

bool VectorWindow::showRotateRectangles(const QList<RotRect2D>& rotRects)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	// ��ʼ����ʾ���
	this->initShowRotateRectangles();

	if (rotRects.size() == 0)
	{
		qWarning() << QString::fromLocal8Bit("��ת��������Ϊ0");
		return false;
	}

	foreach(RotRect2D rotRect, rotRects)
	{
		this->appendRotateRectangle(rotRect);
	}

	return true;
}

bool VectorWindow::appendPoint(const cv::Point2d& pt)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	int rowCount, colCount;
	rowCount = this->mTable->rowCount();
	colCount = this->mTable->columnCount();

	if (colCount != 4)
	{
		qWarning() << QString::fromLocal8Bit("������һ�£���ǰΪ(%1)��ӦΪ(%2)")
			.arg(colCount).arg(4);
		return false;
	}

	this->mTable->setRowCount(rowCount + 1);

	QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(rowCount));
	QTableWidgetItem* xItem = new QTableWidgetItem(QString::number(pt.x));
	QTableWidgetItem* yItem = new QTableWidgetItem(QString::number(pt.y));
	QWidget* btnWidget = this->createRowCtrlBtnWidget();

	idItem->setTextAlignment(Qt::AlignCenter);
	xItem->setTextAlignment(Qt::AlignCenter);
	yItem->setTextAlignment(Qt::AlignCenter);

	this->mTable->setItem(rowCount, 0, idItem);
	this->mTable->setItem(rowCount, 1, xItem);
	this->mTable->setItem(rowCount, 2, yItem);
	this->mTable->setCellWidget(rowCount, 3, btnWidget);

	// �������ײ�
	this->mTable->scrollToBottom();

	return true;
}

bool VectorWindow::appendLine(const cv::Vec4d& line)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	int rowCount, colCount;
	rowCount = this->mTable->rowCount();
	colCount = this->mTable->columnCount();
	QWidget* btnWidget = this->createRowCtrlBtnWidget();

	if (colCount != 6)
	{
		qWarning() << QString::fromLocal8Bit("������һ�£���ǰΪ(%1)��ӦΪ(%2)")
			.arg(colCount).arg(6);
		return false;
	}

	this->mTable->setRowCount(rowCount + 1);

	QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(rowCount));
	QTableWidgetItem* x1Item = new QTableWidgetItem(QString::number(line[0]));
	QTableWidgetItem* y1Item = new QTableWidgetItem(QString::number(line[1]));
	QTableWidgetItem* x2Item = new QTableWidgetItem(QString::number(line[2]));
	QTableWidgetItem* y2Item = new QTableWidgetItem(QString::number(line[3]));

	idItem->setTextAlignment(Qt::AlignCenter);
	x1Item->setTextAlignment(Qt::AlignCenter);
	y1Item->setTextAlignment(Qt::AlignCenter);
	x2Item->setTextAlignment(Qt::AlignCenter);
	y2Item->setTextAlignment(Qt::AlignCenter);

	this->mTable->setItem(rowCount, 0, idItem);
	this->mTable->setItem(rowCount, 1, x1Item);
	this->mTable->setItem(rowCount, 2, y1Item);
	this->mTable->setItem(rowCount, 3, x2Item);
	this->mTable->setItem(rowCount, 4, y2Item);
	this->mTable->setCellWidget(rowCount, 5, btnWidget);

	// �������ײ�
	this->mTable->scrollToBottom();

	return true;
}

bool VectorWindow::appendRectangle(const cv::Rect2d& rect)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	int rowCount, colCount;
	rowCount = this->mTable->rowCount();
	colCount = this->mTable->columnCount();

	if (colCount != 6)
	{
		qWarning() << QString::fromLocal8Bit("������һ�£���ǰΪ(%1)��ӦΪ(%2)")
			.arg(colCount).arg(6);
		return false;
	}

	this->mTable->setRowCount(rowCount + 1);

	QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(rowCount));
	QTableWidgetItem* xItem = new QTableWidgetItem(QString::number(rect.x));
	QTableWidgetItem* yItem = new QTableWidgetItem(QString::number(rect.y));
	QTableWidgetItem* wItem = new QTableWidgetItem(QString::number(rect.width));
	QTableWidgetItem* hItem = new QTableWidgetItem(QString::number(rect.height));
	QWidget* btnWidget = this->createRowCtrlBtnWidget();

	idItem->setTextAlignment(Qt::AlignCenter);
	xItem->setTextAlignment(Qt::AlignCenter);
	yItem->setTextAlignment(Qt::AlignCenter);
	wItem->setTextAlignment(Qt::AlignCenter);
	hItem->setTextAlignment(Qt::AlignCenter);

	this->mTable->setItem(rowCount, 0, idItem);
	this->mTable->setItem(rowCount, 1, xItem);
	this->mTable->setItem(rowCount, 2, yItem);
	this->mTable->setItem(rowCount, 3, wItem);
	this->mTable->setItem(rowCount, 4, hItem);
	this->mTable->setCellWidget(rowCount, 5, btnWidget);

	// �������ײ�
	this->mTable->scrollToBottom();

	return true;
}

bool VectorWindow::appendRotateRectangle(const RotRect2D& rotRect)
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return false;
	}

	int rowCount, colCount;
	rowCount = this->mTable->rowCount();
	colCount = this->mTable->columnCount();

	if (colCount != 10)
	{
		qWarning() << QString::fromLocal8Bit("������һ�£���ǰΪ(%1)��ӦΪ(%2)")
			.arg(colCount).arg(10);
		return false;
	}

	this->mTable->setRowCount(rowCount + 1);

	QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(rowCount));
	QTableWidgetItem* x1Item = new QTableWidgetItem(QString::number(rotRect.pt1().x));
	QTableWidgetItem* y1Item = new QTableWidgetItem(QString::number(rotRect.pt1().y));
	QTableWidgetItem* x2Item = new QTableWidgetItem(QString::number(rotRect.pt2().x));
	QTableWidgetItem* y2Item = new QTableWidgetItem(QString::number(rotRect.pt2().y));
	QTableWidgetItem* x3Item = new QTableWidgetItem(QString::number(rotRect.pt3().x));
	QTableWidgetItem* y3Item = new QTableWidgetItem(QString::number(rotRect.pt3().y));
	QTableWidgetItem* x4Item = new QTableWidgetItem(QString::number(rotRect.pt4().x));
	QTableWidgetItem* y4Item = new QTableWidgetItem(QString::number(rotRect.pt4().y));
	QWidget* btnWidget = this->createRowCtrlBtnWidget();

	idItem->setTextAlignment(Qt::AlignCenter);
	x1Item->setTextAlignment(Qt::AlignCenter);
	y1Item->setTextAlignment(Qt::AlignCenter);
	x2Item->setTextAlignment(Qt::AlignCenter);
	y2Item->setTextAlignment(Qt::AlignCenter);
	x3Item->setTextAlignment(Qt::AlignCenter);
	y3Item->setTextAlignment(Qt::AlignCenter);
	x4Item->setTextAlignment(Qt::AlignCenter);
	y4Item->setTextAlignment(Qt::AlignCenter);

	this->mTable->setItem(rowCount, 0, idItem);
	this->mTable->setItem(rowCount, 1, x1Item);
	this->mTable->setItem(rowCount, 2, y1Item);
	this->mTable->setItem(rowCount, 3, x2Item);
	this->mTable->setItem(rowCount, 4, y2Item);
	this->mTable->setItem(rowCount, 5, x3Item);
	this->mTable->setItem(rowCount, 6, y3Item);
	this->mTable->setItem(rowCount, 7, x4Item);
	this->mTable->setItem(rowCount, 8, y4Item);
	this->mTable->setCellWidget(rowCount, 9, btnWidget);

	// �������ײ�
	this->mTable->scrollToBottom();

	return true;
}

QWidget* VectorWindow::createRowCtrlBtnWidget()
{
	QWidget* w = new QWidget();

	QPushButton* viewBtn = new QPushButton(QString::fromLocal8Bit("�鿴"));
	QPushButton* delBtn = new QPushButton(QString::fromLocal8Bit("ɾ��"));
	QHBoxLayout* layout = new QHBoxLayout(); //���ˮƽ���ֿؼ�
	QWidget* widget = new QWidget(); //��Ӳ���

	viewBtn->setStyleSheet("QPushButton{margin:0;padding:0;color:blue;border:none;background-color:rgba(0,0,0,0);}\
		QPushButton:hover{font-weight:bold;text-decoration:underline;}");
	viewBtn->setCursor(Qt::PointingHandCursor);

	delBtn->setStyleSheet("QPushButton{margin:0;padding:0;color:blue;border:none;background-color:rgba(0,0,0,0);}\
		QPushButton:hover{font-weight:bold;text-decoration:underline;}");
	delBtn->setCursor(Qt::PointingHandCursor);

	QObject::connect(viewBtn, SIGNAL(clicked()), this, SLOT(onViewRow()));
	QObject::connect(delBtn, SIGNAL(clicked()), this, SLOT(onRemoveRow()));

	layout->addWidget(viewBtn);
	layout->addWidget(delBtn);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignCenter);
	w->setLayout(layout);
	
	return w;
}

void VectorWindow::clearTable()
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return;
	}

	// ��ձ��
	this->mTable->clearContents();
	this->mTable->setRowCount(0);
	this->mTable->setColumnCount(0);

	this->mVecType = Kv::VectorType::None;
}

void VectorWindow::clearTableContents()
{
	if (this->mTable == Q_NULLPTR)
	{
		qWarning() << QString::fromLocal8Bit("���δ��ʼ��");
		return;
	}

	// ��ձ������
	this->mTable->setRowCount(0);
	this->mTable->clearContents();
}

int VectorWindow::vectorCount() const
{
	if (this->mTable == Q_NULLPTR)
	{
		return 0;
	}
	return this->mTable->rowCount();
}

bool VectorWindow::isVecEmpty() const
{
	Qt::AlignCenter;
	return this->vectorCount() == 0;
}

void VectorWindow::selectOneRow(int row, Kv::VectorType vecType, bool isShowRowInfo)
{
	switch (vecType)
	{
	case Kv::VectorType::Point: {
		if (isShowRowInfo)
		{
			QString itemId = this->mTable->item(row, 0)->text();
			double itemX = this->mTable->item(row, 1)->text().toDouble();
			double itemY = this->mTable->item(row, 2)->text().toDouble();
			qDebug() << QString("Point %1: x=%2, y=%3").arg(itemId).arg(itemX).arg(itemY);
		}
		emit selectPoint(row);
		break;
	}
	case Kv::VectorType::Line: {
		if (isShowRowInfo)
		{
			QString itemId = this->mTable->item(row, 0)->text();
			double itemX1 = this->mTable->item(row, 1)->text().toDouble();
			double itemY1 = this->mTable->item(row, 2)->text().toDouble();
			double itemX2 = this->mTable->item(row, 3)->text().toDouble();
			double itemY2 = this->mTable->item(row, 4)->text().toDouble();
			qDebug() << QString("Line %1: x1=%2, y1=%3, x2=%4, y2=%5").arg(itemId)
				.arg(itemX1).arg(itemY1).arg(itemX2).arg(itemY2);
		}
		emit selectLine(row);
		break;
	}
	case Kv::VectorType::Rectangle: {
		if (isShowRowInfo)
		{
			QString itemId = this->mTable->item(row, 0)->text();
			double itemX = this->mTable->item(row, 1)->text().toDouble();
			double itemY = this->mTable->item(row, 2)->text().toDouble();
			double itemW = this->mTable->item(row, 3)->text().toDouble();
			double itemH = this->mTable->item(row, 4)->text().toDouble();
			qDebug() << QString("Rectangle %1: x=%2, y=%3, width=%4, height=%5").arg(itemId)
				.arg(itemX).arg(itemY).arg(itemW).arg(itemH);
		}
		emit selectRectangle(row);
		break;
	}
	case Kv::VectorType::RotateRectangle: {
		if (isShowRowInfo)
		{
			QString itemId = this->mTable->item(row, 0)->text();
			double itemX1 = this->mTable->item(row, 1)->text().toDouble();
			double itemY1 = this->mTable->item(row, 2)->text().toDouble();
			double itemX2 = this->mTable->item(row, 3)->text().toDouble();
			double itemY2 = this->mTable->item(row, 4)->text().toDouble();
			double itemX3 = this->mTable->item(row, 5)->text().toDouble();
			double itemY3 = this->mTable->item(row, 6)->text().toDouble();
			double itemX4 = this->mTable->item(row, 7)->text().toDouble();
			double itemY4 = this->mTable->item(row, 8)->text().toDouble();
			qDebug() << QString("RotateRectangle %1: x1=%2, y1=%3, x2=%4, y2=%5, x3=%6, y3=%7, x4=%8, y4=%9").arg(itemId)
				.arg(itemX1).arg(itemY1)
				.arg(itemX2).arg(itemY2)
				.arg(itemX3).arg(itemY3)
				.arg(itemX4).arg(itemY4);
		}
		emit selectRotateRectangle(row);
		break;
	}
	case Kv::VectorType::None:
	default:
		break;
	}
}

void VectorWindow::onToggleTopHint(bool checked)
{
	if (checked)
	{
		// ������ǰ
		if (!isHidden())
		{
			this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
			this->show();
		} else this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

		qDebug() << QString::fromLocal8Bit("���ô�����ǰ");
	}
	else
	{  // ȡ��������ǰ
		if (!isHidden())
		{
			this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
			this->show();
		}
		else this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);

		qDebug() << QString::fromLocal8Bit("ȡ��������ǰ");
	}
}

void VectorWindow::onCellClicked(int row, int col)
{
	this->mCurRow = row;
	this->mCurCol = col;
	qDebug() << QString::fromLocal8Bit("��ǰ����: row=%1, col=%2").arg(this->mCurRow).arg(this->mCurCol);
}

void VectorWindow::onItemDoubleClicked(int row, int col)
{
	this->selectOneRow(row, this->mVecType, true);
}

void VectorWindow::onRemoveRow()
{
	int row = -1;

	if (!sender()->property("row").isNull())
	{
		row = sender()->property("row").toInt();
	}
	else
	{
		QWidget* w = qobject_cast<QWidget*>(sender()->parent());
		if (!w)
		{
			qWarning() << "parent of sender() is null";
			return;
		}
		qDebug() << w->pos();
		row = this->mTable->indexAt(w->pos()).row();
	}

	if (row == -1)
	{
		qWarning() << "row == -1";
		return;
	}

	qDebug() << row;

	this->mTable->setCurrentCell(row, this->mTable->columnCount() - 1);
	this->selectOneRow(row, this->mVecType);

	if (QMessageBox::question(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�Ƿ�ɾ����"),
		QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel) == QMessageBox::StandardButton::Ok)
	{
		this->mTable->removeRow(row);
		switch (this->mVecType)
		{
		case Kv::VectorType::Point: {
			emit deletePoint(row);
			break;
		}
		case Kv::VectorType::Line: {
			emit deleteLine(row);
			break;
		}
		case Kv::VectorType::Rectangle: {
			emit deleteRectangle(row);
			break;
		}
		case Kv::VectorType::RotateRectangle: {
			emit deleteRotateRectangle(row);
			break;
		}
		case Kv::VectorType::None:
		default:
			break;
		}
	}
}

void VectorWindow::onViewRow()
{
	int row = -1;

	if (!sender()->property("row").isNull())
	{
		row = sender()->property("row").toInt();
	}
	else
	{
		QWidget* w = qobject_cast<QWidget*>(sender()->parent());
		if (!w)
		{
			qWarning() << "parent of sender() is null";
			return;
		}
		qDebug() << w->pos();
		row = this->mTable->indexAt(w->pos()).row();
	}

	if (row == -1)
	{
		qWarning() << "row == -1";
		return;
	}

	qDebug() << row;
	
	this->mTable->setCurrentCell(row, this->mTable->columnCount() - 1);
	this->selectOneRow(row, this->mVecType);
}

void VectorWindow::onTableRightClicked(const QPoint pos)
{
	qDebug() << pos;

	if (this->mVecType == Kv::VectorType::None)
	{
		return;
	}

	QModelIndex idx = this->mTable->indexAt(pos);
	qDebug() << idx;

	if (idx.row() < 0 || idx.column() < 0)
	{
		return;
	}

	QMenu* menu = new QMenu(this->mTable);
	QAction* actView = new QAction(QString::fromLocal8Bit("�鿴"), this->mTable);
	QAction* actDel = new QAction(QString::fromLocal8Bit("ɾ��"), this->mTable);
	actView->setProperty("row", QVariant(idx.row()));
	actDel->setProperty("row", QVariant(idx.row()));
	QObject::connect(actView, SIGNAL(triggered()), this, SLOT(onViewRow()));
	QObject::connect(actDel, SIGNAL(triggered()), this, SLOT(onRemoveRow()));

	menu->addAction(actView);
	menu->addAction(actDel);
	menu->move(cursor().pos());
	menu->show();
}

void VectorWindow::showEvent(QShowEvent* evt)
{
	emit vectorWindowShow(true);
}

void VectorWindow::closeEvent(QCloseEvent* evt)
{
	emit vectorWindowShow(false);
}
