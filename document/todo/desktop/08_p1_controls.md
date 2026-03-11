# Phase 8: P1 控件补全 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 3~4 周
> **依赖阶段**: Phase 0 (已有 ThemeEngine/AnimationManager)
> **目标交付物**: 9 个 P1 常用控件（Slider、Switch、ProgressBar、TabBar/TabView、ToolBar/ToolButton、ComboBox、Menu、StatusBar、ToggleButton）

---

## 一、阶段目标

### 核心目标
实现桌面 Shell 所需的 P1 常用控件，所有控件符合 Material Design 3 规范，支持主题切换和 HWTier 联动。

### 具体交付物
- [ ] `MaterialSlider` 滑条控件
- [ ] `MaterialSwitch` 开关控件
- [ ] `MaterialProgressBar` 进度条
- [ ] `MaterialTabBar` / `MaterialTabView` 标签页
- [ ] `MaterialToolBar` / `MaterialToolButton` 工具栏
- [ ] `MaterialComboBox` 下拉框
- [ ] `MaterialMenu` / `MaterialContextMenu` 菜单
- [ ] `MaterialStatusBar` 状态栏
- [ ] `MaterialToggleButton` 切换按钮

---

## 二、待实现任务

### Week 1: Slider / Switch / ProgressBar

#### Day 1-2: MaterialSlider 滑条
- [ ] 创建 MaterialSlider 类
  - [ ] 继承 QWidget
  - [ ] 水平/垂直方向支持
- [ ] 实现滑块绘制
  - [ ] 轨道（Track）绘制
  - [ ] 滑块（Thumb）绘制
  - [ ] 激活态高亮
  - [ ] 禁用态样式
- [ ] 实现交互逻辑
  - [ ] 鼠标拖拽
  - [ ] 键盘方向键
  - [ ] 点击跳转
  - [ ] 滚轮支持
- [ ] 实现信号槽
  - [ ] `valueChanged(int)`
  - [ ] `sliderPressed()`
  - [ ] `sliderReleased()`
- [ ] 实现 HWTier 联动
  - [ ] Low Tier 简化动画
- [ ] 编写示例程序
- [ ] 单元测试

#### Day 3: MaterialSwitch 开关
- [ ] 创建 MaterialSwitch 类
  - [ ] 继承 QAbstractButton
- [ ] 实现轨道绘制
  - [ ] 未选中状态
  - [ ] 选中状态
  - [ ] 禁用状态
- [ ] 实现滑块动画
  - [ ] 滑动效果
  - [ ] 颜色过渡
- [ ] 实现信号槽
  - [ ] `toggled(bool)`
- [ ] 编写示例程序
- [ ] 单元测试

#### Day 4-5: MaterialProgressBar 进度条
- [ ] 创建 MaterialProgressBar 类
- [ ] 实现确定态进度条
  - [ ] 线性进度
  - [ ] 百分比显示（可选）
  - [ ] 自定义颜色
- [ ] 实现不确定态
  - [ ] 循环动画
- [ ] 实现缓冲区显示（可选）
  - [ ] 双层进度条
- [ ] 实现信号槽
  - [ ] `valueChanged(int)`
  - [ ] `finished()`
- [ ] 编写示例程序
- [ ] 单元测试

---

### Week 2: TabBar / TabView / ToolBar

#### Day 1-2: MaterialTabBar 标签栏
- [ ] 创建 MaterialTabBar 类
- [ ] 实现 Tab 项
  - [ ] 图标+文字
  - [ ] 仅图标
  - [ ] 仅文字
- [ ] 实现交互
  - [ ] 点击切换
  - [ ] 滑动手势
  - [ ] 长按拖拽（可选）
- [ ] 实现指示器
  - [ ] 下划线动画
  - [ ] 背景高亮
- [ ] 实现信号槽
  - [ ] `currentChanged(int)`
  - [ ] `tabCloseRequested(int)`
- [ ] 编写示例程序

