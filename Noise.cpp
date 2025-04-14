#include "Noise.h"
#include "PixmapData.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <cmath>
#include <random>
#include <vector>

Noise::Noise()
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

unsigned int Noise::nPorts(PortType portType) const
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

bool Noise::eventFilter(QObject *object, QEvent *event)
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

NodeDataType Noise::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> Noise::outData(PortIndex)
{
    return _nodeData;
}

void Noise::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = nodeData;
    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
        int w = _label->width();
        int h = _label->height();
        _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
    } else {
        _label->setPixmap(QPixmap());
    }

    // Generate noise when input data changes
    generateNoise();

    Q_EMIT dataUpdated(0);
}

void Noise::setoctave(int value) {
    _octave = std::max(1, std::min(8, value));
    generateNoise();
}

void Noise::setpersistence(int value) {
    _persistence = std::max(1, std::min(9, value));
    generateNoise();
}

void Noise::setscale(int value) {
    _scale = std::max(1, std::min(500, value));
    generateNoise();
}

void Noise::setnoisepattern(const std::string &mode) {
    _pattern = mode;
    generateNoise();
}

void Noise::setnoiseuse(const std::string &mode) {
    _noiseuse = mode;
    generateNoise();
}

void Noise::generateNoise() {
    // Don't process if there's no input data
    auto pixmapData = std::dynamic_pointer_cast<PixmapData>(_nodeData);
    if (!pixmapData) {
        return;
    }

    QPixmap originalPixmap = pixmapData->pixmap();
    if (originalPixmap.isNull()) {
        return;
    }

    // Convert QPixmap to cv::Mat
    QImage image = originalPixmap.toImage().convertToFormat(QImage::Format_RGB888);
    cv::Mat inputMat(image.height(), image.width(), CV_8UC3, image.bits(), image.bytesPerLine());
    cv::Mat result;

    // Generate noise based on selected pattern
    cv::Mat noiseMat;
    if (_pattern == "Perlin") {
        noiseMat = generatePerlinNoise(image.width(), image.height());
    } else if (_pattern == "Simplex") {
        noiseMat = generateSimplexNoise(image.width(), image.height());
    } else if (_pattern == "Worley") {
        noiseMat = generateWorleyNoise(image.width(), image.height());
    } else {
        noiseMat = generatePerlinNoise(image.width(), image.height());
    }

    // Apply noise based on selected use mode
    if (_noiseuse == "Displacement") {
        // Use noise as displacement map
        cv::Mat displaceX, displaceY;
        cv::Mat channels[2];

        // Create displacement maps
        displaceX = noiseMat.clone();
        displaceY = noiseMat.clone();

        // Scale displacement values
        float displacementFactor = _scale / 50.0f;
        displaceX = displaceX * displacementFactor;
        displaceY = displaceY * displacementFactor;

        // Create map for remap function
        cv::Mat mapX(noiseMat.size(), CV_32FC1);
        cv::Mat mapY(noiseMat.size(), CV_32FC1);

        for (int y = 0; y < noiseMat.rows; y++) {
            for (int x = 0; x < noiseMat.cols; x++) {
                float dx = displaceX.at<float>(y, x);
                float dy = displaceY.at<float>(y, x);

                mapX.at<float>(y, x) = x + dx;
                mapY.at<float>(y, x) = y + dy;
            }
        }

        // Apply displacement using remap
        cv::remap(inputMat, result, mapX, mapY, cv::INTER_LINEAR, cv::BORDER_REFLECT);
    } else {
        // Use noise as direct color output
        cv::Mat colorNoise;
        cv::normalize(noiseMat, noiseMat, 0, 255, cv::NORM_MINMAX);
        noiseMat.convertTo(colorNoise, CV_8UC1);

        // Create a colored version of noise
        cv::Mat colorChannels[3] = {colorNoise.clone(), colorNoise.clone(), colorNoise.clone()};
        cv::merge(colorChannels, 3, result);
    }

    // Convert back to QPixmap
    QImage resultImage(result.data, result.cols, result.rows, result.step, QImage::Format_RGB888);
    QPixmap resultPixmap = QPixmap::fromImage(resultImage.copy());

    // Update output data
    _nodeData = std::make_shared<PixmapData>(resultPixmap);

    // Update the display
    int w = _label->width();
    int h = _label->height();
    _label->setPixmap(resultPixmap.scaled(w, h, Qt::KeepAspectRatio));

    Q_EMIT dataUpdated(0);
}

cv::Mat Noise::generatePerlinNoise(int width, int height) {
    cv::Mat noiseMat(height, width, CV_32FC1);

    float normScale = _scale / 100.0f;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = x * normScale / width;
            float ny = y * normScale / height;

            float noiseValue = perlinNoise2D(nx, ny, _octave, _persistence / 10.0f);
            noiseMat.at<float>(y, x) = noiseValue;
        }
    }

    return noiseMat;
}

cv::Mat Noise::generateSimplexNoise(int width, int height) {
    cv::Mat noiseMat(height, width, CV_32FC1);

    float normScale = _scale / 100.0f;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = x * normScale / width;
            float ny = y * normScale / height;

            float noiseValue = simplexNoise2D(nx, ny, _octave, _persistence / 10.0f);
            noiseMat.at<float>(y, x) = noiseValue;
        }
    }

    return noiseMat;
}

