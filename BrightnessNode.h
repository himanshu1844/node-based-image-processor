#pragma once

#include <iostream>
#include <QSlider>
#include <QVBoxLayout>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class BrightnessNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    BrightnessNode();

    ~BrightnessNode() = default;

public:
    QString caption() const override { return QString("Brightness Node"); }

    QString name() const override { return QString("BrightnessNode"); }

public:
    virtual QString modelName() const { return QString("Resulting Image"); }

    unsigned int nPorts(PortType const portType) const override;

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const port) override;

    QWidget *embeddedWidget() override { return _label; }

    bool resizable() const override { return true; }
    void setBrightnessLevel(int value);
    void setConstrastLevel(int value);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QLabel *_label;

    std::shared_ptr<NodeData> _nodeData;
    int _brightnessValue = 0;
    int _contrastvalue=0;
};
