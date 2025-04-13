#include "Blend.h"
#include "PixmapData.h"
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

Blend::Blend()
    : _label(new QLabel("Image will appear here")),
    _blendmode("normal"),
    _opacity(100),
    _mix(100),
    _image1(nullptr),
    _image2(nullptr)
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);
    _label->setFont(f);
    _label->setMinimumSize(200, 200);
    _label->installEventFilter(this);
}

unsigned int Blend::nPorts(PortType portType) const
{
    unsigned int result = 1;
    switch (portType) {
    case PortType::In:
        result = 2;
        break;
    case PortType::Out:
        result = 1;
    default:
        break;
    }
    return result;
}

bool Blend::eventFilter(QObject *object, QEvent *event)
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

NodeDataType Blend::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> Blend::outData(PortIndex)
{
    return _nodeData;
}

void Blend::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const portIndex)
{
    // Store the input images separately based on port index
    if (portIndex == 0) {
        _image1 = nodeData;
    } else if (portIndex == 1) {
        _image2 = nodeData;
    }

    // Process the images if both are available
    if (_image1 && _image2) {
        auto img1 = std::dynamic_pointer_cast<PixmapData>(_image1);
        auto img2 = std::dynamic_pointer_cast<PixmapData>(_image2);

        if (img1 && img2) {
            // Check for empty pixmaps
            if (!img1->pixmap().isNull() && !img2->pixmap().isNull()) {
                // Perform the blend operation
                QPixmap result = blendImages(img1->pixmap(), img2->pixmap());
                _nodeData = std::make_shared<PixmapData>(result);
            } else if (!img1->pixmap().isNull()) {
                _nodeData = _image1;
            } else if (!img2->pixmap().isNull()) {
                _nodeData = _image2;
            } else {
                _nodeData = nullptr;
            }
        }
    } else if (_image1) {
        // If only the first image is available, pass it through
        _nodeData = _image1;
    } else if (_image2) {
        // If only the second image is available, pass it through with opacity applied
        auto img2 = std::dynamic_pointer_cast<PixmapData>(_image2);
        if (img2 && !img2->pixmap().isNull()) {
            try {
                QPixmap pix = img2->pixmap();
                QImage image = pix.toImage();

                // Convert QImage to cv::Mat
                cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.constBits()), image.bytesPerLine());
                cv::Mat result;

                // Check if the mat is valid
                if (!mat.empty() && mat.channels() == 4) {
                    // Apply opacity
                    cv::Mat channels[4];
                    cv::split(mat, channels);

                    // Adjust alpha channel for opacity
                    channels[3] = channels[3] * (_opacity / 100.0);

                    cv::merge(channels, 4, result);

                    // Convert back to QImage
                    QImage resultImage(result.data, result.cols, result.rows, result.step, QImage::Format_ARGB32);
                    resultImage = resultImage.copy(); // Deep copy to avoid OpenCV buffer issues

                    _nodeData = std::make_shared<PixmapData>(QPixmap::fromImage(resultImage));
                } else {
                    _nodeData = _image2;
                }
            } catch (const cv::Exception& e) {

                qWarning("OpenCV error in setInData: %s", e.what());
                _nodeData = _image2;
            }
        } else {
            _nodeData = _image2;
        }
    } else {
        _nodeData = nullptr;
    }

    // Update the label with the result
    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
        if (d) {
            int w = _label->width();
            int h = _label->height();
            _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
        } else {
            _label->setPixmap(QPixmap());
            _label->setText("Invalid image data");
        }
    } else {
        _label->setPixmap(QPixmap());
        _label->setText("Image will appear here");
    }

    Q_EMIT dataUpdated(0);
}


cv::Mat Blend::qPixmapToCvMat(const QPixmap& pixmap)
{
    // Check if pixmap is empty
    if (pixmap.isNull() || pixmap.width() == 0 || pixmap.height() == 0) {
        return cv::Mat(); // Return empty matrix to indicate invalid input
    }

    QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.constBits()), image.bytesPerLine());

    // Create a deep copy to ensure we're not using Qt's buffer
    cv::Mat result;
    mat.copyTo(result);


    return result;
}


