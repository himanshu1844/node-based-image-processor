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
class Noise: public NodeDelegateModel
{
    Q_OBJECT

public:
    Noise();

    ~Noise() = default;

public:
    QString caption() const override { return QString("Noise "); }

    QString name() const override { return QString("Noise"); }

public:
    virtual QString modelName() const { return QString("Resulting Image"); }

    unsigned int nPorts(PortType const portType) const override;

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const port) override;

    QWidget *embeddedWidget() override { return _label; }

    bool resizable() const override { return true; }
    void setscale(int value);
    void setoctave(int value);
    void setpersistence(int value);
     void setnoisepattern (const std::string &mode);
     void setnoiseuse(const std::string &mode);



protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    int _scale;
    int _octave;
    int _persistence;
    std::string _pattern;
    std::string _noiseuse;
    QLabel *_label;

    std::shared_ptr<NodeData> _nodeData;
};
