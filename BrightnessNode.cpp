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

unsigned int BrightnessNode::nPorts(PortType portType) const
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

bool BrightnessNode::eventFilter(QObject *object, QEvent *event)
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

NodeDataType BrightnessNode::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> BrightnessNode::outData(PortIndex)
{
    return _nodeData;
}

void BrightnessNode::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = nodeData;

    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);

        // Convert QPixmap to OpenCV Mat
        QImage img = d->pixmap().toImage().convertToFormat(QImage::Format_RGB888);
        cv::Mat mat(img.height(), img.width(), CV_8UC3, const_cast<uchar*>(img.bits()), img.bytesPerLine());

        // Adjust brightness
        cv::Mat brightMat;
        mat.convertTo(brightMat, -1, 1, _brightnessValue);  // alpha = 1, beta = brightness

        // Convert back to QPixmap
        QImage brightImage(brightMat.data, brightMat.cols, brightMat.rows, brightMat.step, QImage::Format_RGB888);
        QPixmap brightPixmap = QPixmap::fromImage(brightImage.rgbSwapped());

        _label->setPixmap(brightPixmap.scaled(_label->width(), _label->height(), Qt::KeepAspectRatio));


        // Save new data
        _nodeData = std::make_shared<PixmapData>(brightPixmap);
    } else {
        _label->setPixmap(QPixmap());
    }
    // if (_nodeData) {
    //     auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);

    //     int w = _label->width();
    //     int h = _label->height();

    //     _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
    // } else {
    //     _label->setPixmap(QPixmap());
    // }

    Q_EMIT dataUpdated(0);

}

    void BrightnessNode::setConstrastLevel(int value) {
    _contrastvalue = value;
    if (_label)

    {
        _label->setText(QString("Brightness: %1").arg(value));
    }
    setInData(_nodeData, 0);
}

void BrightnessNode::setBrightnessLevel(int value) {
    _brightnessValue = value;
    if (_label)
    {
        _label->setText(QString("Brightness: %1").arg(value));
    }
    setInData(_nodeData, 0);
}

