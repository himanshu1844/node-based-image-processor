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
    _blendmode("Normal"),
    _opacity(50),
    _mix(50),
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
        // If only the second image is available, apply enhanced opacity
        auto img2 = std::dynamic_pointer_cast<PixmapData>(_image2);
        if (img2 && !img2->pixmap().isNull()) {
            try {
                QPixmap pix = img2->pixmap();
                QImage image = pix.toImage();

                // Convert QImage to cv::Mat
                cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.constBits()), image.bytesPerLine());

                // Convert to float for better precision
                cv::Mat matf;
                mat.convertTo(matf, CV_32FC4, 1.0/255.0);

                // Split channels
                std::vector<cv::Mat> channels(4);
                cv::split(matf, channels);

                // Apply enhanced opacity curve to alpha channel for more noticeable changes
                double opacityFactor = pow(_opacity / 100.0, 0.8); // Make changes more dramatic
                channels[3] = channels[3] * opacityFactor;

                // Merge channels back
                cv::Mat resultf;
                cv::merge(channels, resultf);

                // Convert back to 8-bit
                cv::Mat result;
                resultf.convertTo(result, CV_8UC4, 255.0);

                // Convert back to QImage
                QImage resultImage(result.data, result.cols, result.rows, result.step, QImage::Format_ARGB32);
                resultImage = resultImage.copy(); // Deep copy to avoid OpenCV buffer issues

                _nodeData = std::make_shared<PixmapData>(QPixmap::fromImage(resultImage));
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

    // Initialize result matrix
    cv::Mat blendResult;

    try {
        if (_blendmode == "Normal") {
            blendNormal(src1, src2, blendResult);
        }
        else if (_blendmode == "Multiply") {
            blendMultiply(src1, src2, blendResult);
        }
        else if (_blendmode == "Screen") {
            blendScreen(src1, src2, blendResult);
        }
        else if (_blendmode == "Overlay") {
            blendOverlay(src1, src2, blendResult);
        }
        else if (_blendmode == "Difference") {
            blendDifference(src1, src2, blendResult);
        }

        // Apply mix level using enhanced curve for more noticeable effect
        cv::Mat result;
        if (_mix < 100 && !blendResult.empty() && !src1.empty()) {
            // Use a curve to make mix changes more noticeable
            // Square root curve makes small mix values have more impact
            double mixRatio = sqrt(_mix / 100.0);

            // Convert to float for better precision
            cv::Mat src1f, blendResultf, resultf;
            src1.convertTo(src1f, CV_32FC4, 1.0/255.0);
            blendResult.convertTo(blendResultf, CV_32FC4, 1.0/255.0);

            // Mix the original and blended images with enhanced curve
            cv::addWeighted(src1f, 1.0 - mixRatio, blendResultf, mixRatio, 0.0, resultf);

            // Convert back to 8-bit
            resultf.convertTo(result, CV_8UC4, 255.0);
        } else {
            result = blendResult;
        }

        return cvMatToQPixmap(result);
    }
    catch (const cv::Exception& e) {
        // Handle OpenCV exceptions
        qWarning("OpenCV error: %s", e.what());
        // Return original image on error
        return img1;
    }
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

    // Convert images to CV_32F for better precision in calculations
    cv::Mat src1f, src2f;
    src1.convertTo(src1f, CV_32FC4, 1.0/255.0);
    src2.convertTo(src2f, CV_32FC4, 1.0/255.0);

    // Split the images into channels (R, G, B, A)
    std::vector<cv::Mat> src1Channels(4);
    std::vector<cv::Mat> src2Channels(4);
    std::vector<cv::Mat> dstChannels(4);
    cv::split(src1f, src1Channels);
    cv::split(src2f, src2Channels);

    // Apply opacity more aggressively to image B's alpha channel
    // Emphasize the effect by using a non-linear curve for more noticeable changes
    double opacityFactor = pow(_opacity / 100.0, 0.8); // Using power function to make changes more noticeable
    src2Channels[3] = src2Channels[3] * opacityFactor;

    // Process RGB channels with the adjusted opacity
    for (int i = 0; i < 3; i++) {
        // Get the adjusted alpha value
        cv::Mat alpha = src2Channels[3].clone();

        // Calculate inverse alpha
        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        // Blend according to alpha
        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(src2Channels[i], alpha, term2);

        // Add the terms to get the blended result
        cv::Mat resultChannel;
        cv::add(term1, term2, resultChannel);
        dstChannels[i] = resultChannel;
    }

    // Calculate resulting alpha channel
    // For normal blend, the result alpha should be a combination of both alphas
    cv::Mat alpha1 = src1Channels[3];
    cv::Mat alpha2 = src2Channels[3];
    cv::Mat inverseAlpha2;
    cv::subtract(cv::Scalar(1.0), alpha2, inverseAlpha2);

    // Alpha compositing formula: result_alpha = alpha2 + alpha1 * (1 - alpha2)
    cv::Mat resultAlpha;
    cv::multiply(alpha1, inverseAlpha2, resultAlpha);
    cv::add(resultAlpha, alpha2, dstChannels[3]);

    // Merge channels
    cv::Mat dstf;
    cv::merge(dstChannels, dstf);

    // Convert back to 8-bit format
    dstf.convertTo(dst, CV_8UC4, 255.0);
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

    // Convert images to floating point for better precision
    cv::Mat src1f, src2f;
    src1.convertTo(src1f, CV_32FC4, 1.0/255.0);
    src2.convertTo(src2f, CV_32FC4, 1.0/255.0);

    // Split the images into channels
    std::vector<cv::Mat> src1Channels(4);
    std::vector<cv::Mat> src2Channels(4);
    std::vector<cv::Mat> dstChannels(4);
    cv::split(src1f, src1Channels);
    cv::split(src2f, src2Channels);

    // Apply enhanced opacity for more noticeable effect
    double opacityFactor = pow(_opacity / 100.0, 0.8);
    src2Channels[3] = src2Channels[3] * opacityFactor;

    // For each RGB channel, multiply the values
    for (int i = 0; i < 3; i++) {
        cv::Mat multiplied;
        cv::multiply(src1Channels[i], src2Channels[i], multiplied);

        // Apply opacity via the alpha channel
        cv::Mat alpha = src2Channels[3].clone();

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(multiplied, alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Calculate resulting alpha channel
    cv::Mat alpha1 = src1Channels[3];
    cv::Mat alpha2 = src2Channels[3];
    cv::Mat inverseAlpha2;
    cv::subtract(cv::Scalar(1.0), alpha2, inverseAlpha2);

    // Alpha compositing formula: result_alpha = alpha2 + alpha1 * (1 - alpha2)
    cv::Mat resultAlpha;
    cv::multiply(alpha1, inverseAlpha2, resultAlpha);
    cv::add(resultAlpha, alpha2, dstChannels[3]);

    // Merge the channels back
    cv::Mat dstf;
    cv::merge(dstChannels, dstf);

    // Convert back to 8-bit
    dstf.convertTo(dst, CV_8UC4, 255.0);
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

    // Convert images to floating point for better precision
    cv::Mat src1f, src2f;
    src1.convertTo(src1f, CV_32FC4, 1.0/255.0);
    src2.convertTo(src2f, CV_32FC4, 1.0/255.0);

    // Split the images into channels
    std::vector<cv::Mat> src1Channels(4);
    std::vector<cv::Mat> src2Channels(4);
    std::vector<cv::Mat> dstChannels(4);
    cv::split(src1f, src1Channels);
    cv::split(src2f, src2Channels);

    // Apply enhanced opacity for more noticeable effect
    double opacityFactor = pow(_opacity / 100.0, 0.8);
    src2Channels[3] = src2Channels[3] * opacityFactor;

    // For each RGB channel, compute the screen blend: result = 1 - (1 - src1) * (1 - src2)
    for (int i = 0; i < 3; i++) {
        cv::Mat invSrc1, invSrc2, product, screen;
        cv::subtract(cv::Scalar(1.0), src1Channels[i], invSrc1);
        cv::subtract(cv::Scalar(1.0), src2Channels[i], invSrc2);
        cv::multiply(invSrc1, invSrc2, product);
        cv::subtract(cv::Scalar(1.0), product, screen);

        // Apply opacity via the alpha channel
        cv::Mat alpha = src2Channels[3].clone();

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(screen, alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Calculate resulting alpha channel
    cv::Mat alpha1 = src1Channels[3];
    cv::Mat alpha2 = src2Channels[3];
    cv::Mat inverseAlpha2;
    cv::subtract(cv::Scalar(1.0), alpha2, inverseAlpha2);

    // Alpha compositing formula: result_alpha = alpha2 + alpha1 * (1 - alpha2)
    cv::Mat resultAlpha;
    cv::multiply(alpha1, inverseAlpha2, resultAlpha);
    cv::add(resultAlpha, alpha2, dstChannels[3]);

    // Merge the channels back
    cv::Mat dstf;
    cv::merge(dstChannels, dstf);

    // Convert back to 8-bit
    dstf.convertTo(dst, CV_8UC4, 255.0);
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

    // Convert images to floating point for better precision
    cv::Mat src1f, src2f;
    src1.convertTo(src1f, CV_32FC4, 1.0/255.0);
    src2.convertTo(src2f, CV_32FC4, 1.0/255.0);

    // Split the images into channels
    std::vector<cv::Mat> src1Channels(4);
    std::vector<cv::Mat> src2Channels(4);
    std::vector<cv::Mat> dstChannels(4);
    cv::split(src1f, src1Channels);
    cv::split(src2f, src2Channels);

    // Apply enhanced opacity for more noticeable effect
    double opacityFactor = pow(_opacity / 100.0, 0.8);
    src2Channels[3] = src2Channels[3] * opacityFactor;

    // For each RGB channel, apply overlay blend
    for (int i = 0; i < 3; i++) {
        cv::Mat overlay = cv::Mat::zeros(src1Channels[i].size(), CV_32F);
        cv::Mat base = src1Channels[i];
        cv::Mat blend = src2Channels[i];

        // Create masks for dark pixels (< 0.5)
        cv::Mat darkMask;
        cv::compare(base, 0.5, darkMask, cv::CMP_LT);
        darkMask.convertTo(darkMask, CV_32F, 1.0/255.0);

        // Create mask for light pixels (>= 0.5)
        cv::Mat lightMask;
        cv::compare(base, 0.5, lightMask, cv::CMP_GE);
        lightMask.convertTo(lightMask, CV_32F, 1.0/255.0);

        // For dark areas (base < 0.5): result = 2 * base * blend
        cv::Mat darkResult;
        cv::multiply(base, blend, darkResult, 2.0);

        // For light areas (base >= 0.5): result = 1 - 2 * (1 - base) * (1 - blend)
        cv::Mat invBase, invBlend, product, lightResult;
        cv::subtract(cv::Scalar(1.0), base, invBase);
        cv::subtract(cv::Scalar(1.0), blend, invBlend);
        cv::multiply(invBase, invBlend, product, 2.0);
        cv::subtract(cv::Scalar(1.0), product, lightResult);

        // Combine results based on masks
        cv::Mat term1, term2, blendedChannel;
        cv::multiply(darkResult, darkMask, term1);
        cv::multiply(lightResult, lightMask, term2);
        cv::add(term1, term2, blendedChannel);

        // Apply opacity
        cv::Mat alpha = src2Channels[3].clone();
        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat baseTerm, blendTerm;
        cv::multiply(base, inverseAlpha, baseTerm);
        cv::multiply(blendedChannel, alpha, blendTerm);

        cv::add(baseTerm, blendTerm, dstChannels[i]);
    }

    // Calculate resulting alpha channel
    cv::Mat alpha1 = src1Channels[3];
    cv::Mat alpha2 = src2Channels[3];
    cv::Mat inverseAlpha2;
    cv::subtract(cv::Scalar(1.0), alpha2, inverseAlpha2);

    // Alpha compositing formula: result_alpha = alpha2 + alpha1 * (1 - alpha2)
    cv::Mat resultAlpha;
    cv::multiply(alpha1, inverseAlpha2, resultAlpha);
    cv::add(resultAlpha, alpha2, dstChannels[3]);

    // Merge the channels back
    cv::Mat dstf;
    cv::merge(dstChannels, dstf);

    // Convert back to 8-bit
    dstf.convertTo(dst, CV_8UC4, 255.0);
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

    // Convert images to floating point for better precision
    cv::Mat src1f, src2f;
    src1.convertTo(src1f, CV_32FC4, 1.0/255.0);
    src2.convertTo(src2f, CV_32FC4, 1.0/255.0);

    // Split the images into channels
    std::vector<cv::Mat> src1Channels(4);
    std::vector<cv::Mat> src2Channels(4);
    std::vector<cv::Mat> dstChannels(4);
    cv::split(src1f, src1Channels);
    cv::split(src2f, src2Channels);

    // Apply enhanced opacity for more noticeable effect
    double opacityFactor = pow(_opacity / 100.0, 0.8);
    src2Channels[3] = src2Channels[3] * opacityFactor;

    // For each RGB channel, compute absolute difference
    for (int i = 0; i < 3; i++) {
        cv::Mat difference;
        cv::absdiff(src1Channels[i], src2Channels[i], difference);

        // Apply opacity via the alpha channel
        cv::Mat alpha = src2Channels[3].clone();

        cv::Mat inverseAlpha;
        cv::subtract(cv::Scalar(1.0), alpha, inverseAlpha);

        cv::Mat term1, term2;
        cv::multiply(src1Channels[i], inverseAlpha, term1);
        cv::multiply(difference, alpha, term2);

        cv::add(term1, term2, dstChannels[i]);
    }

    // Calculate resulting alpha channel
    cv::Mat alpha1 = src1Channels[3];
    cv::Mat alpha2 = src2Channels[3];
    cv::Mat inverseAlpha2;
    cv::subtract(cv::Scalar(1.0), alpha2, inverseAlpha2);

    // Alpha compositing formula: result_alpha = alpha2 + alpha1 * (1 - alpha2)
    cv::Mat resultAlpha;
    cv::multiply(alpha1, inverseAlpha2, resultAlpha);
    cv::add(resultAlpha, alpha2, dstChannels[3]);

    // Merge the channels back
    cv::Mat dstf;
    cv::merge(dstChannels, dstf);

    // Convert back to 8-bit
    dstf.convertTo(dst, CV_8UC4, 255.0);
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
