#include "BlurNode.h"

#include "PixmapData.h"

#include <QtNodes/NodeDelegateModelRegistry>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <QVBoxLayout>

BlurNode::BlurNode()
    : _label(new QLabel("Image will appear here")),
    _kernelPreviewLabel(new QLabel("Kernel preview"))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _kernelPreviewLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);

    _label->setFont(f);
    _kernelPreviewLabel->setFont(f);

    _label->setMinimumSize(200, 200);
    _kernelPreviewLabel->setFixedSize(100, 100);

    _label->installEventFilter(this);


    QVBoxLayout* layout = new QVBoxLayout(_containerWidget);
    layout->addWidget(_label);
    layout->addWidget(_kernelPreviewLabel);


    _mode = "uniform";


    updateKernelPreview();
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

        // Update kernel preview with current settings
        updateKernelPreview();

        // Process the image with current settings
        _nodeData = _originalData;
        processImage();
    } else {
        _originalData = nullptr;
        _nodeData = nullptr;
        _label->setPixmap(QPixmap());
        // Clear kernel preview when no image is loaded
        _kernelPreviewLabel->setPixmap(QPixmap());
    }
    Q_EMIT dataUpdated(0);
}

void BlurNode::setRadiusLevel(int value) {
    _radius = value;

    // Apply the blur with new radius value if we have image data
    if (_nodeData) {
        // Re-process the input data with the new radius
        processImage();
        updateKernelPreview();

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
        updateKernelPreview();

        // Emit signal that data has changed
        Q_EMIT dataUpdated(0);
    }
}
void BlurNode::updateKernelPreview() {
    // Create kernel based on current mode and radius
    cv::Mat kernel;

    if (_mode == "uniform") {
        // Create Gaussian kernel
        kernel = cv::getGaussianKernel(2 * _radius + 1, 0);
        // Convert 1D kernel to 2D
        kernel = kernel * kernel.t();
    }
    else if (_mode == "horizontal") {
        // Create horizontal Gaussian kernel
        cv::Mat kernelX = cv::getGaussianKernel(2 * _radius + 1, 0);
        kernel = cv::Mat::ones(1, 2 * _radius + 1, CV_64F);
        for (int i = 0; i < 2 * _radius + 1; i++) {
            kernel.at<double>(0, i) = kernelX.at<double>(i, 0);
        }
    }
    else if (_mode == "vertical") {
        // Create vertical Gaussian kernel
        kernel = cv::getGaussianKernel(2 * _radius + 1, 0);
        // Keep as column vector for vertical blur
    }
    else {
        // Default to uniform Gaussian
        kernel = cv::getGaussianKernel(2 * _radius + 1, 0);
        kernel = kernel * kernel.t();
    }

    // Normalize kernel for visualization
    cv::Mat kernelVis;
    cv::normalize(kernel, kernelVis, 0, 255, cv::NORM_MINMAX);
    kernelVis.convertTo(kernelVis, CV_8U);

    // Apply a colormap for better visualization
    cv::Mat kernelColor;
    cv::applyColorMap(kernelVis, kernelColor, cv::COLORMAP_JET);


    int displaySize = 100;
    cv::Mat resizedKernel;
    cv::resize(kernelColor, resizedKernel, cv::Size(displaySize, displaySize), 0, 0, cv::INTER_NEAREST);

    // Convert to QPixmap
    QImage kernelImg;
    if (_mode == "horizontal") {
        kernelImg = QImage(resizedKernel.data, resizedKernel.cols, resizedKernel.rows, resizedKernel.step, QImage::Format_RGB888).rgbSwapped();
    } else if (_mode == "vertical") {
        kernelImg = QImage(resizedKernel.data, resizedKernel.cols, resizedKernel.rows, resizedKernel.step, QImage::Format_RGB888).rgbSwapped();
    } else {
        kernelImg = QImage(resizedKernel.data, resizedKernel.cols, resizedKernel.rows, resizedKernel.step, QImage::Format_RGB888).rgbSwapped();
    }

    QPixmap kernelPixmap = QPixmap::fromImage(kernelImg);
    _kernelPreviewLabel->setPixmap(kernelPixmap);

    // Add text to indicate kernel size
    QString kernelInfo = QString("Kernel: %1x%2").arg(kernel.cols).arg(kernel.rows);
    _kernelPreviewLabel->setToolTip(kernelInfo);
}

// Helper function to process the image with current settings
void BlurNode::processImage() {
    if (!_originalData) return;

    auto d = std::dynamic_pointer_cast<PixmapData>(_originalData);
    if (d) {
        QImage qimg = d->pixmap().toImage();
        if (qimg.isNull()) {
            qDebug() << "QImage is null. Skipping processing.";
            return;
        }

        cv::Mat inMat(qimg.height(), qimg.width(), CV_8UC4, (void*)qimg.bits(), qimg.bytesPerLine());

        if (inMat.empty()) {
            qDebug() << "Input Mat is empty. Skipping GaussianBlur.";
            return;
        }

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

        _nodeData = std::make_shared<PixmapData>(resultPixmap);

        int w = _label->width();
        int h = _label->height();
        _label->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));
    }
}

