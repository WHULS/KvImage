#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QTableWidget>
#include <QStringList>
#include <QHeaderView>
#include <QDesktopWidget>
#include <QApplication>
#include <QRadioButton>
#include <QDebug>
#include <vector>
#include <opencv2/opencv.hpp>
#include "RotRect2D.h"
#include <QList>
#include "KvNamespace.h"
#include <QPushButton>
#include <QMessageBox>
#include <QMenu>

class VectorWindow :
    public QWidget
{
    Q_OBJECT

public:
    VectorWindow(QWidget* parent = Q_NULLPTR);
    void initTable(QTableWidget* table = Q_NULLPTR);
    void initShowPoints();
    void initShowLines();
    void initShowRectangles();
    void initShowRotateRectangles();

    bool showPoints(const std::vector<cv::Point2d>& pts);
    bool showLines(const std::vector<cv::Vec4d>& lines);
    bool showRectangles(const std::vector<cv::Rect2d>& rects);
    bool showRotateRectangles(const std::vector<RotRect2D>& rotRects);

    bool showPoints(const QList<cv::Point2d>& pts);
    bool showLines(const QList<cv::Vec4d>& lines);
    bool showRectangles(const QList<cv::Rect2d>& rects);
    bool showRotateRectangles(const QList<RotRect2D>& rotRects);

    bool appendPoint(const cv::Point2d& pt);
    bool appendLine(const cv::Vec4d& line);
    bool appendRectangle(const cv::Rect2d& rect);
    bool appendRotateRectangle(const RotRect2D& rotRect);

    /// <summary>
    /// ��ȡ�в�����ť�Ŀؼ�
    /// </summary>
    /// <returns></returns>
    QWidget* createRowCtrlBtnWidget();

    // ���������񣨰�����ͷ��
    void clearTable();
    // ��ձ������
    void clearTableContents();

    // ����ʸ���������������)
    int vectorCount() const;
    bool isVecEmpty() const;

    Kv::VectorType vectorType() { return this->mVecType; }

    /// <summary>
    /// ѡ��һ��
    /// </summary>
    /// <param name="row">�к�</param>
    /// <param name="vecType">��ǰʸ������</param>
    /// <param name="isShowRowInfo">�Ƿ��ӡ����Ϣ</param>
    void selectOneRow(int row, Kv::VectorType vecType, bool isShowRowInfo = false);

public slots:
    void onToggleTopHint(bool checked);
    void onCellClicked(int row, int col);
    void onItemDoubleClicked(int row, int col);
    void onRemoveRow();
    void onViewRow();
    void onTableRightClicked(const QPoint pos);

signals:
    void vectorWindowShow(bool isShow);
    void selectPoint(const int idx);
    void selectLine(const int idx);
    void selectRectangle(const int idx);
    void selectRotateRectangle(const int idx);
    void deletePoint(const int idx);
    void deleteLine(const int idx);
    void deleteRectangle(const int idx);
    void deleteRotateRectangle(const int idx);

protected:
    // ��д�رպ���ʾ�¼�
    void showEvent(QShowEvent* evt);
    void closeEvent(QCloseEvent* evt);

private:
    QGridLayout* mLayout = Q_NULLPTR;
    QTableWidget* mTable = Q_NULLPTR;
    
    Kv::VectorType mVecType = Kv::VectorType::None;

    int mCurRow, mCurCol;
};

