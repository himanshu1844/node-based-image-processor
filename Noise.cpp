#include "Noise.h"

#include "PixmapData.h"

#include <QtNodes/NodeDelegateModelRegistry>

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

Noise::Noise()
    : _label(new QLabel("Image will appear here"))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = _label->font();
    f.setBold(true);
    f.setItalic(true);

    _label->setFont(f);

    _label->setMinimumSize(200, 200);

    _label->installEventFilter(this);
}

unsigned int Noise::nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType) {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 1;

    default:
        break;
    }

    return result;
}

bool Noise::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::Resize) {
            auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
            if (d) {
                _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
            }
        }
    }

    return false;
}

NodeDataType Noise::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> Noise::outData(PortIndex)
{
    return _nodeData;
}

void Noise::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = nodeData;

    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);

        int w = _label->width();
        int h = _label->height();

        _label->setPixmap(d->pixmap().scaled(w, h, Qt::KeepAspectRatio));
    } else {
        _label->setPixmap(QPixmap());
    }

    Q_EMIT dataUpdated(0);
}
void Noise::setoctave(int value){
    _octave=value;
}
void Noise::setpersistence(int value){
    _persistence=value;
}
void Noise::setscale(int value){
    _scale=value;
}

void Noise::setnoisepattern (const std::string &mode){
  _pattern=mode;
}
void Noise::setnoiseuse(const std::string &mode){
    _noiseuse=mode;
}
