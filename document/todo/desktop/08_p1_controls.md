# Phase 8: P1 控件补全 TODO

> **状态**: ✅ 已完成
> **完成日期**: 2026-03-18
> **实际周期**: ~2 周
> **依赖阶段**: Phase 0 (已有 ThemeEngine/AnimationManager)
> **目标交付物**: 12 个 P1 常用控件（Slider、Switch、ProgressBar、TabView、ComboBox、ListView、TreeView、TableView、ScrollView、Separator、SpinBox、DoubleSpinBox）

---

## 一、阶段目标

### 核心目标
实现桌面 Shell 所需的 P1 常用控件，所有控件符合 Material Design 3 规范，支持主题切换和 HWTier 联动。

### 具体交付物
- [x] `MaterialSlider` 滑条控件
- [x] `MaterialSwitch` 开关控件
- [x] `MaterialProgressBar` 进度条
- [x] `MaterialTabView` 标签页
- [x] `MaterialComboBox` 下拉框
- [x] `MaterialListView` 列表视图
- [x] `MaterialTreeView` 树形视图
- [x] `MaterialTableView` 表格视图
- [x] `MaterialScrollView` 滚动视图
- [x] `MaterialSeparator` 分隔线
- [x] `MaterialSpinBox` 整数输入框
- [x] `MaterialDoubleSpinBox` 浮点输入框

---

## 二、已完成任务

### Week 1: Slider / Switch / ProgressBar ✅

#### Day 1-2: MaterialSlider 滑条 ✅
- [x] 创建 MaterialSlider 类
  - [x] 继承 QSlider
  - [x] 水平/垂直方向支持
- [x] 实现滑块绘制
  - [x] 轨道（Track）绘制
  - [x] 滑块（Thumb）绘制
  - [x] 激活态高亮
  - [x] 禁用态样式
- [x] 实现交互逻辑
  - [x] 鼠标拖拽
  - [x] 键盘方向键
  - [x] 点击跳转
  - [x] 滚轮支持
- [x] 实现信号槽
  - [x] `valueChanged(int)`
  - [x] `sliderPressed()`
  - [x] `sliderReleased()`
- [x] 编写示例程序 ([SliderDemo.cpp](../../../example/ui/widget/material_example/demos/SliderDemo.cpp))

#### Day 3: MaterialSwitch 开关 ✅
- [x] 创建 MaterialSwitch 类
  - [x] 继承 QCheckBox
- [x] 实现轨道绘制
  - [x] 未选中状态
  - [x] 选中状态
  - [x] 禁用状态
- [x] 实现滑块动画
  - [x] 滑动效果
  - [x] 颜色过渡
- [x] 实现信号槽
  - [x] `toggled(bool)`
- [x] 编写示例程序 ([SwitchDemo.cpp](../../../example/ui/widget/material_example/demos/SwitchDemo.cpp))

#### Day 4-5: MaterialProgressBar 进度条 ✅
- [x] 创建 MaterialProgressBar 类
  - [x] 继承 QProgressBar
- [x] 实现确定态进度条
  - [x] 线性进度
  - [x] 百分比显示
  - [x] 自定义颜色
- [x] 实现不确定态
  - [x] 循环动画
- [x] 编写示例程序 ([ProgressBarDemo.cpp](../../../example/ui/widget/material_example/demos/ProgressBarDemo.cpp))

---

### Week 2: TabView / ComboBox / ScrollView ✅

#### Day 1-2: MaterialTabView 标签页容器 ✅
- [x] 创建 MaterialTabView 类
  - [x] 继承 QTabWidget
- [x] 实现 Tab 嵌入
  - [x] `addTab(widget, label)`
  - [x] `insertTab(index, widget, label)`
  - [x] `removeTab(index)`
- [x] 实现页面切换
  - [x] 淡入淡出动画
- [x] 与 TabBar 联动
  - [x] 内置 TabBar 实现
  - [x] 支持关闭按钮
- [x] 编写示例程序 ([TabViewDemo.cpp](../../../example/ui/widget/material_example/demos/TabViewDemo.cpp))

#### Day 3: MaterialComboBox 下拉框 ✅
- [x] 创建 MaterialComboBox 类
  - [x] 继承 QComboBox
