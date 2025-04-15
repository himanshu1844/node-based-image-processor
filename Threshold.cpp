#include "Threshold.h"

#include "PixmapData.h"

#include <QtNodes/NodeDelegateModelRegistry>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
cv::Mat QPixmapToGrayMat(const QPixmap& pixmap) {
    QImage img = pixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
    return cv::Mat(img.height(), img.width(), CV_8UC1, const_cast<uchar*>(img.bits()), img.bytesPerLine()).clone();
}

ThresholdNode::ThresholdNode()
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

unsigned int ThresholdNode::nPorts(PortType portType) const
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

bool ThresholdNode::eventFilter(QObject *object, QEvent *event)
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

NodeDataType ThresholdNode::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> ThresholdNode::outData(PortIndex)
{
    return _nodeData;
}

void ThresholdNode::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = nodeData;

    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
        if (d) {
            _originalMat = QPixmapToGrayMat(d->pixmap());
            applyThreshold();
        }
    } else {
        _label->setPixmap(QPixmap());
    }

    Q_EMIT dataUpdated(0);
}

void ThresholdNode::applyThreshold()
{
    if (_originalMat.empty()) return;

    cv::Mat result;

    if (_thresholdmethod == "binary") {
        cv::threshold(_originalMat, result, _threshold, 255, cv::THRESH_BINARY);
    }
    else if (_thresholdmethod == "adaptive") {
        cv::adaptiveThreshold(_originalMat, result, 255,
                              cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 2);
    }
    else if (_thresholdmethod == "Otsu") {
        cv::Mat gray;
        if (_originalMat.channels() == 3) {
            cv::cvtColor(_originalMat, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = _originalMat.clone();
        }

        double otsu_thresh_val = cv::threshold(gray, result, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    }

    QImage qimg(result.data, result.cols, result.rows, result.step, QImage::Format_Grayscale8);
    QPixmap pixmap = QPixmap::fromImage(qimg.copy());

    int w = _label->width();
    int h = _label->height();

    _label->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));

    _nodeData = std::make_shared<PixmapData>(pixmap);
    Q_EMIT dataUpdated(0);
}

void ThresholdNode::setthreshold(int value){
    _threshold = value;
    applyThreshold();
}
void ThresholdNode::setthresholdmethod(const std::string &mode){
    _thresholdmethod = mode;

    applyThreshold();
}
