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
#include "ImageLoaderModel.h"
#include "ImageShowModel.h"
#include "BrightnessNode.h"
#include "Splitter.h"
#include "BlurNode.h"
#include "Threshold.h"
#include "edge.h"
#include "Blend.h"
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
    mainWindow.setWindowTitle("Data Flow: Resizable Images");
    // mainWindow.showMaximized();


    // === Left Sidebar ===
    QWidget *leftSidebar = new QWidget();
    leftSidebar->setMinimumWidth(100);  // Allows resizing, minimum width 100
    leftSidebar->setMaximumWidth(300);  // Optional: prevent sidebar from becoming too wide


    // leftSidebar->setStyleSheet("background-color: black;");
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("Tools"));

    QToolButton* loadImageBtn = new QToolButton();
    loadImageBtn->setText("LoadImageBtn");
    loadImageBtn->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/brightness.png"));
    loadImageBtn->setIconSize(QSize(48, 48)); // Set your desired size
    loadImageBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    leftLayout->addWidget(loadImageBtn);

    QToolButton* loadImageBtn1 = new QToolButton();
    loadImageBtn1->setText("Other");
    loadImageBtn1->setIcon(QIcon("C:/Users/Himanshu Pancholi/ImageProcessor/icons/brightness.png"));
    loadImageBtn1->setIconSize(QSize(48, 48));
    loadImageBtn1->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);  // Icon above label
    leftLayout->addWidget(loadImageBtn1);

    // leftLayout->addWidget(new QPushButton("Clear"));
    leftLayout->addStretch();
    leftSidebar->setLayout(leftLayout);

    // === Right Sidebar ===
    QWidget *rightSidebar = new QWidget();
    rightSidebar->setMinimumWidth(100);
    rightSidebar->setMaximumWidth(300);

    // rightSidebar->setStyleSheet("background-color: black;");
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel("Properties"));
    rightLayout->addWidget(new QPushButton("Export"));
    rightLayout->addWidget(new QPushButton("Settings"));
    rightLayout->addStretch();
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
    opacityslider->setValue(0);
    mixslider->setRange(0,100);
    mixslider->setValue(0);
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
                                                  brightnessNode->setConstrastLevel(static_cast<float>(value));
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
                             QObject::connect(ConstrastSlider, &QSlider::valueChanged,
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




    // Center window on screen
    mainWindow.move(QApplication::primaryScreen()->availableGeometry().center() - mainWindow.rect().center());
    mainWindow.show();

    return app.exec();
}
