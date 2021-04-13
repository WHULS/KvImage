#include "Transform.h"


QImage Transform::MatToQImage(const cv::Mat& img)
{
    switch (img.type())
    {
        // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image(img.data,
            img.cols, img.rows,
            static_cast<int>(img.step),
            QImage::Format_ARGB32);

        return image;
    }

    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image(img.data,
            img.cols, img.rows,
            static_cast<int>(img.step),
            QImage::Format_RGB888);

        return image.rgbSwapped();
    }

    // 8-bit, 1 channel
    case CV_8UC1:
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        QImage image(img.data,
            img.cols, img.rows,
            static_cast<int>(img.step),
            QImage::Format_Grayscale8);  //Format_Alpha8 and Format_Grayscale8 were added in Qt 5.5
#else
        static QVector<QRgb>  sColorTable;

        // only create our color table the first time
        if (sColorTable.isEmpty())
        {
            sColorTable.resize(256);

            for (int i = 0; i < 256; ++i)
            {
                sColorTable[i] = qRgb(i, i, i);
            }
        }

        QImage image(img.data,
            img.cols, img.rows,
            static_cast<int>(img.step),
            QImage::Format_Indexed8);

        image.setColorTable(sColorTable);
#endif

        return image;
    }

    default:
        qWarning() << "Transform::MatToQImage() - cv::Mat image type not handled in switch: " << img.type();
        break;
    }

    return QImage();
}

QImage Transform::MatToQImage(const cv::Mat& img, cv::Size imgSize)
{
    cv::Mat src;
    cv::Rect imgRect = Calc2D::calcImageRect(img, imgSize);
    cv::resize(img, src, cv::Size(imgRect.width, imgRect.height));
    
    return Transform::MatToQImage(src);
}

QImage Transform::MatToQImage(const cv::Mat& img, cv::Size viewSize, cv::Rect imgRect)
{
    // 视图区矩形
    cv::Rect viewRect(0, 0, viewSize.width, viewSize.height);

    // 图像显示的矩形
    cv::Rect imgViewRect = viewRect & imgRect;

    // 如果图像矩形位于显示区内
    if (imgViewRect != cv::Rect())
    {
        // 背景图
        cv::Mat viewImg = cv::Mat::zeros(viewSize, img.type());
        
        // 将图片上要显示的区域提取出来
        // 由于图像矩形和原图并不一定等大，因此需要进行区域选取处理
        cv::Rect rawImgRect;
        // 获取相对于图像左上角的坐标
        rawImgRect.x = (imgViewRect.x - imgRect.x) * img.cols / imgRect.width;
        rawImgRect.y = (imgViewRect.y - imgRect.y) * img.rows / imgRect.height;
        // 获取实际宽高
        rawImgRect.width = imgViewRect.width * img.cols / imgRect.width;
        rawImgRect.height = imgViewRect.height * img.rows / imgRect.height;

        cv::Mat src;
        cv::resize(img(rawImgRect), src, cv::Size(imgViewRect.width, imgViewRect.height));
        src.copyTo(viewImg(imgViewRect));

        return Transform::MatToQImage(viewImg);
    }

    // 如果图像矩形不位于显示区内，直接显示黑图
    QImage image(viewSize.width, viewSize.height, QImage::Format_RGB888);
    image.fill(Qt::GlobalColor::black);
    return image;
}

QPixmap Transform::MatToQPixmap(const cv::Mat& img)
{
    return QPixmap::fromImage(Transform::MatToQImage(img));
}

QPixmap Transform::MatToQPixmap(const cv::Mat& img, cv::Size imgSize)
{
    return QPixmap::fromImage(Transform::MatToQImage(img, imgSize));
}

QPixmap Transform::MatToQPixmap(const cv::Mat& img, cv::Size viewSize, cv::Rect imgRect)
{
    return QPixmap::fromImage(Transform::MatToQImage(img, viewSize, imgRect));
}

cv::Mat Transform::QImageToMat(const QImage& qimg, bool cloneImageData)
{
    switch (qimg.format())
    {
        // 8-bit, 4 channel
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        cv::Mat mat(qimg.height(), qimg.width(),
            CV_8UC4,
            const_cast<uchar*>(qimg.bits()),
            static_cast<size_t>(qimg.bytesPerLine())
        );

        return (cloneImageData ? mat.clone() : mat);
    }

    // 8-bit, 3 channel
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {
        if (!cloneImageData)
        {
            qWarning() << "Transform::QImageToMat() - Conversion requires cloning because we use a temporary QImage";
        }

        QImage   swapped = qimg;

        if (qimg.format() == QImage::Format_RGB32)
        {
            swapped = swapped.convertToFormat(QImage::Format_RGB888);
        }

        swapped = swapped.rgbSwapped();

        return cv::Mat(swapped.height(), swapped.width(),
            CV_8UC3,
            const_cast<uchar*>(swapped.bits()),
            static_cast<size_t>(swapped.bytesPerLine())
        ).clone();
    }

    // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat mat(qimg.height(), qimg.width(),
            CV_8UC1,
            const_cast<uchar*>(qimg.bits()),
            static_cast<size_t>(qimg.bytesPerLine())
        );

        return (cloneImageData ? mat.clone() : mat);
    }

    default:
        qWarning() << "Transform::QImageToMat() - QImage format not handled in switch: " << qimg.format();
        break;
    }

    return cv::Mat();
}

cv::Mat Transform::QPixmapToMat(const QPixmap& pxm, bool cloneImageData)
{
    return Transform::QImageToMat(pxm.toImage(), cloneImageData);
}
