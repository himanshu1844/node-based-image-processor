#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class PixmapData;

class BrightnessNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    BrightnessNode();
    ~BrightnessNode() override = default;

public:
    QString caption() const override { return QString("Brightness/Contrast"); }
    QString name() const override { return QString("BrightnessNode"); }

public:
    QWidget* embeddedWidget() override { return _label; }

    bool captionVisible() const override { return true; }

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    bool eventFilter(QObject* object, QEvent* event) override;

public slots:
    // Contrast range: 0.0 - 3.0 (1.0 is normal)
    void setContrastLevel(double value);

    // Brightness range: -100 to 100 (0 is normal)
    void setBrightnessLevel(int value);

private:
    void processImage();
    void updateDisplay();

private:
    QLabel* _label;
    int _brightnessValue;     // Range: -100 to 100
    double _contrastValue;    // Range: 0.0 to 3.0

    std::shared_ptr<NodeData> _originalData;   // Store the original image data
    std::shared_ptr<NodeData> _processedData;  // Store the processed image data
};
