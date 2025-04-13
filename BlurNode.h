#pragma once

#include <iostream>

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
class BlurNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    BlurNode();

    ~BlurNode() = default;

public:
    QString caption() const override { return QString("BlurNode"); }

    QString name() const override { return QString("BlurNodel"); }

public:
    virtual QString modelName() const { return QString("Resulting Image"); }

    unsigned int nPorts(PortType const portType) const override;

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const port) override;

    QWidget *embeddedWidget() override { return _label; }

    void setRadiusLevel(int value);
    void setBlurMode(const std::string &mode);
    bool resizable() const override { return true; }

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QLabel *_label;
    int _radius;
    std::string _mode;
    void processImage();
    std::shared_ptr<NodeData> _nodeData;
    std::shared_ptr<NodeData> _originalData;
};
