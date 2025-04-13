#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class FilterNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    FilterNode();

    ~FilterNode() = default;

public:
    QString caption() const override { return QString("Image Display"); }

    QString name() const override { return QString("FilterNode"); }

public:
    virtual QString modelName() const { return QString("Resulting Image"); }

    unsigned int nPorts(PortType const portType) const override;

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const port) override;

    QWidget *embeddedWidget() override { return _label; }

    bool resizable() const override { return true; }
    void setmatrix(QVector<QVector<double>> matrix);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QLabel *_label;
    QVector<QVector<double>> _matrix;
    std::shared_ptr<NodeData> _nodeData;
};