QPixmap Blend::cvMatToQPixmap(const cv::Mat& mat)
{
    // Check if mat is empty
    if (mat.empty()) {
        return QPixmap(); // Return empty pixmap to indicate invalid input
    }

    // Create a copy to ensure memory safety
    QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
    image = image.copy(); // Deep copy to avoid OpenCV buffer issues
    return QPixmap::fromImage(image);
}
QPixmap Blend::blendImages(const QPixmap& img1, const QPixmap& img2)
{
    // Check for empty inputs
    if (img1.isNull() || img2.isNull()) {
        return img1.isNull() ? img2 : img1; // Return the non-null image or empty pixmap
    }

    // Convert QPixmap to cv::Mat
    cv::Mat src1 = qPixmapToCvMat(img1);
    cv::Mat src2 = qPixmapToCvMat(img2);

    if (src1.empty() || src2.empty()) {
        return img1; // Return the first image if conversion failed
    }

    // Resize images to match dimensions
    if (src1.size() != src2.size()) {
        // Only resize if src2 is valid
        if (!src2.empty() && !src1.empty()) {
            cv::resize(src2, src2, src1.size(), 0, 0, cv::INTER_LINEAR);
        }
    }

    // Apply the opacity to the second image
    cv::Mat src2WithOpacity;
    if (_opacity < 100) {
        // Ensure we have 4 channels
        if (src2.channels() == 4) {
            cv::Mat channels[4];
            cv::split(src2, channels);

            // Scale the alpha channel
            channels[3] = channels[3] * (_opacity / 100.0);

            cv::merge(channels, 4, src2WithOpacity);
        } else {
            src2WithOpacity = src2; // Just use original if not 4 channels
        }
    } else {
        src2WithOpacity = src2;
    }

    // Initialize result matrix
    cv::Mat result = src1.clone();


    try {
        if (_blendmode == "Normal") {
            blendNormal(src1, src2WithOpacity, result);
        }
        else if (_blendmode == "Multiply") {
            blendMultiply(src1, src2WithOpacity, result);
        }
        else if (_blendmode == "Screen") {
            blendScreen(src1, src2WithOpacity, result);
        }
        else if (_blendmode == "Overlay") {
            blendOverlay(src1, src2WithOpacity, result);
        }
        else if (_blendmode == "Difference") {
            blendDifference(src1, src2WithOpacity, result);
        }

        // Apply mix level if not 100%
        if (_mix < 100 && !result.empty() && !src1.empty()) {
            cv::Mat blended = result.clone();
            double mixRatio = _mix / 100.0;
            cv::addWeighted(src1, 1.0 - mixRatio, blended, mixRatio, 0.0, result);
        }
    }
    catch (const cv::Exception& e) {
        // Handle OpenCV exceptions
        qWarning("OpenCV error: %s", e.what());
        // Return original image on error
        return img1;
    }

    // Convert back to QPixmap
    return cvMatToQPixmap(result);
}


