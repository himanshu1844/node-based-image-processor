#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QToolButton>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QAction>
#include <QWidget>
#include <QVBoxLayout>
#include <QtNodes/NodeDelegateModel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QButtonGroup>
#include <QGroupBox>
#include <QGridLayout>


#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QVector>
#include "ImageLoaderModel.h"
#include "ImageShowModel.h"
#include "BrightnessNode.h"
#include "Splitter.h"
#include "BlurNode.h"
#include "Threshold.h"
#include "edge.h"
#include "Blend.h"
#include "Noise.h"
#include "Filter.h"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeId;

static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<NodeDelegateModelRegistry>();
    ret->registerModel<ImageShowModel>();
    ret->registerModel<ImageLoaderModel>();
    ret->registerModel<BrightnessNode>();
    ret->registerModel<Splitter>();
    ret->registerModel<BlurNode>();
    ret->registerModel<ThresholdNode>();
    ret->registerModel<Edge>();
    ret->registerModel<Blend>();
    ret->registerModel<Noise>();
    ret->registerModel<FilterNode>();
    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::shared_ptr<NodeDelegateModelRegistry> registry = registerDataModels();
    DataFlowGraphModel dataFlowGraphModel(registry);
    DataFlowGraphicsScene scene(dataFlowGraphModel);
    GraphicsView *view = new GraphicsView(&scene);

    // === Create Main Window ===
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Image Processor");
    // mainWindow.showMaximized();


    // === Left Sidebar ===
    QWidget *leftSidebar = new QWidget();
    leftSidebar->setMinimumWidth(100);  // Allows resizing, minimum width 100
    leftSidebar->setMaximumWidth(300);  // Optional: prevent sidebar from becoming too wide


    // leftSidebar->setStyleSheet("background-color: black;");
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("Tools"));

    QToolButton* Imageloader = new QToolButton();
    Imageloader->setText("Image Loader");
    Imageloader->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/input.png"));
    Imageloader->setIconSize(QSize(42, 42)); // Set your desired size
    Imageloader->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    Imageloader->setFixedSize(70, 70);
    leftLayout->addWidget(Imageloader);


    // Connect button click to add an Brightness node
    QObject::connect(Imageloader, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("ImageLoaderModel");


            // Set some properties for the node
            dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
            dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });
    QToolButton* imageshow = new QToolButton();
    imageshow->setText("Image Show");
    imageshow->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/output.png"));
    imageshow->setIconSize(QSize(42, 42)); // Set your desired size
    imageshow->setFixedSize(70, 70);
    imageshow->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    leftLayout->addWidget(imageshow);


    // Connect button click to add an Brightness node
    QObject::connect(imageshow, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("ImageShowModel");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* Brightness = new QToolButton();
    Brightness->setText("Brightness");
    Brightness->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/brightness.png"));
    Brightness->setIconSize(QSize(32, 32)); // Set your desired size
    Brightness->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    Brightness->setFixedSize(70, 70);
    leftLayout->addWidget(Brightness);


    // Connect button click to add an Brightness node
    QObject::connect(Brightness, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("BrightnessNode");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* splitter = new QToolButton();
    splitter->setText("Splitter");
    splitter->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/splitter.png"));
    splitter->setIconSize(QSize(42, 42)); // Set your desired size
    splitter->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    splitter->setFixedSize(70, 70);
    leftLayout->addWidget(splitter);


    // Connect button click to add an Brightness node
    QObject::connect(splitter, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("Splitter");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* blur = new QToolButton();
    blur->setText("Blur");
    blur->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/blur.png"));
    blur->setIconSize(QSize(42, 42)); // Set your desired size
    blur->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    blur->setFixedSize(70, 70);
    leftLayout->addWidget(blur);


    // Connect button click to add an Brightness node
    QObject::connect(blur, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("BlurNode");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* threshold = new QToolButton();
    threshold->setText("Threshold");
    threshold->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/threshold.png"));
    threshold->setIconSize(QSize(42, 42)); // Set your desired size
    threshold->setFixedSize(70, 70);
    threshold->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    leftLayout->addWidget(threshold);


    // Connect button click to add an Brightness node
    QObject::connect(threshold, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("ThresholdNode");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* edge = new QToolButton();
    edge->setText("Edge Detection");
    edge->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/edge.png"));
    edge->setIconSize(QSize(42, 42)); // Set your desired size
    edge->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    edge->setFixedSize(70, 70);
    leftLayout->addWidget(edge);


    // Connect button click to add an Brightness node
    QObject::connect(edge, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("Edge");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* blend = new QToolButton();
    blend->setText("Blend Mode");
    blend->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/blend.png"));
    blend->setIconSize(QSize(42, 42)); // Set your desired size
    blend->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    blend->setFixedSize(70, 70);
    leftLayout->addWidget(blend);


    // Connect button click to add an Brightness node
    QObject::connect(blend, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("Blend");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* noise = new QToolButton();
    noise->setText("Noise");
   noise->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/noise.png"));
    noise->setIconSize(QSize(42, 42)); // Set your desired size
    noise->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    noise->setFixedSize(70, 70);
    leftLayout->addWidget(noise);


    // Connect button click to add an Brightness node
    QObject::connect(noise, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("Noise");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });

    QToolButton* filter_n = new QToolButton();
    filter_n->setText("Filter");
    filter_n->setIcon(QIcon(":/icons/filter.png"));
    filter_n->setIconSize(QSize(42, 42)); // Set your desired size
    filter_n->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    filter_n->setFixedSize(70, 70);
    leftLayout->addWidget(filter_n);


    // Connect button click to add an Brightness node
    QObject::connect(filter_n, &QToolButton::clicked, [&dataFlowGraphModel]() {
        // Create a node with the ImageShowModel type
        QtNodes::NodeId nodeId = dataFlowGraphModel.addNode("FilterNode");


        // Set some properties for the node
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Position, QPointF(100, 100));
        dataFlowGraphModel.setNodeData(nodeId, QtNodes::NodeRole::Caption, QString("Image Viewer"));

    });




    leftLayout->addStretch();
    leftSidebar->setLayout(leftLayout);

    // === Right Sidebar ===
    QWidget *rightSidebar = new QWidget();
    rightSidebar->setMinimumWidth(300);
    rightSidebar->setMaximumWidth(500);

    // rightSidebar->setStyleSheet("background-color: black;");
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // rightLayout->addStretch();
    rightSidebar->setLayout(rightLayout);




    //brightness slider
    QLabel *sliderLabel = new QLabel("Brightness:");
    QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setRange(-100, 100);
    brightnessSlider->setValue(0);
    sliderLabel->setVisible(false);
    brightnessSlider->setVisible(false);
    rightLayout->addWidget(sliderLabel);
    rightLayout->addWidget(brightnessSlider);



    //contrast slider
    QLabel *sliderLabel_c = new QLabel("Contrast:");
    QSlider *ConstrastSlider = new QSlider(Qt::Horizontal);
    ConstrastSlider->setRange(0, 3);
   ConstrastSlider->setValue(0);
    sliderLabel_c->setVisible(false);
    ConstrastSlider->setVisible(false);
    rightLayout->addWidget(sliderLabel_c);
    rightLayout->addWidget(ConstrastSlider);



    //BLUR NODE RADIUS
    QLabel *sliderLabel_br = new QLabel("BLUR RADIUS:");
    QSlider *RadiusSlider = new QSlider(Qt::Horizontal);
    QLabel *blurModeLabel = new QLabel("Blur Mode:");
    QCheckBox *Uniform= new QCheckBox("Gaussian Uniform");
    QCheckBox *Horizontal = new QCheckBox("Horizontal Directional");
    QCheckBox *Vertical = new QCheckBox("Vertical Directional");
    RadiusSlider->setRange(1, 20);
    RadiusSlider->setValue(1);
    // Initially hidden
    blurModeLabel->setVisible(false);
    Uniform->setVisible(false);
    Horizontal->setVisible(false);
    Vertical->setVisible(false);
    sliderLabel_br->setVisible(false);
    RadiusSlider->setVisible(false);
     // Add to the right sidebar layout
    rightLayout->addWidget(blurModeLabel);
    rightLayout->addWidget(Uniform);
    rightLayout->addWidget(Horizontal);
    rightLayout->addWidget(Vertical);
    rightLayout->addWidget(sliderLabel_br);
    rightLayout->addWidget(RadiusSlider);

    //create exclusive checkbox
    QButtonGroup *blurModeGroup = new QButtonGroup();
    blurModeGroup->setExclusive(true);  // ensures only one checkbox can be selected at a time

    blurModeGroup->addButton(Uniform, 0);
    blurModeGroup->addButton(Horizontal, 1);
    blurModeGroup->addButton(Vertical, 2);

    //ThresholdNode
    QLabel *sliderLabel_tr = new QLabel(" Threshold:");
    QSlider *ThresholdSlider = new QSlider(Qt::Horizontal);
    QLabel *thresholdmethod = new QLabel("Thresholding Method:");
    QCheckBox *Binary= new QCheckBox("Bianry");
    QCheckBox *Adaptive = new QCheckBox("Adaptive");
    QCheckBox *Otsu = new QCheckBox("Otsu");
    ThresholdSlider->setRange(0, 255);
    ThresholdSlider->setValue(127);
    // Initially hidden
    thresholdmethod->setVisible(false);
    Binary->setVisible(false);
    Adaptive->setVisible(false);
    Otsu->setVisible(false);
    sliderLabel_tr->setVisible(false);
    ThresholdSlider->setVisible(false);
        // Add to the right sidebar layout
    rightLayout->addWidget(thresholdmethod);
    rightLayout->addWidget(Binary);
    rightLayout->addWidget(Adaptive);
    rightLayout->addWidget(Otsu);
    rightLayout->addWidget(sliderLabel_tr);
    rightLayout->addWidget(ThresholdSlider);

    //create exclusive checkbox
    QButtonGroup *thresholdModeGroup = new QButtonGroup();
    thresholdModeGroup ->setExclusive(true);  // ensures only one checkbox can be selected at a time

    thresholdModeGroup ->addButton(Binary, 0);
    thresholdModeGroup ->addButton(Adaptive, 1);
    thresholdModeGroup ->addButton(Otsu, 2);

    //edge  Node
    QCheckBox *overlay=new QCheckBox("Overlay Image Over Original");
    QLabel *sliderLabel_ed = new QLabel(" Threshold:");
    QSlider *ThresholdSlider_ed = new QSlider(Qt::Horizontal);
    QLabel *edgemethod = new QLabel("edge detection Method:");
    QCheckBox *Sobel= new QCheckBox("Sobel");
    QCheckBox *Canny = new QCheckBox("Canny");
    QLabel *kernelSize_label= new QLabel("Kernel Size:");
    QComboBox* kernelSize = new QComboBox;
    kernelSize->setObjectName("kernelSizeDropdown");

    // Add values to the dropdown
    kernelSize->addItem("1");
    kernelSize->addItem("3");
    kernelSize->addItem("5");
    kernelSize->addItem("7");

    ThresholdSlider_ed->setRange(0, 255);
    ThresholdSlider_ed->setValue(127);
    // Initially hidden
    overlay->setVisible(false);
    kernelSize_label->setVisible(false);
    kernelSize->setVisible(false);
    edgemethod->setVisible(false);
    Sobel->setVisible(false);
    Canny->setVisible(false);
    sliderLabel_ed->setVisible(false);
    ThresholdSlider_ed->setVisible(false);
        // Add to the right sidebar layout
    rightLayout->addWidget(overlay);
    rightLayout->addWidget(edgemethod);
    rightLayout->addWidget(Sobel);
    rightLayout->addWidget(Canny);
    rightLayout->addWidget(kernelSize_label);
    rightLayout->addWidget(kernelSize);

    rightLayout->addWidget(sliderLabel_ed);
    rightLayout->addWidget(ThresholdSlider_ed);

    //create exclusive checkbox
    QButtonGroup *thresholdModeGroup_ed = new QButtonGroup();
    thresholdModeGroup_ed ->setExclusive(true);  // ensures only one checkbox can be selected at a time

    thresholdModeGroup_ed ->addButton(Sobel, 0);
    thresholdModeGroup_ed->addButton(Canny, 1);


    //Blend Mode
    QLabel *Blendmethod = new QLabel("Blend Mode:");
    QCheckBox *Normal= new QCheckBox("Normal");
    QCheckBox *Multiply = new QCheckBox("Multiply");
    QCheckBox *Screen= new QCheckBox("Screen");
    QCheckBox *Overlay=new QCheckBox("Overlay");
    QCheckBox *Difference=new QCheckBox("Difference");
    QLabel *opacityslider_label=new QLabel("opacity");
    QSlider *opacityslider=new QSlider(Qt::Horizontal);
    QLabel *mixslider_label=new QLabel("Mix");
    QSlider *mixslider=new QSlider(Qt::Horizontal);
    QButtonGroup *blendmethod_gr = new QButtonGroup();
    blendmethod_gr ->setExclusive(true);  // ensures only one checkbox can be selected at a time

    blendmethod_gr->addButton(Normal, 0);
    blendmethod_gr->addButton(Multiply, 1);
    blendmethod_gr->addButton(Screen,2);
    blendmethod_gr->addButton(Overlay, 3);
    blendmethod_gr->addButton(Difference, 4);



    opacityslider->setRange(0,100);
    opacityslider->setValue(50);
    mixslider->setRange(0,100);
    mixslider->setValue(50);
    rightLayout->addWidget(Blendmethod);
    rightLayout->addWidget(Normal);
    rightLayout->addWidget(Multiply);
    rightLayout->addWidget(Screen);
    rightLayout->addWidget(Overlay);
    rightLayout->addWidget(Difference);
    rightLayout->addWidget(opacityslider_label);
    rightLayout->addWidget(opacityslider);
    rightLayout->addWidget(mixslider_label);
    rightLayout->addWidget(mixslider);


    Blendmethod->setVisible(false);
    Normal->setVisible(false);
    Multiply->setVisible(false);
    Screen->setVisible(false);
    Overlay->setVisible(false);
    Difference->setVisible(false);
    opacityslider_label->setVisible(false);
    opacityslider->setVisible(false);
    mixslider_label->setVisible(false);
    mixslider->setVisible(false);







    QButtonGroup *blendmethodgroup = new QButtonGroup();
    blendmethodgroup ->setExclusive(true);  // ensures only one checkbox can be selected at a time

    blendmethodgroup ->addButton(Normal, 0);
    blendmethodgroup ->addButton(Multiply, 1);
    blendmethodgroup->addButton(Screen, 2);
    blendmethodgroup->addButton(Overlay, 3);
    blendmethodgroup->addButton(Difference, 4);


    //Noise Node
    QLabel *noisepattern=new QLabel("Noise Pattern");
    QCheckBox *Perlin= new QCheckBox("Perlin");
    QCheckBox *Simplex = new QCheckBox("Simplex");
    QCheckBox *Worley= new QCheckBox("Worley");
    QButtonGroup *noisepattern_grp = new QButtonGroup();
    noisepattern_grp ->setExclusive(true);  // ensures only one checkbox can be selected at a time

    noisepattern_grp ->addButton(Perlin, 0);
    noisepattern_grp->addButton(Simplex, 1);
    noisepattern_grp->addButton(Worley, 2);

    QLabel *use_noise=new QLabel("Use Noise as:");
    QCheckBox *Displacement= new QCheckBox("Displacement Map");
    QCheckBox *Direct = new QCheckBox("Direct Color Output");

    QButtonGroup *noise_use = new QButtonGroup();
    noise_use ->setExclusive(true);  // ensures only one checkbox can be selected at a time

    noise_use ->addButton(Displacement, 0);
    noise_use->addButton(Direct, 1);



    QLabel *scale_label=new QLabel("Scale");
    QSlider *scaleslider=new QSlider(Qt::Horizontal);

    QLabel *octave_label=new QLabel("Octave");
    QSlider *octaveslider=new QSlider(Qt::Horizontal);

    QLabel *persistence_label=new QLabel("Persistence");
    QSlider *persitenceslider=new QSlider(Qt::Horizontal);

    scaleslider->setRange(1,500);
    scaleslider->setValue(100);
    octaveslider->setRange(1,8);
    octaveslider->setValue(4);
    persitenceslider->setRange(1,9);
    persitenceslider->setValue(5);




    rightLayout->addWidget(noisepattern);
    rightLayout->addWidget(Perlin);
    rightLayout->addWidget(Simplex);
    rightLayout->addWidget(Worley);
    rightLayout->addWidget(use_noise);
    rightLayout->addWidget(Displacement);
    rightLayout->addWidget(Direct);
    rightLayout->addWidget(scale_label);
    rightLayout->addWidget(scaleslider);
    rightLayout->addWidget(octave_label);
    rightLayout->addWidget(octaveslider);
    rightLayout->addWidget(persistence_label);
    rightLayout->addWidget(persitenceslider);


    noisepattern->setVisible(false);
    Perlin->setVisible(false);
    Simplex->setVisible(false);
    Worley->setVisible(false);
    use_noise->setVisible(false);
    Displacement->setVisible(false);
    Direct->setVisible(false);
    scale_label->setVisible(false);
    scaleslider->setVisible(false);
    octave_label->setVisible(false);
    octaveslider->setVisible(false);
    persistence_label->setVisible(false);
    persitenceslider->setVisible(false);


    //convulational Filter Node
    QLabel *filter=new QLabel("Filter");
    QCheckBox *Sharpness= new QCheckBox("Sharpness");
    QCheckBox *Emboss = new QCheckBox("Emboss");
    QCheckBox *Edgeenhance= new QCheckBox("Edge Enhance");
     QCheckBox *Custom= new QCheckBox("Custom Kernel");
    QButtonGroup *filter_gr = new QButtonGroup();
    filter_gr ->setExclusive(true);  // ensures only one checkbox can be selected at a time

    filter_gr ->addButton(Sharpness, 0);
    filter_gr->addButton(Emboss, 1);
    filter_gr->addButton(Edgeenhance, 2);
    filter_gr->addButton(Custom,3);
    QComboBox* kernel = new QComboBox;
    kernelSize->setObjectName("kernelSizeDropdown");

    // Add values to the dropdown
    kernel->addItem("3*3");
    kernel->addItem("5*5");




    // QGridLayout *matrixLayout = new QGridLayout();
    // matrixLayout->setSpacing(6);

    // // Create 3x3 matrix of line edits
    // const int rows = 3;
    // const int cols = 3;
    // QVector<QVector<QLineEdit*>> cells(rows, QVector<QLineEdit*>(cols));

    // for (int i = 0; i < rows; ++i) {
    //     for (int j = 0; j < cols; ++j) {
    //         QLineEdit *cell = new QLineEdit();
    //         cell->setFixedSize(80, 30);
    //         cell->setAlignment(Qt::AlignCenter);
    //         cell->setStyleSheet("border: 1px solid #3878B8; border-radius: 3px;");
    //         matrixLayout->addWidget(cell, i, j);
    //         cells[i][j] = cell;
    //     }
    // }
    // QPushButton *saveButton = new QPushButton("Save Matrix");
    // saveButton->setStyleSheet("background-color: #3878B8; color: white; padding: 5px;");
    // QWidget *matrixWidget = new QWidget();
    // matrixWidget->setLayout(matrixLayout);
    rightLayout->addWidget(filter);
    rightLayout->addWidget(Sharpness);
    rightLayout->addWidget(Emboss);
    rightLayout->addWidget(Edgeenhance);
    rightLayout->addWidget(Custom);
    rightLayout->addWidget(kernel);
    // rightLayout->addWidget(matrixWidget);
    // rightLayout->addWidget(saveButton);
    filter->setVisible(false);
    Sharpness->setVisible(false);
    Emboss->setVisible(false);
    Edgeenhance->setVisible(false);
    Custom->setVisible(false);
    kernel->setVisible(false);
    // matrixWidget->setVisible(false);
    // saveButton->setVisible(false);


    QVector<QVector<double>> sharpnessMatrix = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };
    QVector<QVector<double>> embossMatrix = {
        {-2, -1, 0},
        {-1, 1, 1},
        {0, 1, 2}
    };
    QVector<QVector<double>> edgeEnhanceMatrix = {
        {0, 0, 0},
        {-1, 1, 0},
        {0, 0, 0}
    };

    //on click
    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto filter_ = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<FilterNode>(nodeId);

                         if (filter_) {
                             filter->setVisible(true);
                             Sharpness->setVisible(true);
                             Emboss->setVisible(true);
                             Edgeenhance->setVisible(true);
                             Custom->setVisible(true);
                             QObject::disconnect(filter_gr, nullptr, nullptr, nullptr);
                             QObject::connect(filter_gr, &QButtonGroup::idClicked, [=](int id) {
                                 switch (id) {
                                 case 0:
                                     kernel->setVisible(false);
                                     for (int i = 0; i < rightLayout->count(); ++i) {
                                         QWidget *widget = rightLayout->itemAt(i)->widget();
                                         if (widget && widget != kernel && widget != filter &&
                                             widget != Sharpness && widget != Emboss && widget != Edgeenhance && widget != Custom) {
                                             widget->deleteLater();
                                         }
                                     }
                                     filter_->setmatrix(sharpnessMatrix);
                                     break;
                                 case 1:
                                      kernel->setVisible(false);
                                     for (int i = 0; i < rightLayout->count(); ++i) {
                                         QWidget *widget = rightLayout->itemAt(i)->widget();
                                         if (widget && widget != kernel && widget != filter &&
                                             widget != Sharpness && widget != Emboss && widget != Edgeenhance && widget != Custom) {
                                             widget->deleteLater();
                                         }
                                     }
                                     filter_->setmatrix(embossMatrix);
                                     break;
                                 case 2:
                                      kernel->setVisible(false);
                                     for (int i = 0; i < rightLayout->count(); ++i) {
                                         QWidget *widget = rightLayout->itemAt(i)->widget();
                                         if (widget && widget != kernel && widget != filter &&
                                             widget != Sharpness && widget != Emboss && widget != Edgeenhance && widget != Custom) {
                                             widget->deleteLater();
                                         }
                                     }
                                     filter_->setmatrix(edgeEnhanceMatrix);
                                     break;
                                 case 3:
                                     kernel->setVisible(true);
                                     // First, clear any existing matrix widget
                                     for (int i = 0; i < rightLayout->count(); ++i) {
                                         QWidget *widget = rightLayout->itemAt(i)->widget();
                                         if (widget && widget != kernel && widget != filter &&
                                             widget != Sharpness && widget != Emboss && widget != Edgeenhance && widget != Custom) {
                                             widget->deleteLater();
                                         }
                                     }

                                     // Generate 3x3 matrix by default
                                     QGridLayout *matrixLayout = new QGridLayout();
                                     matrixLayout->setSpacing(6);

                                     // Create 3x3 matrix of line edits
                                     const int rows = 3;
                                     const int cols = 3;
                                     QVector<QVector<QLineEdit*>> cells(rows, QVector<QLineEdit*>(cols));

                                     for (int i = 0; i < rows; ++i) {
                                         for (int j = 0; j < cols; ++j) {
                                             QLineEdit *cell = new QLineEdit();
                                             cell->setFixedSize(80, 30);
                                             cell->setAlignment(Qt::AlignCenter);
                                             cell->setStyleSheet("border: 1px solid #3878B8; border-radius: 3px;");
                                             matrixLayout->addWidget(cell, i, j);
                                             cells[i][j] = cell;
                                         }
                                     }

                                     QPushButton *saveButton = new QPushButton("Save Matrix");
                                     saveButton->setStyleSheet("background-color: #3878B8; color: white; padding: 5px;");
                                     QWidget *matrixWidget = new QWidget();
                                     matrixWidget->setLayout(matrixLayout);
                                     rightLayout->addWidget(matrixWidget);
                                     rightLayout->addWidget(saveButton);
                                     QObject::connect(saveButton, &QPushButton::clicked, [=]() {
                                         QVector<QVector<double>> matrix(rows, QVector<double>(cols));
                                         for (int i = 0; i < rows; ++i) {
                                             for (int j = 0; j < cols; ++j) {
                                                 bool ok;
                                                 double value = cells[i][j]->text().toDouble(&ok);
                                                 matrix[i][j] = ok ? value : 0.0; // Default to 0 if conversion fails
                                             }
                                         }
                                         filter_->setmatrix(matrix);
                                     });

                                     // Then set up the dropdown connection for future changes
                                     QObject::disconnect(kernel, nullptr, nullptr, nullptr);
                                     QObject::connect(kernel, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
                                         QString selectedText = kernel->itemText(index);

                                         // Clear any existing matrix widget
                                         for (int i = 0; i < rightLayout->count(); ++i) {
                                             QWidget *widget = rightLayout->itemAt(i)->widget();
                                             if (widget && widget != kernel && widget != filter &&
                                                 widget != Sharpness && widget != Emboss && widget != Edgeenhance && widget != Custom) {
                                                 widget->deleteLater();
                                             }
                                         }

                                         if (selectedText == "3*3") {
                                             // Create 3x3 matrix (same code as default)
                                             QGridLayout *matrixLayout = new QGridLayout();
                                             matrixLayout->setSpacing(6);

                                             const int rows = 3;
                                             const int cols = 3;
                                             QVector<QVector<QLineEdit*>> cells(rows, QVector<QLineEdit*>(cols));

                                             for (int i = 0; i < rows; ++i) {
                                                 for (int j = 0; j < cols; ++j) {
                                                     QLineEdit *cell = new QLineEdit();
                                                     cell->setFixedSize(80, 30);
                                                     cell->setAlignment(Qt::AlignCenter);
                                                     cell->setStyleSheet("border: 1px solid #3878B8; border-radius: 3px;");
                                                     matrixLayout->addWidget(cell, i, j);
                                                     cells[i][j] = cell;
                                                 }
                                             }

                                             QPushButton *saveButton = new QPushButton("Save Matrix");
                                             saveButton->setStyleSheet("background-color: #3878B8; color: white; padding: 5px;");
                                             QWidget *matrixWidget = new QWidget();
                                             matrixWidget->setLayout(matrixLayout);
                                             rightLayout->addWidget(matrixWidget);
                                             rightLayout->addWidget(saveButton);
                                             QObject::connect(saveButton, &QPushButton::clicked, [=]() {
                                                 QVector<QVector<double>> matrix(rows, QVector<double>(cols));
                                                 for (int i = 0; i < rows; ++i) {
                                                     for (int j = 0; j < cols; ++j) {
                                                         bool ok;
                                                         double value = cells[i][j]->text().toDouble(&ok);
                                                         matrix[i][j] = ok ? value : 0.0; // Default to 0 if conversion fails
                                                     }
                                                 }
                                                 filter_->setmatrix(matrix);
                                             });
                                         }
                                         else if (selectedText == "5*5") {
                                             // Create 5x5 matrix
                                             QGridLayout *matrixLayout = new QGridLayout();
                                             matrixLayout->setSpacing(6);

                                             const int rows = 5;
                                             const int cols = 5;
                                             QVector<QVector<QLineEdit*>> cells(rows, QVector<QLineEdit*>(cols));

                                             for (int i = 0; i < rows; ++i) {
                                                 for (int j = 0; j < cols; ++j) {
                                                     QLineEdit *cell = new QLineEdit();
                                                     cell->setFixedSize(60, 30);
                                                     cell->setAlignment(Qt::AlignCenter);
                                                     cell->setStyleSheet("border: 1px solid #3878B8; border-radius: 3px;");
                                                     matrixLayout->addWidget(cell, i, j);
                                                     cells[i][j] = cell;
                                                 }
                                             }

                                             QPushButton *saveButton = new QPushButton("Save Matrix");
                                             saveButton->setStyleSheet("background-color: #3878B8; color: white; padding: 5px;");
                                             QWidget *matrixWidget = new QWidget();
                                             matrixWidget->setLayout(matrixLayout);
                                             rightLayout->addWidget(matrixWidget);
                                             rightLayout->addWidget(saveButton);
                                             QObject::connect(saveButton, &QPushButton::clicked, [=]() {
                                                 QVector<QVector<double>> matrix(rows, QVector<double>(cols));
                                                 for (int i = 0; i < rows; ++i) {
                                                     for (int j = 0; j < cols; ++j) {
                                                         bool ok;
                                                         double value = cells[i][j]->text().toDouble(&ok);
                                                         matrix[i][j] = ok ? value : 0.0; // Default to 0 if conversion fails
                                                     }
                                                 }
                                                 filter_->setmatrix(matrix);
                                             });
                                         }
                                     });
                                     break;
                                 }


                             });





                         } else {
                             filter->setVisible(false);
                             Sharpness->setVisible(false);
                             Emboss->setVisible(false);
                             Edgeenhance->setVisible(false);
                             Custom->setVisible(false);
                             kernel->setVisible(false);
                             // matrixWidget->setVisible(false);
                             // saveButton->setVisible(false);
                         }
                     });
    //save button

    //on click
    auto imageShowModel = std::make_unique<ImageShowModel>();

    // Create save button and add to layout
    QPushButton *saveButton = new QPushButton("Save Image");
    saveButton->setVisible(false);
    rightLayout->addWidget(saveButton);

    ImageShowModel* currentImageShowModel = nullptr;

    // Connect the save button click to the ImageShowModel's saveImage slot
    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto imageshow = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<ImageShowModel>(nodeId);

                         if (imageshow) {
                             currentImageShowModel = imageshow;
                             saveButton->setVisible(true);
                         } else {
                             currentImageShowModel = nullptr;
                             saveButton->setVisible(false);
                         }
                     });
    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        if (currentImageShowModel) {
            currentImageShowModel->saveImage();  // This must be a public slot
        }
    });



     rightLayout->addStretch();









    // === Layout for Central Area ===
    QWidget *centralWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(leftSidebar);
    mainLayout->addWidget(view);
    mainLayout->addWidget(rightSidebar);
    centralWidget->setLayout(mainLayout);

    mainWindow.setCentralWidget(centralWidget);

    // === Menu Bar ===
    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *fileMenu = menuBar->addMenu("File");

    QAction *newAction = new QAction("New");
    QAction *openAction = new QAction("Open");
    QAction *saveAction = new QAction("Save");
    QAction *aboutAction = new QAction("About");
    QAction *exitAction = new QAction("Exit");

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(aboutAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);




   //brightness/contrast Node on click
    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto brightnessNode = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<BrightnessNode>(nodeId);

                         if (brightnessNode) {
                             sliderLabel->setVisible(true);
                             brightnessSlider->setVisible(true);
                             sliderLabel_c->setVisible(true);
                             ConstrastSlider->setVisible(true);




                             QObject::disconnect(brightnessSlider, nullptr, nullptr, nullptr);
                             QObject::disconnect(ConstrastSlider, nullptr, nullptr, nullptr);

                             QObject::connect(brightnessSlider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  brightnessNode->setBrightnessLevel(static_cast<float>(value));
                                              });
                             QObject::connect(ConstrastSlider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  brightnessNode->setContrastLevel(static_cast<float>(value));
                                              });
                         } else {
                             sliderLabel->setVisible(false);
                             brightnessSlider->setVisible(false);
                             sliderLabel_c->setVisible(false);
                             ConstrastSlider->setVisible(false);
                         }
                     });




    //BLUR NODE on click
    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto blurNode = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<BlurNode>(nodeId);

                         if (blurNode) {
                             sliderLabel_br->setVisible(true);
                             RadiusSlider->setVisible(true);
                             blurModeLabel->setVisible(true);
                             Uniform->setVisible(true);
                             Horizontal->setVisible(true);
                             Vertical->setVisible(true);





                             QObject::disconnect(RadiusSlider, nullptr, nullptr, nullptr);


                             QObject::connect(RadiusSlider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  blurNode->setRadiusLevel(value);
                                              });
                               QObject::disconnect(blurModeGroup, nullptr, nullptr, nullptr);
                             QObject::connect(blurModeGroup, &QButtonGroup::idClicked, [=](int id) {
                                 switch (id) {
                                 case 0:
                                     blurNode->setBlurMode("uniform");
                                     break;
                                 case 1:
                                     blurNode->setBlurMode("horizontal");
                                     break;
                                 case 2:
                                     blurNode->setBlurMode("vertical");
                                     break;
                                 }
                             });

                         } else {
                             sliderLabel_br->setVisible(false);
                             RadiusSlider->setVisible(false);
                             blurModeLabel->setVisible(false);
                             Uniform->setVisible(false);
                             Horizontal->setVisible(false);
                             Vertical->setVisible(false);

                         }
                     });

    //threshold Node on click
    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto thresholdNode = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<ThresholdNode>(nodeId);

                         if (thresholdNode) {
                             thresholdmethod->setVisible(true);
                             Binary->setVisible(true);
                             Adaptive->setVisible(true);
                             Otsu->setVisible(true);
                             sliderLabel_tr->setVisible(true);
                             ThresholdSlider->setVisible(true);





                             QObject::disconnect(ThresholdSlider, nullptr, nullptr, nullptr);


                             QObject::connect(ThresholdSlider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  thresholdNode->setthreshold(value);
                                              });
                             QObject::disconnect(thresholdModeGroup, nullptr, nullptr, nullptr);
                             QObject::connect(thresholdModeGroup, &QButtonGroup::idClicked, [=](int id) {
                                 switch (id) {
                                 case 0:
                                     thresholdNode->setthresholdmethod("binary");
                                     break;
                                 case 1:
                                     thresholdNode->setthresholdmethod("adaptive");
                                     break;
                                 case 2:
                                     thresholdNode->setthresholdmethod("Otsu");
                                     break;
                                 }
                             });

                         } else {
                             thresholdmethod->setVisible(false);
                             Binary->setVisible(false);
                             Adaptive->setVisible(false);
                             Otsu->setVisible(false);
                             sliderLabel_tr->setVisible(false);
                             ThresholdSlider->setVisible(false);

                         }
                     });

    //edge detection Node on click
    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto edgeNode = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<Edge>(nodeId);

                         if (edgeNode) {
                             overlay->setVisible(true);
                              kernelSize_label->setVisible(true);
                             kernelSize->setVisible(true);
                             edgemethod->setVisible(true);
                             Sobel->setVisible(true);
                             Canny->setVisible(true);
                             sliderLabel_ed->setVisible(true);
                             ThresholdSlider_ed->setVisible(true);





                             QObject::disconnect(ThresholdSlider_ed, nullptr, nullptr, nullptr);


                             QObject::connect(ThresholdSlider_ed, &QSlider::valueChanged,
                                              [=](int value) {
                                                  edgeNode->setthreshold(value);
                                              });
                             QObject::disconnect( thresholdModeGroup_ed, nullptr, nullptr, nullptr);
                             QObject::connect( thresholdModeGroup_ed, &QButtonGroup::idClicked, [=](int id) {
                                 switch (id) {
                                 case 0:
                                     edgeNode->setedgemethod("Sobel");
                                     break;
                                 case 1:
                                     edgeNode->setedgemethod("Canny");
                                     break;

                                 }
                             });
                             QObject::disconnect(kernelSize, nullptr, nullptr, nullptr);
                             QObject::connect(kernelSize, QOverload<int>::of(&QComboBox::currentIndexChanged),  [=](int index) {
                                 QString selectedText = kernelSize->itemText(index);
                                 int kernelSize = selectedText.toInt(); // Convert "1", "3", "5", etc. to int

                                 edgeNode->setKernelSize(kernelSize);
                             });
                             QObject::disconnect(overlay, nullptr, nullptr, nullptr);
                             QObject::connect(overlay, &QCheckBox::toggled, overlay, [edgeNode](bool checked) {
                                 edgeNode->setOverlay(checked);
                             });

                         } else {
                             overlay->setVisible(false);
                              kernelSize_label->setVisible(false);
                             kernelSize->setVisible(false);
                             edgemethod->setVisible(false);
                             Sobel->setVisible(false);
                             Canny->setVisible(false);
                             sliderLabel_ed->setVisible(false);
                             ThresholdSlider_ed->setVisible(false);

                         }
                     });

    //blend Mode on click

    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto blend = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<Blend>(nodeId);

                         if (blend) {
                             Blendmethod->setVisible(true);
                             Normal->setVisible(true);
                             Multiply->setVisible(true);
                             Screen->setVisible(true);
                             Overlay->setVisible(true);
                             Difference->setVisible(true);
                             opacityslider_label->setVisible(true);
                             opacityslider->setVisible(true);
                             mixslider_label->setVisible(true);
                             mixslider->setVisible(true);





                             QObject::disconnect(opacityslider, nullptr, nullptr, nullptr);
                             QObject::disconnect(mixslider, nullptr, nullptr, nullptr);

                             QObject::connect(opacityslider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  blend->setopacityLevel(value);
                                              });
                             QObject::connect(mixslider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  blend->setmixLevel(value);
                                              });
                             QObject::disconnect( blendmethodgroup, nullptr, nullptr, nullptr);
                             QObject::connect( blendmethodgroup, &QButtonGroup::idClicked, [=](int id) {
                                 switch (id) {
                                 case 0:
                                     blend->setblendMode("Normal");
                                     break;
                                 case 1:
                                     blend->setblendMode("Multiply");
                                     break;
                                 case 2:
                                     blend->setblendMode("Screen");
                                     break;
                                 case 3:
                                     blend->setblendMode("Overlay");
                                     break;

                                 case 4:
                                     blend->setblendMode("Difference");
                                     break;

                                 }
                             });
                         } else {
                             Blendmethod->setVisible(false);
                             Normal->setVisible(false);
                             Multiply->setVisible(false);
                             Screen->setVisible(false);
                             Overlay->setVisible(false);
                             Difference->setVisible(false);
                             opacityslider_label->setVisible(false);
                             opacityslider->setVisible(false);
                             mixslider_label->setVisible(false);
                             mixslider->setVisible(false);

                         }
                     });


    //Noise Mode on Click
    QObject::connect(&scene, &DataFlowGraphicsScene::nodeClicked,
                     [&](QtNodes::NodeId const &nodeId) {
                         auto &graphModel = scene.graphModel();

                         auto noise = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<Noise>(nodeId);

                         if (noise) {
                             noisepattern->setVisible(true);
                             Perlin->setVisible(true);
                             Simplex->setVisible(true);
                             Worley->setVisible(true);
                             use_noise->setVisible(true);
                             Displacement->setVisible(true);
                             Direct->setVisible(true);
                             scale_label->setVisible(true);
                             scaleslider->setVisible(true);
                             octave_label->setVisible(true);
                             octaveslider->setVisible(true);
                             persistence_label->setVisible(true);
                             persitenceslider->setVisible(true);




                             QObject::disconnect(scaleslider, nullptr, nullptr, nullptr);
                             QObject::disconnect(octaveslider, nullptr, nullptr, nullptr);
                             QObject::disconnect(persitenceslider, nullptr, nullptr, nullptr);
                             QObject::connect(scaleslider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  noise->setscale(value);
                                              });
                             QObject::connect(octaveslider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  noise->setoctave(value);
                                              });
                             QObject::connect(persitenceslider, &QSlider::valueChanged,
                                              [=](int value) {
                                                  noise->setpersistence(value);
                                              });
                             QObject::disconnect( noisepattern_grp, nullptr, nullptr, nullptr);
                             QObject::connect( noisepattern_grp, &QButtonGroup::idClicked, [=](int id) {
                                 switch (id) {
                                 case 0:
                                     noise->setnoisepattern("Perlin");
                                     break;
                                 case 1:
                                    noise->setnoisepattern("Simplex");
                                     break;
                                 case 2:
                                     noise->setnoisepattern("(Worley");
                                     break;


                                 }
                             });
                             QObject::disconnect(noise_use , nullptr, nullptr, nullptr);
                             QObject::connect( noise_use , &QButtonGroup::idClicked, [=](int id) {
                                 switch (id) {
                                 case 0:
                                     noise->setnoiseuse("Displacement");
                                     break;
                                 case 1:
                                     noise->setnoiseuse("Direct");
                                     break;


                                 }
                             });
                         } else {
                             noisepattern->setVisible(false);
                             Perlin->setVisible(false);
                             Simplex->setVisible(false);
                             Worley->setVisible(false);
                             use_noise->setVisible(false);
                             Displacement->setVisible(false);
                             Direct->setVisible(false);
                             scale_label->setVisible(false);
                             scaleslider->setVisible(false);
                             octave_label->setVisible(false);
                             octaveslider->setVisible(false);
                             persistence_label->setVisible(false);
                             persitenceslider->setVisible(false);

                         }
                     });






    // Center window on screen
    mainWindow.move(QApplication::primaryScreen()->availableGeometry().center() - mainWindow.rect().center());
    mainWindow.show();

    return app.exec();
}
