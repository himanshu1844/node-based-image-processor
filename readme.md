# Node-Based Image Processor

A visual, modular image processing system built using **Qt**, **OpenCV**, and **QtNodes**. This project lets users create and connect processing nodes in a graph-based editor interface — inspired by tools like Unreal Engine Blueprints and Blender's Shader Node Editor.
---
![ImageProcessor Application](https://github.com/user-attachments/assets/4f8ae7cb-86ff-466a-a47f-211df9cbd9b9)
1/3
![ImageProcessor Application](https://github.com/user-attachments/assets/8d2ed6e6-b56d-4de7-a4b6-8363004ccaca)
2/3
![ImageProcessor Application](https://github.com/user-attachments/assets/e9ae7061-e072-4937-9150-760c32b1c36a)
3/3

---
## 🛠️ Built With

- **Qt 6.9 / Qt Creator** – GUI framework and IDE used for development.
- **OpenCV** – For all image manipulation and processing logic.
- **QtNodes** – Open-source node editor library used for creating node graphs in C++.

---

## 🎯 Objective

The goal of this application is to provide a **modular, extensible** way to process images visually. Each operation (brightness, blur, etc.) is represented as a "node" that can be connected to other nodes, forming a **dataflow graph**.

---

## 📦 Project Architecture

This application is architected around the **Node Graph Execution Model** using the QtNodes framework. It consists of the following core layers:

### 1. Node Definition Layer

Each node inherits from a base class provided by QtNodes and implements the following:

- **Ports**: Defined using portType and portCaption, typically Input or Output.
- **Embedded QWidget**: A small UI component (like a slider) to control parameters.
- **compute() method**: Executes the core logic using OpenCV.
- **DataModel**: Represents the logic of the node and handles data propagation.

### 2. Graph Execution Layer (QtNodes Engine)

- QtNodes constructs a **graph** where each node's output is passed to its downstream connected nodes.
- The engine manages:
  - **Node connections and layout**
  - **Data propagation**
  - **Recalculation on parameter change**

### 3. Image Pipeline

Data flows from InputNode → (Processing Nodes) → OutputNode, with intermediate results stored temporarily and deleted as soon as they are passed downstream.

---

## 🧩 Nodes Included

### Input & Output Nodes

#### 📥 InputNode
- **Function**: Loads an image into the pipeline using a file dialog
- **Details**: The image is stored as a cv::Mat. Supported formats include .jpg, .png, .bmp, etc.
- **Ports**: One **Output port** that sends the image downstream for processing

#### 📤 OutputNode
- **Function**: Displays the final output image in a QLabel preview embedded in the node
- **Features**:
  - Auto-updates when upstream data changes
  - Can optionally export/save the result (planned feature)
  - **Read-only**, accepts 1 Input

### Basic Processing Nodes

#### 🔆 BrightnessNode
- **Function**: Modifies image brightness using OpenCV's convertTo() function
- **UI Controls**: 
  - Slider ranging from **-100 to +100**
- **Implementation**: 
  - Internally adjusts the beta value and alpha value for contrast and brightness respectively;

#### 🌫️ BlurNode
- **Function**: Applies a **Gaussian Blur** to smooth the image using OpenCV's GaussianBlur()
- **UI Controls**:
  - Integer spin box to control **Kernel Size** (must be an odd number, auto-adjusted if even)
  - Range: Typically **1–99**
  - Option to apply **Horizontal Blur** only (applies blur in x-direction)
  - Option to apply **Vertical Blur** only (applies blur in y-direction)
  - When both horizontal and vertical options are selected, a full 2D blur is applied
- **Implementation**:
  - Uses separate kernel sizes for x and y directions when directional blur is selected
  - For horizontal blur: ksize = (kernelSize, 1)
  - For vertical blur: ksize = (1, kernelSize)
  - For full blur: ksize = (kernelSize, kernelSize)
- **Effect**: Reduces noise, softens edges

#### ⚡ ThresholdNode
- **Function**: Converts the image to a binary black & white image using OpenCV's threshold() and adaptiveThreshold() functions
- **UI Controls**:
  - **Threshold Value**: Slider (0–255) for manual threshold
  - **Thresholding Method**: Dropdown with options:
    - **Simple**: Standard global thresholding (cv::threshold)
    - **Adaptive Gaussian**: Local adaptive thresholding using weighted sum (Gaussian) of neighborhood values
    - **Otsu**: Automatically determines optimal threshold value (works with Simple threshold type)
- **Implementation**:
  - For Simple + Otsu: Uses cv::THRESH_OTSU flag with threshold()
  - For Adaptive methods: Uses cv::adaptiveThreshold() with appropriate parameters
- **Effect**: Highlights areas of interest based on intensity; adaptive methods work better with varying lighting conditions

#### 🔀 BlendNode
- **Function**: Blends two images together using multiple blend modes and OpenCV processing
- **UI Controls**:
  - **Blend Mode**: Dropdown with options:
    - **Normal**: Standard alpha compositing
    - **Multiply**: Darkens image by multiplying pixel values
    - **Screen**: Lightens image by inverting, multiplying, and inverting again
    - **Overlay**: Combines Multiply and Screen for contrast
    - **Difference**: Shows absolute difference between images
  - **Opacity**: Slider (0–100) controlling the intensity of the blend effect
  - **Mix**: Slider (0–100) determining how much of the blend result to mix with the original image
- **Implementation**:
  - Each blend mode processed as 32-bit float for precision
  - Enhanced opacity curve (using power function) for more noticeable changes
  - Alpha channel properly composited using the formula: `result_alpha = alpha2 + alpha1 * (1 - alpha2)`
  - Handles image resizing when dimensions don't match
- **Input Ports**: Requires two images
- **Output**: Processed blend based on selected mode and parameters

### Advanced Processing Nodes

#### 🔱 SplitterNode
A specialized node that takes an input image and splits it into three separate color channels (red, green, blue), allowing independent processing of each channel.

- **Features**:
  - **Separates image into RGB components**: Extracts the red, green, and blue channels from an input image
  - **Grayscale conversion options**: Toggle grayscale effect for each individual color channel
  - **Real-time processing**: Updates all outputs immediately when input changes
  - **Visual controls**: Checkbox interface for applying grayscale to each channel

- **Ports**:
  - **Input**: 1 image input port that accepts pixmap data
  - **Output**: 3 output ports
    - Port 0: Red channel output
    - Port 1: Green channel output 
    - Port 2: Blue channel output

- **Usage**:
  1. Connect an image source to the input port
  2. The node automatically processes the image and outputs the separated channels
  3. Use the checkboxes to toggle grayscale conversion for any channel

- **Technical Details**:
  - Converting the input image to RGB format
  - Creating three separate images containing only the red, green, or blue values
  - Storing both original and processed versions of each channel
  - Applying grayscale conversion using the qGray function when enabled

#### 🎨 FilterNode
A powerful image processing node that applies convolution filters to input images using OpenCV's filter2D function.

- **Features**:
  - **Convolution Filter Application**: Apply various kernel-based filters to transform images
  - **Predefined Filter Library**: Choose from common image processing kernels
  - **Custom Kernel Support**: Define your own convolution matrices for specialized effects
  - **Fine-Tuned Control**: Adjust anchor points and border handling methods

- **UI Controls**:
  - **Filter Type Dropdown**: Select from predefined kernels (Sharpen, Edge Detection, Emboss)
  - **Custom Kernel Editor**: Matrix input interface for 3x3 or 5x5 kernels
  - **Anchor Point Selection**: Determine the center of the convolution operation

- **Implementation**:
  - Converts input image to compatible OpenCV format
  - Applies the selected or custom kernel using the filter2D operation
  - Handles border conditions according to user selection
  - Converts processed image back to QPixmap for display and output

#### 📊 NoiseNode
A procedural noise generation node that can create various noise patterns and apply them to images.

- **Features**:
  - **Multiple Noise Algorithms**: Generate different types of procedural noise patterns
  - **Customizable Parameters**: Fine-tune noise characteristics with multiple controls
  - **Dual Application Modes**: Use noise as direct output or as displacement map
  - **Real-time Preview**: See changes as you adjust parameters

- **UI Controls**:
  - **Noise Pattern Dropdown**:
    - **Perlin**: Smooth gradient noise with natural appearance
    - **Simplex**: Improved version of Perlin noise with fewer directional artifacts
    - **Worley**: Cell/Voronoi-based noise creating organic patterns
  - **Noise Use Mode**:
    - **Direct Output**: Renders the noise pattern as a grayscale or RGB image
    - **Displacement**: Uses the noise pattern to distort the input image
  - **Parameter Controls**:
    - **Octaves** (1-8): Controls noise detail/complexity by layering iterations
    - **Persistence** (1-9): Affects contrast and how quickly amplitudes diminish
    - **Scale** (1-500): Adjusts the zoom level/frequency of the noise pattern

---

## ✅ Key Features Across All Nodes

- All image data is managed using std::shared_ptr<cv::Mat> for safe and efficient memory usage
- Each node inherits from NodeDataModel and integrates with the QtNodes system for reactive updates
- All UI widgets are embedded using Qt (sliders, spin boxes, dropdowns, matrix editors)
- Nodes are fully modular — easily extendable for new operations

---

## 🔄 Data Propagation & Execution Flow

The project utilizes **lazy evaluation** combined with **reactive propagation**:

1. When a node's parameter changes (e.g., brightness slider), the node marks itself **dirty**
2. QtNodes triggers a recalculation for this node and all downstream nodes
3. compute() function is executed in each node, using OpenCV for processing
4. Processed cv::Mat is passed as std::shared_ptr<NodeData> to the next node

---

## 💾 Memory Management

To ensure efficient and safe memory usage:

### Image Data Handling

- All images (cv::Mat) are passed using std::shared_ptr, ensuring **automatic memory deallocation** when no longer in use
- Intermediate node results are stored only temporarily

### Node Output Cleanup

- When a new connection is made or parameters are changed:
  - Previous results are invalidated
  - Downstream nodes receive updated results
  - Orphaned cv::Mat data is released

### RAII

- OpenCV's internal smart pointer mechanisms ensure that images are released automatically as soon as they go out of scope

---

## 🖼️ UI Overview

- **Canvas**: Drag and connect nodes visually
- **Right-Click Menu**: Dynamically add nodes to the canvas
- **Sidebar Controls**: Adjust parameters like brightness and blur
- **Live Preview**: See real-time output in the OutputNode

---

## 🚀 Getting Started

### 📋 Prerequisites

- [Qt Creator](https://www.qt.io/download)
- [CMake](https://cmake.org/)
- [OpenCV](https://opencv.org/releases/)
- QtNodes library (included or cloned as a submodule)

### 🧪 Build Instructions

```bash
git clone https://github.com/himanshu1844/node-based-image-processor.git
cd node-based-image-processor

mkdir build && cd build
cmake ..
cmake --build .
```
