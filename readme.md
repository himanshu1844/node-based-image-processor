# Node-Based Image Processor

A visual, modular image processing system built using **Qt**, **OpenCV**, and **QtNodes**. This project lets users create and connect processing nodes in a graph-based editor interface — inspired by tools like Unreal Engine Blueprints and Blender's Shader Node Editor.

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

This application is architected around the **Node Graph Execution Model** using the `QtNodes` framework. It consists of the following core layers:

### 1. Node Definition Layer

Each processing unit in the pipeline is implemented as a subclass of `QtNodes::NodeDataModel`, and defines:

- **Ports**: Using `nPorts()`, `dataType()`, and `portCaption()` to describe inputs and outputs.
- **Embedded QWidget**: A UI widget (like sliders or spin boxes) embedded within the node for user interaction.
- **compute()** method: Executes the processing logic using OpenCV based on the input data and parameters.
- **DataModel Lifecycle**:
  - `setInData()`: Called when input data arrives; stores data and triggers computation.
  - `outData()`: Called when output data is requested; returns processed result.
  - `resend()` and `dataUpdated()` ensure downstream nodes are notified of changes.

### 2. Graph Execution Layer (QtNodes Engine)

This layer handles the construction, visualization, and propagation of the node graph. QtNodes provides:

- **Scene & Flow View**:
  - `QtNodes::FlowScene` manages the logic and data of the node graph.
  - `QtNodes::FlowView` is the actual canvas UI where nodes are placed and linked.
- **Connections**: Wires between ports that transfer `NodeData`.
- **Recalculation Mechanism**:
  - When data or a parameter changes, `dataUpdated()` is called.
  - The system propagates changes recursively to all connected downstream nodes.

### 3. Image Pipeline

- **Data Flow**: 
  - Images are introduced via `InputNode`, processed through chained nodes, and rendered in `OutputNode`.
- **Lazy Evaluation**:
  - Nodes are recalculated **only when needed**, reducing redundant computation.
  - Changes in one node only re-trigger calculations for affected downstream paths.

---

## 🧩 Nodes Included

| Node             | Description |
|------------------|-------------|
| **InputNode**    | Loads an image into the pipeline. Accepts file input. |
| **BrightnessNode** | Adjusts image brightness. Controlled by a slider (-100 to +100). |
| **BlurNode**     | Applies a Gaussian blur to the image. User sets kernel size. |
| **ThresholdNode**| Applies a binary threshold to the image. |
| **OutputNode**   | Displays the final processed image. Acts as the graph's sink. |

Each node is visually represented and includes real-time parameter adjustment.

---

## 🔄 Data Propagation & Execution Flow

The project utilizes **lazy evaluation** combined with **reactive propagation**:

1. When a node’s parameter changes (e.g., brightness slider), the node marks itself **dirty**.
2. QtNodes triggers a recalculation for this node and all downstream nodes.
3. `compute()` function is executed in each node, using OpenCV for processing.
4. Processed `cv::Mat` is passed as `std::shared_ptr<NodeData>` to the next node.

> This structure ensures only affected parts of the graph are recomputed — saving CPU time and memory.

---

## 💾 Memory Management

To ensure efficient and safe memory usage:

### ✅ Image Data Handling
- All images (`cv::Mat`) are passed using `std::shared_ptr`, ensuring **automatic memory deallocation** when no longer in use.
- Intermediate node results are stored only temporarily.

### ✅ Node Output Cleanup
- When a new connection is made or parameters are changed:
  - Previous results are invalidated.
  - Downstream nodes receive updated results.
  - Orphaned `cv::Mat` data is released.

### ✅ RAII
- OpenCV's internal smart pointer mechanisms ensure that images are released automatically as soon as they go out of scope.

---

## 🖼️ UI Overview

- **Canvas**: Drag and connect nodes visually.
- **Right-Click Menu**: Dynamically add nodes to the canvas.
- **Sidebar Controls**: Adjust parameters like brightness and blur.
- **Live Preview**: See real-time output in the `OutputNode`.

---

## 🚀 Getting Started

### 📋 Prerequisites

- [Qt Creator](https://www.qt.io/download)
- [CMake](https://cmake.org/)
- [OpenCV](https://opencv.org/releases/)
- `QtNodes` library (included or cloned as a submodule)

### 🧪 Build Instructions

```bash
git clone https://github.com/himanshu1844/node-based-image-processor.git
cd node-based-image-processor

mkdir build && cd build
cmake ..
cmake --build .
