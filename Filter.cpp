#include "Filter.h"
#include "PixmapData.h"
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <opencv2/opencv.hpp>
#include <QImage>

FilterNode::FilterNode()
    : _label(new QLabel("Image will appear here")),
    _originalImage(nullptr),
    _filteredImage(nullptr)
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);
    _label->setFont(f);
    _label->setMinimumSize(200, 200);
    _label->installEventFilter(this);

    // Initialize with identity matrix 3x3
    _matrix = QVector<QVector<double>>{
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}
    };
}

FilterNode::~FilterNode()
{
    // Clean up allocated resources
    _originalImage.reset();
    _filteredImage.reset();
}

unsigned int FilterNode::nPorts(PortType portType) const
{
    unsigned int result = 1;
    switch (portType) {
    case PortType::In:
        result = 1;
        break;
    case PortType::Out:
        result = 1;
    default:
        break;
    }
    return result;
}

bool FilterNode::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();
        if (event->type() == QEvent::Resize) {
            auto d = std::dynamic_pointer_cast<PixmapData>(_filteredImage);
            if (d) {
                _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
            }
        }
    }
    return false;
}

NodeDataType FilterNode::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> FilterNode::outData(PortIndex)
{
    return _filteredImage;
}

void FilterNode::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _originalImage = nodeData;

    if (_originalImage) {
        // Apply filter and generate filtered image
        applyFilter();

        // Update display
        auto d = std::dynamic_pointer_cast<PixmapData>(_filteredImage);
        if (d) {
            int w = _label->width();
            int h = _label->height();
            _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
        }
    } else {
        _filteredImage.reset();
        _label->setPixmap(QPixmap());
    }

    Q_EMIT dataUpdated(0);
}

void FilterNode::setmatrix(const QVector<QVector<double>>& matrix)
{
    _matrix = matrix;

    // If we already have an image, reapply the filter with the new matrix
    if (_originalImage) {
        applyFilter();

        // Update display
        auto d = std::dynamic_pointer_cast<PixmapData>(_filteredImage);
        if (d) {
            int w = _label->width();
            int h = _label->height();
            _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
        }

        Q_EMIT dataUpdated(0);
    }
}

void FilterNode::applyFilter()
{
    auto pixmapData = std::dynamic_pointer_cast<PixmapData>(_originalImage);
    if (!pixmapData) return;

    QPixmap originalPixmap = pixmapData->pixmap();
    QImage originalImage = originalPixmap.toImage();

    if (originalImage.isNull() || originalImage.width() == 0 || originalImage.height() == 0) {
        return;
    }

    // Convert QImage to cv::Mat
    cv::Mat originalMat;
    if (originalImage.format() == QImage::Format_RGB32 ||
        originalImage.format() == QImage::Format_ARGB32 ||
        originalImage.format() == QImage::Format_ARGB32_Premultiplied) {
        originalMat = cv::Mat(originalImage.height(), originalImage.width(), CV_8UC4,
                              const_cast<uchar*>(originalImage.bits()), originalImage.bytesPerLine());
        cv::cvtColor(originalMat, originalMat, cv::COLOR_BGRA2BGR);
    } else {
        originalImage = originalImage.convertToFormat(QImage::Format_RGB888);
        originalMat = cv::Mat(originalImage.height(), originalImage.width(), CV_8UC3,
                              const_cast<uchar*>(originalImage.bits()), originalImage.bytesPerLine());
        cv::cvtColor(originalMat, originalMat, cv::COLOR_RGB2BGR);
    }

    // Create kernel from matrix
    int rows = _matrix.size();
    int cols = rows > 0 ? _matrix[0].size() : 0;

    cv::Mat kernel(rows, cols, CV_64F);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            kernel.at<double>(i, j) = _matrix[i][j];
        }
    }

    // Apply filter
    cv::Mat filteredMat;
    cv::filter2D(originalMat, filteredMat, -1, kernel);

    // Convert back to QImage
    cv::Mat rgbMat;
    cv::cvtColor(filteredMat, rgbMat, cv::COLOR_BGR2RGB);

    QImage filteredImage(rgbMat.data, rgbMat.cols, rgbMat.rows,
                         rgbMat.step, QImage::Format_RGB888);
    filteredImage = filteredImage.copy(); // deep copy to avoid issues

    // Create new PixmapData with filtered image
    _filteredImage = std::make_shared<PixmapData>(QPixmap::fromImage(filteredImage));

    // Explicitly release OpenCV resources
    originalMat.release();
    filteredMat.release();
    rgbMat.release();
}
