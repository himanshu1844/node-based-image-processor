#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtWidgets/QLabel>
#include <opencv2/core/core.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class Blend : public NodeDelegateModel
{
    Q_OBJECT

public:
    Blend();
    ~Blend() override = default;

public:
    QString caption() const override { return QString("Blend"); }
    bool captionVisible() const override { return true; }
    QString name() const override { return QString("Blend"); }

public:
    QWidget* embeddedWidget() override { return _label; }
    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    // Blend mode and parameter setters
    void setblendMode(const std::string &mode);
    void setopacityLevel(int value);
    void setmixLevel(int value);

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private:
    QLabel* _label;
    std::shared_ptr<NodeData> _nodeData;
    std::shared_ptr<NodeData> _image1;
    std::shared_ptr<NodeData> _image2;
    std::string _blendmode;
    int _opacity;
    int _mix;

    // OpenCV conversion functions
    cv::Mat qPixmapToCvMat(const QPixmap& pixmap);
    QPixmap cvMatToQPixmap(const cv::Mat& mat);

    // Blend functions
    QPixmap blendImages(const QPixmap& img1, const QPixmap& img2);
    void blendNormal(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
    void blendMultiply(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
    void blendScreen(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
    void blendOverlay(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
    void blendDifference(const cv::Mat& src1, const cv::Mat& src2, cv::Mat& dst);
};
