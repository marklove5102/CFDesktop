# Phase G: Widget 应用与示例程序状态

> **状态**: ✅ 已完成
> **完成日期**: 2026-03-21
> **总体进度**: 100%

---

## 一、应用框架

### 1.1 Application 基础框架

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`ui/widget/application_support/application.h`](../../../ui/widget/application_support/application.h) | 基础应用框架 |
| [`ui/widget/application_support/application.cpp`](../../../ui/widget/application_support/application.cpp) | 实现文件 |

#### 功能实现

- [x] 继承自 QApplication
- [x] 主题管理集成
- [x] 动画管理器集成
- [x] Token-based API 支持
- [x] 全局访问接口

---

### 1.2 MaterialApplication 框架

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`ui/widget/material/application/material_application.h`](../../../ui/widget/material/application/material_application.h) | Material 应用框架 |
| [`ui/widget/material/application/material_application.cpp`](../../../ui/widget/material/application/material_application.cpp) | 实现文件 |

#### 功能实现

- [x] 预配置 Material Design 3 主题
- [x] 自动注册亮色和暗色主题
- [x] 完整的 Material Design 组件支持

---

## 二、Material Widget Gallery

### 2.1 主程序

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`example/ui/widget/material_example/main.cpp`](../../../example/ui/widget/material_example/main.cpp) | 主入口 |
| [`example/ui/widget/material_example/MainWindow.h`](../../../example/ui/widget/material_example/MainWindow.h) | 主窗口 |
| [`example/ui/widget/material_example/MainWindow.cpp`](../../../example/ui/widget/material_example/MainWindow.cpp) | 主窗口实现 |

#### 功能特性

- [x] 画廊形式展示 (左侧导航，右侧展示)
- [x] 实时预览和交互
- [x] 每个控件独立演示页面
- [x] 统一的 Material Design 风格

---

### 2.2 已实现的 Demo 程序 (19个)