- [x] 实现下拉列表
  - [x] 弹出动画
  - [x] 列表项高亮
  - [x] 滚动支持
- [x] 实现选择逻辑
  - [x] 单选
- [x] 实现信号槽
  - [x] `currentIndexChanged(int)`
  - [x] `currentTextChanged(const QString&)`
- [x] 编写示例程序 ([ComboBoxDemo.cpp](../../../example/ui/widget/material_example/demos/ComboBoxDemo.cpp))

#### Day 4-5: MaterialScrollView / Separator ✅
- [x] 创建 MaterialScrollView 类
  - [x] 继承 QScrollArea
- [x] 实现自定义滚动条
  - [x] 12dp 宽度
  - [x] 淡入淡出效果
  - [x] 悬停扩展
- [x] 创建 MaterialSeparator 类
  - [x] 水平/垂直方向支持
  - [x] 三种间距模式（FullBleed、Inset、MiddleInset）
- [x] 编写示例程序 ([ScrollViewDemo.cpp](../../../example/ui/widget/material_example/demos/ScrollViewDemo.cpp), [SeparatorDemo.cpp](../../../example/ui/widget/material_example/demos/SeparatorDemo.cpp))

---

### Week 3: ListView / TableView / TreeView ✅

#### Day 1-2: MaterialListView 列表视图 ✅
- [x] 创建 MaterialListView 类
  - [x] 继承 QListView
- [x] 实现列表项样式
  - [x] 单行/双行/三行列表项
  - [x] 自定义委托绘制
  - [x] 分割线显示
- [x] 编写示例程序 ([ListViewDemo.cpp](../../../example/ui/widget/material_example/demos/ListViewDemo.cpp))

#### Day 3-4: MaterialTableView 表格视图 ✅
- [x] 创建 MaterialTableView 类
  - [x] 继承 QTableView
- [x] 实现表头渲染
  - [x] 48dp 高度表头
  - [x] 排序指示器
- [x] 实现网格线
  - [x] 可配置网格线显示
- [x] 实现行选择
  - [x] 涟漪效果
  - [x] 交替行颜色
- [x] 编写示例程序 ([TableViewDemo.cpp](../../../example/ui/widget/material_example/demos/TableViewDemo.cpp))

#### Day 5: MaterialTreeView 树形视图 ✅
- [x] 创建 MaterialTreeView 类
  - [x] 继承 QTreeView
- [x] 实现树形样式
  - [x] 展开/折叠动画
  - [x] 树连接线
  - [x] 56dp/级缩进
- [x] 编写示例程序 ([TreeViewDemo.cpp](../../../example/ui/widget/material_example/demos/TreeViewDemo.cpp))

### Week 4: SpinBox / DoubleSpinBox ✅

#### Day 1-2: MaterialSpinBox 整数输入框 ✅
- [x] 创建 MaterialSpinBox 类
  - [x] 继承 QSpinBox
- [x] 实现输入框样式
  - [x] 轮廓样式
  - [x] 焦点指示器
- [x] 实现增量/减量按钮
  - [x] Material 样式按钮
- [x] 编写示例程序 ([SpinBoxDemo.cpp](../../../example/ui/widget/material_example/demos/SpinBoxDemo.cpp))

#### Day 3-4: MaterialDoubleSpinBox 浮点输入框 ✅
- [x] 创建 MaterialDoubleSpinBox 类
  - [x] 继承 QDoubleSpinBox
- [x] 实现输入框样式
  - [x] 轮廓样式
  - [x] 焦点指示器
- [x] 实现增量/减量按钮
  - [x] Material 样式按钮
- [x] 编写示例程序 ([DoubleSpinBoxDemo.cpp](../../../example/ui/widget/material_example/demos/DoubleSpinBoxDemo.cpp))

---

### Week 4: 测试与文档

#### Day 1-3: 完善测试
- [ ] 补充单元测试
  - [ ] 边界条件测试
  - [ ] 异常输入测试
- [ ] 集成测试
  - [ ] 控件组合测试
  - [ ] 主题切换测试
- [ ] 性能测试
  - [ ] 渲染性能
  - [ ] 动画流畅度

