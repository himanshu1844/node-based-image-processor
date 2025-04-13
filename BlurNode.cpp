#include "BlurNode.h"

#include "PixmapData.h"

#include <QtNodes/NodeDelegateModelRegistry>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

BlurNode::BlurNode()
    : _label(new QLabel("Image will appear here"))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);

    _label->setFont(f);

    _label->setMinimumSize(200, 200);

    _label->installEventFilter(this);
}

unsigned int BlurNode::nPorts(PortType portType) const
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

bool BlurNode::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::Resize) {
            auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
            if (d) {
                _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
            }
        }
    }

    return false;
}

NodeDataType BlurNode::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> BlurNode::outData(PortIndex)
{
    return _nodeData;
}

void BlurNode::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{

    if (nodeData) {
        // Store the original input data
        _originalData = nodeData;

        // Process the image with current settings
        _nodeData = _originalData;
        processImage();
    } else {

        _originalData = nullptr;
        _nodeData = nullptr;
        _label->setPixmap(QPixmap());
    }
    Q_EMIT dataUpdated(0);
    }


void BlurNode::setRadiusLevel(int value) {
    _radius = value;

    // Apply the blur with new radius value if we have image data
    if (_nodeData) {
        // Re-process the input data with the new radius
        processImage();

        // Emit signal that data has changed
        Q_EMIT dataUpdated(0);
    }
}

void BlurNode::setBlurMode(const std::string &mode) {
    _mode = mode;

    // Apply the blur with new mode if we have image data
    if (_nodeData) {
        // Re-process the input data with the new mode
        processImage();

        // Emit signal that data has changed
        Q_EMIT dataUpdated(0);
    }
}

// Helper function to process the image with current settings
void BlurNode::processImage() {
    if (!_originalData) return;

    auto d = std::dynamic_pointer_cast<PixmapData>(_originalData);
    if (d) {
        // Convert QPixmap to OpenCV Mat
        QImage qimg = d->pixmap().toImage();
        cv::Mat inMat(qimg.height(), qimg.width(), CV_8UC4, (void*)qimg.bits(), qimg.bytesPerLine());
        cv::Mat outMat;

        // Apply blur based on mode and radius
        if (_mode == "uniform") {
            cv::GaussianBlur(inMat, outMat, cv::Size(2 * _radius + 1, 2 * _radius + 1), 0);
        }
        else if (_mode == "horizontal") {
            cv::GaussianBlur(inMat, outMat, cv::Size(2 * _radius + 1, 1), 0, 0);
        }
        else if (_mode == "vertical") {
            cv::GaussianBlur(inMat, outMat, cv::Size(1, 2 * _radius + 1), 0, 0);
        }
        else {
            cv::GaussianBlur(inMat, outMat, cv::Size(2 * _radius + 1, 2 * _radius + 1), 0);
        }

        // Convert result back to QPixmap
        QImage result(outMat.data, outMat.cols, outMat.rows, outMat.step, QImage::Format_ARGB32);
        QPixmap resultPixmap = QPixmap::fromImage(result.copy());

        // Create new PixmapData for output
        _nodeData = std::make_shared<PixmapData>(resultPixmap);

        // Display the blurred image
        int w = _label->width();
        int h = _label->height();
        _label->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));
    }
}