| # | Demo 名称 | 头文件 | 实现文件 |
|---|----------|--------|----------|
| 1 | ButtonDemo | [`demos/ButtonDemo.h`](../../../example/ui/widget/material_example/demos/ButtonDemo.h) | [`demos/ButtonDemo.cpp`](../../../example/ui/widget/material_example/demos/ButtonDemo.cpp) |
| 2 | CheckBoxDemo | [`demos/CheckBoxDemo.h`](../../../example/ui/widget/material_example/demos/CheckBoxDemo.h) | [`demos/CheckBoxDemo.cpp`](../../../example/ui/widget/material_example/demos/CheckBoxDemo.cpp) |
| 3 | ComboBoxDemo | [`demos/ComboBoxDemo.h`](../../../example/ui/widget/material_example/demos/ComboBoxDemo.h) | [`demos/ComboBoxDemo.cpp`](../../../example/ui/widget/material_example/demos/ComboBoxDemo.cpp) |
| 4 | DoubleSpinBoxDemo | [`demos/DoubleSpinBoxDemo.h`](../../../example/ui/widget/material_example/demos/DoubleSpinBoxDemo.h) | [`demos/DoubleSpinBoxDemo.cpp`](../../../example/ui/widget/material_example/demos/DoubleSpinBoxDemo.cpp) |
| 5 | GroupBoxDemo | [`demos/GroupBoxDemo.h`](../../../example/ui/widget/material_example/demos/GroupBoxDemo.h) | [`demos/GroupBoxDemo.cpp`](../../../example/ui/widget/material_example/demos/GroupBoxDemo.cpp) |
| 6 | LabelDemo | [`demos/LabelDemo.h`](../../../example/ui/widget/material_example/demos/LabelDemo.h) | [`demos/LabelDemo.cpp`](../../../example/ui/widget/material_example/demos/LabelDemo.cpp) |
| 7 | ListViewDemo | [`demos/ListViewDemo.h`](../../../example/ui/widget/material_example/demos/ListViewDemo.h) | [`demos/ListViewDemo.cpp`](../../../example/ui/widget/material_example/demos/ListViewDemo.cpp) |
| 8 | ProgressBarDemo | [`demos/ProgressBarDemo.h`](../../../example/ui/widget/material_example/demos/ProgressBarDemo.h) | [`demos/ProgressBarDemo.cpp`](../../../example/ui/widget/material_example/demos/ProgressBarDemo.cpp) |
| 9 | RadioButtonDemo | [`demos/RadioButtonDemo.h`](../../../example/ui/widget/material_example/demos/RadioButtonDemo.h) | [`demos/RadioButtonDemo.cpp`](../../../example/ui/widget/material_example/demos/RadioButtonDemo.cpp) |
| 10 | ScrollViewDemo | [`demos/ScrollViewDemo.h`](../../../example/ui/widget/material_example/demos/ScrollViewDemo.h) | [`demos/ScrollViewDemo.cpp`](../../../example/ui/widget/material_example/demos/ScrollViewDemo.cpp) |
| 11 | SeparatorDemo | [`demos/SeparatorDemo.h`](../../../example/ui/widget/material_example/demos/SeparatorDemo.h) | [`demos/SeparatorDemo.cpp`](../../../example/ui/widget/material_example/demos/SeparatorDemo.cpp) |
| 12 | SliderDemo | [`demos/SliderDemo.h`](../../../example/ui/widget/material_example/demos/SliderDemo.h) | [`demos/SliderDemo.cpp`](../../../example/ui/widget/material_example/demos/SliderDemo.cpp) |
| 13 | SpinBoxDemo | [`demos/SpinBoxDemo.h`](../../../example/ui/widget/material_example/demos/SpinBoxDemo.h) | [`demos/SpinBoxDemo.cpp`](../../../example/ui/widget/material_example/demos/SpinBoxDemo.cpp) |
| 14 | SwitchDemo | [`demos/SwitchDemo.h`](../../../example/ui/widget/material_example/demos/SwitchDemo.h) | [`demos/SwitchDemo.cpp`](../../../example/ui/widget/material_example/demos/SwitchDemo.cpp) |
| 15 | TabViewDemo | [`demos/TabViewDemo.h`](../../../example/ui/widget/material_example/demos/TabViewDemo.h) | [`demos/TabViewDemo.cpp`](../../../example/ui/widget/material_example/demos/TabViewDemo.cpp) |
| 16 | TableViewDemo | [`demos/TableViewDemo.h`](../../../example/ui/widget/material_example/demos/TableViewDemo.h) | [`demos/TableViewDemo.cpp`](../../../example/ui/widget/material_example/demos/TableViewDemo.cpp) |
| 17 | TextAreaDemo | [`demos/TextAreaDemo.h`](../../../example/ui/widget/material_example/demos/TextAreaDemo.h) | [`demos/TextAreaDemo.cpp`](../../../example/ui/widget/material_example/demos/TextAreaDemo.cpp) |
| 18 | TextFieldDemo | [`demos/TextFieldDemo.h`](../../../example/ui/widget/material_example/demos/TextFieldDemo.h) | [`demos/TextFieldDemo.cpp`](../../../example/ui/widget/material_example/demos/TextFieldDemo.cpp) |
| 19 | TreeViewDemo | [`demos/TreeViewDemo.h`](../../../example/ui/widget/material_example/demos/TreeViewDemo.h) | [`demos/TreeViewDemo.cpp`](../../../example/ui/widget/material_example/demos/TreeViewDemo.cpp) |

---

## 三、已实现的 Material Design 组件

### P0 核心控件 (7/7) ✅

1. **Button** - 按钮 (5种变体: Filled, Tonal, Outlined, Text, Elevated)
2. **TextField** - 单行文本输入
3. **TextArea** - 多行文本编辑器
4. **Label** - 显示文本或图像
5. **CheckBox** - 二态选择控件
6. **RadioButton** - 互斥选择控件
7. **GroupBox** - 容器/分组框

### P1 控件 (12/12) ✅

1. **ComboBox** - 下拉列表
2. **Slider** - 滑块
3. **ProgressBar** - 进度条
4. **Switch** - 开关
5. **ListView** - 列表视图
6. **TreeView** - 树视图
7. **TableView** - 表格视图
8. **TabView** - 标签页
9. **ScrollView** - 滚动区域
10. **Separator** - 分隔线
11. **SpinBox** - 整数微调框
12. **DoubleSpinBox** - 浮点微调框

---

## 四、其他示例程序

### GUI 相关示例

- Material Color Scheme Gallery
- Material Typography Gallery
- Material Motion Spec Gallery
- Material Radius Scale Gallery
- Theme Gallery
- Toast 演示

### 系统示例

- CPU 信息示例
- 内存信息示例
- GPU 信息示例
- 网络信息示例

---

## 五、相关文档

- 原始 TODO: [`../desktop/13_widget_apps.md`](../desktop/13_widget_apps.md)
- UI 框架状态: [`../done/99_ui_layer1-4_and_p0_widgets_done.md`](../done/99_ui_layer1-4_and_p0_widgets_done.md)
- P1 控件状态: [`../done/08_p1_controls_status.md`](../done/08_p1_controls_status.md)

---

*最后更新: 2026-03-21*