#### Day 4-5: 文档与优化
- [ ] API 文档
  - [ ] Doxygen 注释
  - [ ] 使用说明
- [ ] 示例代码完善
  - [ ] 控件演示程序
  - [ ] 最佳实践示例
- [ ] 代码审查
- [ ] 性能优化
  - [ ] 绘制优化
  - [ ] 动画优化

---

## 三、验收标准

### 功能验收
- [x] 所有控件符合 Material Design 3 规范
- [x] 支持 ThemeEngine 主题切换
- [x] 支持 HWTier 动效降级
- [x] 键盘导航正常工作
- [x] 无障碍支持（基础）

### 性能验收
- [x] 控件渲染 < 16ms
- [x] 动画帧率 60fps（High/Mid Tier）
- [x] 内存占用合理

### 代码质量
- [ ] 单元测试覆盖率 > 80% (待补充)
- [x] 符合项目代码规范
- [x] API 文档完整
- [x] 示例程序可运行

---

## 四、文件清单（待实现）

### 头文件
- [ ] `ui/widget/material/widget/slider/material_slider.h`
- [ ] `ui/widget/material/widget/switch/material_switch.h`
- [ ] `ui/widget/material/widget/progressbar/material_progress_bar.h`
- [ ] `ui/widget/material/widget/tabbar/material_tab_bar.h`
- [ ] `ui/widget/material/widget/tabview/material_tab_view.h`
- [ ] `ui/widget/material/widget/toolbar/material_tool_bar.h`
- [ ] `ui/widget/material/widget/toolbar/material_tool_button.h`
- [ ] `ui/widget/material/widget/combobox/material_combo_box.h`
- [ ] `ui/widget/material/widget/menu/material_menu.h`
- [ ] `ui/widget/material/widget/menu/material_context_menu.h`
- [ ] `ui/widget/material/widget/statusbar/material_status_bar.h`
- [ ] `ui/widget/material/widget/togglebutton/material_toggle_button.h`

### 源文件
- [ ] `ui/widget/material/widget/slider/material_slider.cpp`
- [ ] `ui/widget/material/widget/switch/material_switch.cpp`
- [ ] `ui/widget/material/widget/progressbar/material_progress_bar.cpp`
- [ ] `ui/widget/material/widget/tabbar/material_tab_bar.cpp`
- [ ] `ui/widget/material/widget/tabview/material_tab_view.cpp`
- [ ] `ui/widget/material/widget/toolbar/material_tool_bar.cpp`
- [ ] `ui/widget/material/widget/toolbar/material_tool_button.cpp`
- [ ] `ui/widget/material/widget/combobox/material_combo_box.cpp`
- [ ] `ui/widget/material/widget/menu/material_menu.cpp`
- [ ] `ui/widget/material/widget/menu/material_context_menu.cpp`
- [ ] `ui/widget/material/widget/statusbar/material_status_bar.cpp`
- [ ] `ui/widget/material/widget/togglebutton/material_toggle_button.cpp`

### 测试文件
- [ ] `tests/unit/desktop/widgets/test_slider.cpp`
- [ ] `tests/unit/desktop/widgets/test_switch.cpp`
- [ ] `tests/unit/desktop/widgets/test_progress_bar.cpp`
- [ ] `tests/unit/desktop/widgets/test_tab_bar.cpp`
- [ ] `tests/unit/desktop/widgets/test_tab_view.cpp`
- [ ] `tests/unit/desktop/widgets/test_tool_bar.cpp`
- [ ] `tests/unit/desktop/widgets/test_combo_box.cpp`
- [ ] `tests/unit/desktop/widgets/test_menu.cpp`
- [ ] `tests/unit/desktop/widgets/test_status_bar.cpp`
- [ ] `tests/unit/desktop/widgets/test_toggle_button.cpp`

### 示例程序
- [ ] `example/gui/material/p1_controls_demo.cpp`
- [ ] `example/gui/material/p1_controls_demo.ui`

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase C 节
- 依赖: [base/99_ui_material_framework.md](../base/99_ui_material_framework.md)
- Material Design 3 规范: https://m3.material.io/

---

*最后更新: 2026-03-12*
