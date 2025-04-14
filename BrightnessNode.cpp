#include "BrightnessNode.h"
#include "PixmapData.h"
#include <QtNodes/NodeDelegateModelRegistry>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

BrightnessNode::BrightnessNode()
    : _label(new QLabel("Adjust brightness and contrast"))
    , _brightnessValue(0)    // Default brightness: 0 (no change)
    , _contrastValue(1.0)    // Default contrast: 1.0 (no change)
    , _originalData(nullptr)
    , _processedData(nullptr)
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);
    _label->setFont(f);
    _label->setMinimumSize(200, 200);
    _label->installEventFilter(this);
}

unsigned int BrightnessNode::nPorts(PortType portType) const
{
    unsigned int result = 1;
    switch (portType) {
    case PortType::In:
        result = 1;
        break;
    case PortType::Out:
        result = 1;
        break;
    default:
        break;
    }
    return result;
}

bool BrightnessNode::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label && event->type() == QEvent::Resize) {
        updateDisplay();
    }
    return false;
}

NodeDataType BrightnessNode::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> BrightnessNode::outData(PortIndex)
{
    return _processedData;
}

void BrightnessNode::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    // Store the original input data
    _originalData = nodeData;

    // Process the image with current brightness and contrast values
    processImage();

    // Update the display
    updateDisplay();

    // Notify that output data has changed
    Q_EMIT dataUpdated(0);
}

void BrightnessNode::setContrastLevel(double value)
{
    // Set the new contrast value
    _contrastValue = value;

    // Reprocess the original image with the new values
    processImage();

    // Update the display
    updateDisplay();

    // Notify that output data has changed
    Q_EMIT dataUpdated(0);
}

void BrightnessNode::setBrightnessLevel(int value)
{
    // Set the new brightness value
    _brightnessValue = value;

    // Reprocess the original image with the new values
    processImage();

    // Update the display
    updateDisplay();

    // Notify that output data has changed
    Q_EMIT dataUpdated(0);
}

void BrightnessNode::processImage()
{
    if (!_originalData)
        return;

    auto inputData = std::dynamic_pointer_cast<PixmapData>(_originalData);
    if (!inputData)
        return;

    // Get the input pixmap
    QPixmap inputPixmap = inputData->pixmap();
    if (inputPixmap.isNull())
        return;

    // Convert QPixmap to QImage and then to cv::Mat only once
    QImage img = inputPixmap.toImage().convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(img.height(), img.width(), CV_8UC3, const_cast<uchar*>(img.bits()), img.bytesPerLine());

    // Clone to avoid modifying original memory
    cv::Mat matCopy = mat.clone();

    // Convert from BGR to RGB (OpenCV default is BGR)
    cv::cvtColor(matCopy, matCopy, cv::COLOR_RGB2BGR);

    // Apply brightness and contrast
    cv::Mat result;
    matCopy.convertTo(result, -1, _contrastValue, _brightnessValue);

    // Convert back to RGB for Qt
    cv::cvtColor(result, result, cv::COLOR_BGR2RGB);

    // Create a new QImage from the processed Mat
    QImage processedImage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);

    // Create a QPixmap from the QImage
    QPixmap processedPixmap = QPixmap::fromImage(processedImage.copy());

    // Update the output data
    _processedData = std::make_shared<PixmapData>(processedPixmap);
}

void BrightnessNode::updateDisplay()
{
    if (_processedData) {
        auto pixmap = std::dynamic_pointer_cast<PixmapData>(_processedData)->pixmap();
        if (!pixmap.isNull()) {
            _label->setPixmap(pixmap.scaled(_label->width(), _label->height(), Qt::KeepAspectRatio));
            _label->setText(QString("Brightness: %1, Contrast: %2").arg(_brightnessValue).arg(_contrastValue));
        }
    } else if (_label) {
        _label->setPixmap(QPixmap());
        _label->setText("No image data");
    }
}
