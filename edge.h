#ifndef EDGE_H
#define EDGE_H

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QtNodes/NodeDelegateModel>

#include "PixmapData.h"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace cv {
class Mat;  // Forward declaration for OpenCV Mat
}

class Edge : public NodeDelegateModel
{
    Q_OBJECT

public:
    Edge();
    virtual ~Edge();

public:
    QString caption() const override { return QString("Edge Detection"); }

    QString name() const override { return QString("Edge"); }

public:
    QWidget *embeddedWidget() override { return _label; }

    bool eventFilter(QObject *object, QEvent *event) override;

public:
    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    // Parameter setters
    void setthreshold(int value);
    void setedgemethod(const std::string &mode);
    void setKernelSize(int value);
    void setOverlay(bool checked);

private:
    // Process the edge detection with current parameters
    void processEdgeDetection();

private:
    QLabel * _label;
    std::shared_ptr<NodeData> _nodeData;
    cv::Mat* _originalMat;  // Store original image data

    // Edge detection parameters
    int _threshold;         // Threshold value for edge detection
    int _kernelSize;        // Kernel size for operators
    std::string _edgemethod; // Method for edge detection ("sobel" or "canny")
    bool _overlayenable;    // Whether to overlay edges on original image
};

#endif // EDGE_H