void Blend::blendNormal(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst)
{
    // Verify inputs
    if (src1.empty() || src2.empty() || src1.channels() != 4 || src2.channels() != 4) {
        if (!src1.empty()) {
            src1.copyTo(dst);
        }
        return;
    }

    // Split the images into channels (R, G, B, A)
    cv::Mat src1Channels[4];
    cv::Mat src2Channels[4];
    cv::Mat dstChannels[4];
    cv::split(src1, src1Channels);
    cv::split(src2, src2Channels);

    // For each pixel, compute: result = src1 * (1 - src2Alpha) + src2 * src2Alpha
    for (int i = 0; i < 3; i++) { // Process RGB channels
        cv::Mat alpha;
        cv::divide(src2Channels[3], cv::Scalar(255), alpha); // Normalize alpha to 0-1

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(src2Channels[i], alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Keep the alpha channel of the bottom layer
    dstChannels[3] = src1Channels[3].clone();

    // Merge the channels back
    cv::merge(dstChannels, 4, dst);
}


void Blend::blendMultiply(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst)
{
    // Verify inputs
    if (src1.empty() || src2.empty() || src1.channels() != 4 || src2.channels() != 4) {
        if (!src1.empty()) {
            src1.copyTo(dst);
        }
        return;
    }


    // Split the images into channels
    cv::Mat src1Channels[4];
    cv::Mat src2Channels[4];
    cv::Mat dstChannels[4];
    cv::split(src1, src1Channels);
    cv::split(src2, src2Channels);

    // For each RGB channel, multiply the values and normalize
    for (int i = 0; i < 3; i++) {
        cv::Mat multiplied;
        cv::multiply(src1Channels[i], src2Channels[i], multiplied, 1.0/255.0);

        // Apply opacity via the alpha channel
        cv::Mat alpha;
        cv::divide(src2Channels[3], cv::Scalar(255), alpha); // Normalize alpha to 0-1

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(multiplied, alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Keep the alpha channel of the bottom layer
    dstChannels[3] = src1Channels[3].clone();

    // Merge the channels back
    cv::merge(dstChannels, 4, dst);
}


void Blend::blendScreen(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst)
{
    // Verify inputs
    if (src1.empty() || src2.empty() || src1.channels() != 4 || src2.channels() != 4) {
        if (!src1.empty()) {
            src1.copyTo(dst);
        }
        return;
    }


    // Split the images into channels
    cv::Mat src1Channels[4];
    cv::Mat src2Channels[4];
    cv::Mat dstChannels[4];
    cv::split(src1, src1Channels);
    cv::split(src2, src2Channels);

    // For each RGB channel, compute: result = 1 - (1 - src1) * (1 - src2)
    for (int i = 0; i < 3; i++) {
        cv::Mat invSrc1, invSrc2, product, screen;
        cv::subtract(cv::Scalar(255), src1Channels[i], invSrc1);
        cv::subtract(cv::Scalar(255), src2Channels[i], invSrc2);
        cv::multiply(invSrc1, invSrc2, product, 1.0/255.0);
        cv::subtract(cv::Scalar(255), product, screen);

        // Apply opacity via the alpha channel
        cv::Mat alpha;
        cv::divide(src2Channels[3], cv::Scalar(255), alpha); // Normalize alpha to 0-1

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(screen, alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Keep the alpha channel of the bottom layer
    dstChannels[3] = src1Channels[3].clone();

    // Merge the channels back
    cv::merge(dstChannels, 4, dst);
}

void Blend::blendOverlay(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst)
{
    // Verify inputs
    if (src1.empty() || src2.empty() || src1.channels() != 4 || src2.channels() != 4) {
        if (!src1.empty()) {
            src1.copyTo(dst);
        }
        return;
    }


    // Split the images into channels
    cv::Mat src1Channels[4];
    cv::Mat src2Channels[4];
    cv::Mat dstChannels[4];
    cv::split(src1, src1Channels);
    cv::split(src2, src2Channels);

    // For each RGB channel
    for (int i = 0; i < 3; i++) {
        cv::Mat overlay = cv::Mat::zeros(src1Channels[i].size(), CV_8UC1);

        // Create mask for dark pixels (< 128)
        cv::Mat darkMask;
        cv::compare(src1Channels[i], 128, darkMask, cv::CMP_LT);

        // Create mask for light pixels (>= 128)
        cv::Mat lightMask;
        cv::compare(src1Channels[i], 128, lightMask, cv::CMP_GE);

        // For dark areas (base < 128): result = 2 * base * blend / 255
        cv::Mat darkResult;
        cv::multiply(src1Channels[i], src2Channels[i], darkResult, 2.0/255.0);

        // For light areas (base >= 128): result = 255 - 2 * (255 - base) * (255 - blend) / 255
        cv::Mat invSrc1, invSrc2, product, lightResult;
        cv::subtract(cv::Scalar(255), src1Channels[i], invSrc1);
        cv::subtract(cv::Scalar(255), src2Channels[i], invSrc2);
        cv::multiply(invSrc1, invSrc2, product, 2.0/255.0);
        cv::subtract(cv::Scalar(255), product, lightResult);

        // Combine results based on masks
        cv::Mat blendedChannel;
        darkResult.copyTo(overlay, darkMask);
        lightResult.copyTo(overlay, lightMask);

        // Apply opacity via the alpha channel
        cv::Mat alpha;
        cv::divide(src2Channels[3], cv::Scalar(255), alpha); // Normalize alpha to 0-1

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(overlay, alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Keep the alpha channel of the bottom layer
    dstChannels[3] = src1Channels[3].clone();

    // Merge the channels back
    cv::merge(dstChannels, 4, dst);
}

void Blend::blendDifference(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst)
{
    // Verify inputs
    if (src1.empty() || src2.empty() || src1.channels() != 4 || src2.channels() != 4) {
        if (!src1.empty()) {
            src1.copyTo(dst);
        }
        return;
    }


    // Split the images into channels
    cv::Mat src1Channels[4];
    cv::Mat src2Channels[4];
    cv::Mat dstChannels[4];
    cv::split(src1, src1Channels);
    cv::split(src2, src2Channels);

    // For each RGB channel, compute absolute difference
    for (int i = 0; i < 3; i++) {
        cv::Mat difference;
        cv::absdiff(src1Channels[i], src2Channels[i], difference);

        // Apply opacity via the alpha channel
        cv::Mat alpha;
        cv::divide(src2Channels[3], cv::Scalar(255), alpha); // Normalize alpha to 0-1

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(difference, alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Keep the alpha channel of the bottom layer
    dstChannels[3] = src1Channels[3].clone();

    // Merge the channels back
    cv::merge(dstChannels, 4, dst);
}


void Blend::setblendMode(const std::string &mode)
{
    _blendmode = mode;

    // Re-process the images if both are available
    if (_image1 && _image2) {
        setInData(_image1, 0);
        setInData(_image2, 1);
    }
}

void Blend::setopacityLevel(int value)
{
    _opacity = value;

    // Re-process the images if both are available
    if (_image1 && _image2) {
        setInData(_image1, 0);
        setInData(_image2, 1);
    }
}

void Blend::setmixLevel(int value)
{
    _mix = value;

    // Re-process the images if both are available
    if (_image1 && _image2) {
        setInData(_image1, 0);
        setInData(_image2, 1);
    }
}
