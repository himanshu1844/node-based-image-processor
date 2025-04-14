#include "ImageShowModel.h"

#include "PixmapData.h"

#include <QtNodes/NodeDelegateModelRegistry>

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QMessageBox>
#include <QtWidgets/QFileDialog>

ImageShowModel::ImageShowModel()
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

unsigned int ImageShowModel::nPorts(PortType portType) const
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

bool ImageShowModel::eventFilter(QObject *object, QEvent *event)
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

NodeDataType ImageShowModel::dataType(PortType const, PortIndex const) const
{
    return PixmapData().type();
}

std::shared_ptr<NodeData> ImageShowModel::outData(PortIndex)
{
    return _nodeData;
}


void ImageShowModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = nodeData;
    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<PixmapData>(_nodeData);
        if (d) {
            _currentImage = d->pixmap(); // Store the current image
            int w = _label->width();
            int h = _label->height();
            _label->setPixmap(_currentImage.scaled(w, h, Qt::KeepAspectRatio));
        }
    } else {
        _currentImage = QPixmap(); // Clear the stored image
        _label->setPixmap(QPixmap());
    }
}
QPixmap ImageShowModel::getCurrentImage() const
{
    return _currentImage;
}
void ImageShowModel::saveImage()
{
    // Check if we have a valid image to save
    if (_currentImage.isNull()) {
        QMessageBox::warning(nullptr, "Save Error", "No image available to save.");
        return;
    }

    // Open file dialog to get save location
    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        "Save Image",
        QDir::homePath(),
        "Images (*.png *.jpg *.bmp);;All Files (*)"
        );

    if (filePath.isEmpty()) {
        // User canceled the dialog
        return;
    }

    // Add default extension if none specified
    if (!filePath.contains(".")) {
        filePath += ".png";
    }

    // Save the image
    if (!_currentImage.save(filePath)) {
        QMessageBox::critical(nullptr, "Save Error", "Failed to save image to " + filePath);
    } else {
        QMessageBox::information(nullptr, "Save Success", "Image saved successfully to " + filePath);
    }
}