cv::Mat Noise::generateWorleyNoise(int width, int height) {
    cv::Mat noiseMat(height, width, CV_32FC1);

    // Generate random feature points for Worley noise
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    int numPoints = 20 + _octave * 5; // Increase points with octaves
    std::vector<cv::Point2f> featurePoints;

    for (int i = 0; i < numPoints; i++) {
        float x = dis(gen);
        float y = dis(gen);
        featurePoints.push_back(cv::Point2f(x, y));
    }

    float normScale = _scale / 100.0f;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = x * normScale / width;
            float ny = y * normScale / height;

            // Calculate distance to nearest feature point
            float minDist = 2.0f; // Initialize with value larger than possible distance

            for (const auto& point : featurePoints) {
                float dx = nx - point.x;
                float dy = ny - point.y;
                float dist = std::sqrt(dx*dx + dy*dy);

                minDist = std::min(minDist, dist);
            }

            // Apply persistence as contrast control
            float contrast = 1.0f + (_persistence / 4.5f);
            float noiseValue = std::pow(1.0f - minDist, contrast);

            noiseMat.at<float>(y, x) = noiseValue;
        }
    }

    return noiseMat;
}

float Noise::perlinNoise2D(float x, float y, int octaves, float persistence) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++) {
        // Sample noise at current frequency
        int ix = static_cast<int>(x * frequency);
        int iy = static_cast<int>(y * frequency);

        float fx = x * frequency - ix;
        float fy = y * frequency - iy;

        float n00 = grad((ix & 0xFF) + ((iy & 0xFF) << 8), fx, fy);
        float n01 = grad((ix & 0xFF) + (((iy+1) & 0xFF) << 8), fx, fy-1);
        float n10 = grad(((ix+1) & 0xFF) + ((iy & 0xFF) << 8), fx-1, fy);
        float n11 = grad(((ix+1) & 0xFF) + (((iy+1) & 0xFF) << 8), fx-1, fy-1);

        // Smooth interpolation
        float u = fade(fx);
        float v = fade(fy);

        float noise = lerp(
            lerp(n00, n10, u),
            lerp(n01, n11, u),
            v
            );

        total += noise * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    // Normalize
    return total / maxValue;
}
float Noise::simplexNoise2D(float x, float y, int octaves, float persistence) {
    // Simplified 2D simplex noise implementation
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++) {
        // Skew input space to determine simplex cell
        const float F2 = 0.366025404f; // 0.5*(sqrt(3)-1)
        const float G2 = 0.211324865f; // (3-sqrt(3))/6

        float s = (x + y) * F2;
        float xs = x + s;
        float ys = y + s;
        int i_grid = static_cast<int>(xs);
        int j_grid = static_cast<int>(ys);

        float t = (i_grid + j_grid) * G2;
        float X0 = i_grid - t;
        float Y0 = j_grid - t;
        float x0 = x - X0;
        float y0 = y - Y0;

        // Determine which simplex we're in
        int i1, j1;
        if (x0 > y0) {
            i1 = 1;
            j1 = 0;
        } else {
            i1 = 0;
            j1 = 1;
        }

        float x1 = x0 - i1 + G2;
        float y1 = y0 - j1 + G2;
        float x2 = x0 - 1.0f + 2.0f * G2;
        float y2 = y0 - 1.0f + 2.0f * G2;

        // Calculate contributions from 3 corners
        float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f;

        // Corner 0
        float t0 = 0.5f - x0*x0 - y0*y0;
        if (t0 > 0) {
            t0 *= t0;
            n0 = t0 * t0 * grad((i_grid & 0xFF) + ((j_grid & 0xFF) << 8), x0, y0);
        }

        // Corner 1
        float t1 = 0.5f - x1*x1 - y1*y1;
        if (t1 > 0) {
            t1 *= t1;
            n1 = t1 * t1 * grad(((i_grid+i1) & 0xFF) + (((j_grid+j1) & 0xFF) << 8), x1, y1);
        }

        // Corner 2
        float t2 = 0.5f - x2*x2 - y2*y2;
        if (t2 > 0) {
            t2 *= t2;
            n2 = t2 * t2 * grad(((i_grid+1) & 0xFF) + (((j_grid+1) & 0xFF) << 8), x2, y2);
        }

        // Add contribution from each corner
        float noise = 40.0f * (n0 + n1 + n2);

        total += noise * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;

        x *= 2.0f;
        y *= 2.0f;
    }

    // Normalize
    return (total / maxValue) * 0.5f + 0.5f;
}


float Noise::worleyNoise2D(float x, float y) {
    // Implementation included in generateWorleyNoise function
    return 0.0f; // Not used directly
}

float Noise::fade(float t) {
    // Quintic interpolation curve
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Noise::lerp(float a, float b, float t) {
    // Linear interpolation
    return a + t * (b - a);
}

float Noise::grad(int hash, float x, float y) {
    // Convert low 4 bits of hash code into 12 gradient directions
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

