#include "Splitter.h"
#include "PixmapData.h"
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>

Splitter::Splitter()
    : _label(nullptr), _widget(new QWidget())
{
    _label = new QLabel("Image will appear here");
    _checkRed = new QCheckBox("Greyscale Red");
    _checkGreen = new QCheckBox("Greyscale Green");
    _checkBlue = new QCheckBox("Greyscale Blue");

    auto layout = new QVBoxLayout(_widget);
    layout->addWidget(_checkRed);
    layout->addWidget(_checkGreen);
    layout->addWidget(_checkBlue);
    // layout->addWidget(_label);

    _widget->setLayout(layout);

    connect(_checkRed, &QCheckBox::toggled, this, [this](bool checked) {
        if (_redData) {
            auto pixmapRedData = std::dynamic_pointer_cast<PixmapData>(_redData);
            if (pixmapRedData) {
                if (checked) {
                    // Apply grayscale
                    _redData = std::make_shared<PixmapData>(QPixmap::fromImage(convertToGrayscaleImage(pixmapRedData->pixmap().toImage())));
                } else {
                    // Restore original data
                    _redData = _originalRedData;
                }
                Q_EMIT dataUpdated(0);
            }
        }
    });

    connect(_checkGreen, &QCheckBox::toggled, this, [this](bool checked) {
        if (_greenData) {
            auto pixmapGreenData = std::dynamic_pointer_cast<PixmapData>(_greenData);
            if (pixmapGreenData) {
                if (checked) {
                    // Apply grayscale
                    _greenData = std::make_shared<PixmapData>(QPixmap::fromImage(convertToGrayscaleImage(pixmapGreenData->pixmap().toImage())));
                } else {
                    // Restore original data
                    _greenData = _originalGreenData;
                }
                Q_EMIT dataUpdated(1);
            }
        }
    });

    connect(_checkBlue, &QCheckBox::toggled, this, [this](bool checked) {
        if (_blueData) {
            auto pixmapBlueData = std::dynamic_pointer_cast<PixmapData>(_blueData);
            if (pixmapBlueData) {
                if (checked) {
                    // Apply grayscale
                    _blueData = std::make_shared<PixmapData>(QPixmap::fromImage(convertToGrayscaleImage(pixmapBlueData->pixmap().toImage())));
                } else {
                    // Restore original data
                    _blueData = _originalBlueData;
                }
                Q_EMIT dataUpdated(2);
            }
        }
    });

    _label->installEventFilter(this);
}

QWidget* Splitter::embeddedWidget()
{
    return _widget;
}

unsigned int Splitter::nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType) {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 3;
        break;  // Added break

    default:
        break;
    }

    return result;
}

bool Splitter::eventFilter(QObject *object, QEvent *event)
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

NodeDataType Splitter::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> Splitter::outData(PortIndex portindex)
{
    switch (portindex) {
    case 0: return _redData;
    case 1: return _greenData;
    case 2: return _blueData;
    default: return nullptr;
    }
}

QImage Splitter::convertToGrayscaleImage(const QImage &input)
{
    QImage grayImage(input.size(), QImage::Format_RGB888);

    for (int y = 0; y < input.height(); ++y) {
        for (int x = 0; x < input.width(); ++x) {
            QColor color(input.pixel(x, y));
            int gray = qGray(color.red(), color.green(), color.blue());
            grayImage.setPixel(x, y, qRgb(gray, gray, gray));
        }
    }

    return grayImage;
}

void Splitter::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = std::dynamic_pointer_cast<PixmapData>(nodeData);

    if (_nodeData) {
        auto pixmapData = std::dynamic_pointer_cast<PixmapData>(_nodeData);
        QImage image = pixmapData->pixmap().toImage().convertToFormat(QImage::Format_RGB888);

        // Update the main image display
        _label->setPixmap(pixmapData->pixmap().scaled(_label->width(), _label->height(), Qt::KeepAspectRatio));

        QImage redImage(image.size(), QImage::Format_RGB888);
        QImage greenImage(image.size(), QImage::Format_RGB888);
        QImage blueImage(image.size(), QImage::Format_RGB888);

        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                QColor color(image.pixel(x, y));
                int r = color.red();
                int g = color.green();
                int b = color.blue();

                redImage.setPixel(x, y, qRgb(r, 0, 0));
                greenImage.setPixel(x, y, qRgb(0, g, 0));
                blueImage.setPixel(x, y, qRgb(0, 0, b));
            }
        }

        // Store original color channel data
        _originalRedData = std::make_shared<PixmapData>(QPixmap::fromImage(redImage));
        _originalGreenData = std::make_shared<PixmapData>(QPixmap::fromImage(greenImage));
        _originalBlueData = std::make_shared<PixmapData>(QPixmap::fromImage(blueImage));

        // Assign to output ports (initially the original data)
        _redData = _originalRedData;
        _greenData = _originalGreenData;
        _blueData = _originalBlueData;

        // Apply grayscale effect if checkboxes are already checked
        if (_checkRed->isChecked()) {
            _redData = std::make_shared<PixmapData>(QPixmap::fromImage(convertToGrayscaleImage(redImage)));
        }

        if (_checkGreen->isChecked()) {
            _greenData = std::make_shared<PixmapData>(QPixmap::fromImage(convertToGrayscaleImage(greenImage)));
        }

        if (_checkBlue->isChecked()) {
            _blueData = std::make_shared<PixmapData>(QPixmap::fromImage(convertToGrayscaleImage(blueImage)));
        }

        Q_EMIT dataUpdated(0);
        Q_EMIT dataUpdated(1);
        Q_EMIT dataUpdated(2);
    } else {
        _label->setPixmap(QPixmap());
    }
}
