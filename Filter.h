#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtNodes/NodeDelegateModel>

#include <memory>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class FilterNode : public NodeDelegateModel
{
    Q_OBJECT

public:
    FilterNode();
    virtual ~FilterNode();

public:
    QString caption() const override { return QString("Filter"); }
    bool captionVisible() const override { return true; }

    QString name() const override { return QString("Filter"); }

public:
    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    QWidget *embeddedWidget() override { return _label; }
    bool resizable() const override { return true; }

    // Sets the filter matrix (3x3 or 5x5)
    void setmatrix(const QVector<QVector<double>>& matrix);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    // Applies the filter matrix to the original image
    void applyFilter();

private:
    QLabel * _label;
    std::shared_ptr<NodeData> _originalImage;
    std::shared_ptr<NodeData> _filteredImage;
    QVector<QVector<double>> _matrix;
};