#### Day 3: MaterialTabView 标签页容器
- [ ] 创建 MaterialTabView 类
- [ ] 实现 Tab 嵌入
  - [ ] `addTab(widget, label)`
  - [ ] `insertTab(index, widget, label)`
  - [ ] `removeTab(index)`
- [ ] 实现页面切换
  - [ ] 淡入淡出动画
- [ ] 与 TabBar 联动
  - [ ] 可选内置 TabBar
  - [ ] 支持外部 TabBar
- [ ] 编写示例程序
- [ ] 单元测试

#### Day 4-5: MaterialToolBar / MaterialToolButton
- [ ] 创建 MaterialToolBar 类
  - [ ] 继承 QWidget
- [ ] 实现 ToolButton
  - [ ] 图标按钮
  - [ ] 文字按钮
  - [ ] 图标+文字
  - [ ] 下拉箭头样式
- [ ] 实现按钮分组
  - [ ] 分组容器
  - [ ] 分组样式
- [ ] 实现分隔符
  - [ ] 垂直分隔符
  - [ ] 水平分隔符
- [ ] 实现可拖拽（可选）
  - [ ] 工具栏拖拽
  - [ ] 按钮重排
- [ ] 编写示例程序
- [ ] 单元测试

---

### Week 3: ComboBox / Menu / StatusBar

#### Day 1-2: MaterialComboBox 下拉框
- [ ] 创建 MaterialComboBox 类
  - [ ] 继承 QWidget
- [ ] 实现下拉列表
  - [ ] 弹出动画
  - [ ] 列表项高亮
  - [ ] 滚动支持
- [ ] 实现选择逻辑
  - [ ] 单选
  - [ ] 可搜索（可选）
  - [ ] 可编辑（可选）
- [ ] 实现信号槽
  - [ ] `currentIndexChanged(int)`
  - [ ] `currentTextChanged(const QString&)`
- [ ] 编写示例程序
- [ ] 单元测试

#### Day 3: MaterialMenu / MaterialContextMenu
- [ ] 创建 MaterialMenu 类
- [ ] 创建 MaterialContextMenu 类
- [ ] 实现菜单项
  - [ ] 图标+文字
  - [ ] 快捷键显示
  - [ ] 子菜单
  - [ ] 分隔符
  - [ ] 复选标记
- [ ] 实现菜单操作
  - [ ] 触发动作
  - [ ] 禁用项
  - [ ] 隐藏项
- [ ] 实现 ContextMenu
  - [ ] 右键弹出
  - [ ] 位置自适应
- [ ] 编写示例程序
- [ ] 单元测试

#### Day 4: MaterialStatusBar 状态栏
- [ ] 创建 MaterialStatusBar 类
  - [ ] 继承 QWidget
- [ ] 实现内容区域
  - [ ] 添加小部件
  - [ ] 添加永久小部件
- [ ] 实现尺寸手柄（可选）
  - [ ] 右下角拖拽调整大小
- [ ] 实现消息显示
  - [ ] `showMessage(const QString&, int timeout)`
- [ ] 编写示例程序
- [ ] 单元测试

#### Day 5: MaterialToggleButton
- [ ] 创建 MaterialToggleButton 类
  - [ ] 继承 QAbstractButton
- [ ] 实现图标切换
  - [ ] 选中态图标
  - [ ] 未选中态图标
- [ ] 实现文字切换（可选）
  - [ ] ON/OFF 文字
- [ ] 实现样式
  - [ ] 填充样式
  - [ ] 描边样式
- [ ] 编写示例程序
- [ ] 单元测试

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
- [ ] 所有控件符合 Material Design 3 规范
- [ ] 支持 ThemeEngine 主题切换
- [ ] 支持 HWTier 动效降级
- [ ] 键盘导航正常工作
- [ ] 无障碍支持（基础）

### 性能验收
- [ ] 控件渲染 < 16ms
- [ ] 动画帧率 60fps（High/Mid Tier）
- [ ] 内存占用合理

### 代码质量
- [ ] 单元测试覆盖率 > 80%
- [ ] 符合项目代码规范
- [ ] API 文档完整
- [ ] 示例程序可运行

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
