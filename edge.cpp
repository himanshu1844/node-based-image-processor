#include "edge.h"
#include "PixmapData.h"
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

Edge::Edge()
    : _label(new QLabel("Image will appear here")),
    _threshold(100),
    _kernelSize(3),
    _edgemethod("canny"),
    _overlayenable(false),
    _originalMat(nullptr)
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);
    _label->setFont(f);
    _label->setMinimumSize(200, 200);
    _label->installEventFilter(this);
}

Edge::~Edge()
{
    if (_originalMat) {
        delete _originalMat;
        _originalMat = nullptr;
    }
}

unsigned int Edge::nPorts(PortType portType) const
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

bool Edge::eventFilter(QObject *object, QEvent *event)
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

NodeDataType Edge::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> Edge::outData(PortIndex)
{
    return _nodeData;
}

void Edge::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = nodeData;
    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
        if (d) {
            // Store the original image
            QImage qImg = d->pixmap().toImage();

            // Convert QImage to cv::Mat
            if (_originalMat) {
                delete _originalMat;
            }
            _originalMat = new cv::Mat(qImg.height(), qImg.width(), CV_8UC4, (void*)qImg.bits(), qImg.bytesPerLine());
            *_originalMat = _originalMat->clone(); // Make a deep copy

            // Apply edge detection
            processEdgeDetection();
        }
    } else {
        _label->setPixmap(QPixmap());
    }
    Q_EMIT dataUpdated(0);
}

void Edge::setthreshold(int value) {
    _threshold = value;
    processEdgeDetection();
}

void Edge::setedgemethod(const std::string &mode) {
    _edgemethod = mode;
    processEdgeDetection();
}

void Edge::setKernelSize(int value) {
    // Ensure kernel size is odd
    _kernelSize = (value % 2 == 0) ? value + 1 : value;
    processEdgeDetection();
}

void Edge::setOverlay(bool checked) {
    _overlayenable = checked;
    processEdgeDetection();
}

void Edge::processEdgeDetection() {
    if (!_originalMat || _originalMat->empty()) {
        return;
    }

    // Create grayscale image for edge detection
    cv::Mat grayMat;
    cv::cvtColor(*_originalMat, grayMat, cv::COLOR_BGRA2GRAY);

    // Apply edge detection
    cv::Mat edgeMat;

    if (_edgemethod == "Sobel") {
        // Sobel edge detection
        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;

        cv::Sobel(grayMat, grad_x, CV_16S, 1, 0, _kernelSize);
        cv::Sobel(grayMat, grad_y, CV_16S, 0, 1, _kernelSize);

        cv::convertScaleAbs(grad_x, abs_grad_x);
        cv::convertScaleAbs(grad_y, abs_grad_y);

        cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edgeMat);

        // Apply threshold
        cv::threshold(edgeMat, edgeMat, _threshold, 255, cv::THRESH_BINARY);
    } else {
        // Canny edge detection
        cv::Canny(grayMat, edgeMat, _threshold, _threshold * 2, _kernelSize);
    }

    // Create output image
    cv::Mat resultMat;

    if (_overlayenable) {
        // Overlay mode: Create a colored result with the edges
        cv::Mat colorEdgeMat;
        cv::cvtColor(edgeMat, colorEdgeMat, cv::COLOR_GRAY2BGRA);

        // Create a copy of the original image
        resultMat = _originalMat->clone();

        // Overlay the edges (white) on top of the original image
        for (int y = 0; y < resultMat.rows; y++) {
            for (int x = 0; x < resultMat.cols; x++) {
                if (edgeMat.at<uchar>(y, x) > 0) {
                    // If there's an edge, set the pixel to a bright color (e.g., green)
                    resultMat.at<cv::Vec4b>(y, x)[0] = 0;     // B
                    resultMat.at<cv::Vec4b>(y, x)[1] = 255;   // G
                    resultMat.at<cv::Vec4b>(y, x)[2] = 0;     // R
                    // Keep alpha channel as is
                }
            }
        }
    } else {
        // Convert to BGRA for Qt
        cv::cvtColor(edgeMat, resultMat, cv::COLOR_GRAY2BGRA);
    }

    // Convert to QPixmap
    QImage qImg(resultMat.data, resultMat.cols, resultMat.rows, resultMat.step, QImage::Format_ARGB32);
    QPixmap pixmap = QPixmap::fromImage(qImg.copy()); // Create a deep copy

    // Update the displayed image
    int w = _label->width();
    int h = _label->height();
    _label->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));

    // Update output data
    auto pixmapData = std::make_shared<PixmapData>(pixmap);
    _nodeData = pixmapData;

    // Notify that data has changed
    Q_EMIT dataUpdated(0);
}
