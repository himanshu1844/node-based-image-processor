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
#include "ImageLoaderModel.h"
#include "ImageShowModel.h"
#include "BrightnessNode.h"
#include "Splitter.h"

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

    // === Layout for Central Area ===
    QWidget *centralWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0); // Optional: remove margins


    mainLayout->addWidget(leftSidebar);
    mainLayout->addWidget(view); // Node Editor in center
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

                         // Use the delegateModel<T>() template method
                         auto brightnessNode = dynamic_cast<DataFlowGraphModel&>(graphModel).delegateModel<BrightnessNode>(nodeId);

                         if (brightnessNode) {
                             sliderLabel->setVisible(true);
                             brightnessSlider->setVisible(true);
                             sliderLabel_c->setVisible(true);
                             ConstrastSlider->setVisible(true);


                             // Optional: uncomment if getBrightnessLevel() exists
                             // brightnessSlider->setValue(static_cast<int>(brightnessNode->getBrightnessLevel()));

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


    // Center window on screen
    mainWindow.move(QApplication::primaryScreen()->availableGeometry().center() - mainWindow.rect().center());
    mainWindow.show();

    return app.exec();
}
