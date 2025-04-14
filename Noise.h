// Noise.h
#ifndef NOISE_H
#define NOISE_H

#include <QLabel>
#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include <opencv2/opencv.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class PixmapData;

class Noise : public NodeDelegateModel
{
    Q_OBJECT

public:
    Noise();
    virtual ~Noise() {}

    QString caption() const override { return QString("Noise Generator"); }
    bool captionVisible() const override { return true; }
    QString name() const override { return QString("Noise"); }

    QWidget *embeddedWidget() override { return _label; }

    bool eventFilter(QObject *object, QEvent *event) override;

public:
    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

public slots:
    void setoctave(int value);
    void setpersistence(int value);
    void setscale(int value);
    void setnoisepattern(const std::string &mode);
    void setnoiseuse(const std::string &mode);
    void generateNoise();

private:
    QLabel *_label;
    std::shared_ptr<NodeData> _nodeData;

    // Noise parameters
    int _octave = 4;         // 1-8
    int _persistence = 5;    // 1-9
    int _scale = 100;        // 1-500
    std::string _pattern = "perlin";  // perlin, simplex, worley
    std::string _noiseuse = "Direct"; // Displacement or Direct

    // OpenCV noise generation methods
    cv::Mat generatePerlinNoise(int width, int height);
    cv::Mat generateSimplexNoise(int width, int height);
    cv::Mat generateWorleyNoise(int width, int height);

    // Helper functions
    float perlinNoise2D(float x, float y, int octaves, float persistence);
    float simplexNoise2D(float x, float y, int octaves, float persistence);
    float worleyNoise2D(float x, float y);
    float fade(float t);
    float lerp(float a, float b, float t);
    float grad(int hash, float x, float y);

    // Resource cleanup
};

#endif // NOISE_H
