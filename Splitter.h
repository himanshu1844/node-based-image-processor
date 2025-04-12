#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QCheckBox>
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;


/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class Splitter :  public NodeDelegateModel
{
    Q_OBJECT

public:
    Splitter();

    ~Splitter() = default;

public:
    QString caption() const override { return QString("SPLITTER"); }

    QString name() const override { return QString("Splitter"); }

public:
    virtual QString modelName() const { return QString("Resulting Image"); }

    unsigned int nPorts(PortType const portType) const override;

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const port) override;
    QImage convertToGrayscaleImage(const QImage &input);

    QWidget *embeddedWidget() override;

    bool resizable() const override { return true; }

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:

    // std::shared_ptr<NodeData> _nodeData;

    // std::shared_ptr<NodeData>  _redData;
    // std::shared_ptr<NodeData>  _greenData;
    // std::shared_ptr<NodeData>  _blueData;
    // QWidget *_widget;
    // QLabel *_label;
    // QCheckBox *_checkRed;
    // QCheckBox *_checkGreen;
    // QCheckBox *_checkBlue;
    // QImage redImage;
    // QImage greenImage;
    // QImage blueImage;
    QLabel *_label;
    QWidget *_widget;
    QCheckBox *_checkRed;
    QCheckBox *_checkGreen;
    QCheckBox *_checkBlue;

    std::shared_ptr<NodeData> _nodeData;
    std::shared_ptr<NodeData> _redData;
    std::shared_ptr<NodeData> _greenData;
    std::shared_ptr<NodeData> _blueData;

    // Original color channel data storage
    std::shared_ptr<NodeData> _originalRedData;
    std::shared_ptr<NodeData> _originalGreenData;
    std::shared_ptr<NodeData> _originalBlueData;
};
