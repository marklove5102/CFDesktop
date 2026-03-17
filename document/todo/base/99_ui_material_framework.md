# UI Material Framework TODO

> **状态**: 🚧 进行中
> **预计周期**: 持续迭代
> **依赖阶段**: Phase 0-3
> **参考文档**: [../../ui/MaterialRules.md](../../ui/MaterialRules.md)
> **完成归档**: [done/99_ui_layer1-4_and_p0_widgets_done.md](done/99_ui_layer1-4_and_p0_widgets_done.md)

**完成进度**:
- ✅ Layer 1: Core Math & Utility (100%)
- ✅ Layer 2: Theme Engine (100%)
- ✅ Layer 3: Animation Engine (100% - 实际使用部分)
- ✅ Layer 4: Material Behavior (100%)
- ✅ P0核心控件 (7/7)
- ⬜ P1控件 (0/12)
- ⬜ P2控件 (0/27)
- ⬜ P3控件 (0/25)

---

## 一、架构概览

### 分层架构
```
Layer 6: Qt Native Widgets (QPushButton, QLineEdit, ...)
Layer 5: Material Widget Adapter (Button, TextField, ...)
Layer 4: Material Behavior Layer (StateMachine, Ripple, ...)
Layer 3: Animation Engine Layer (TimingAnimation, SpringAnimation, ...)
Layer 2: Theme Engine Layer (ThemeManager, ICFColorScheme, ...)
Layer 1: Core Math & Utility Layer (math_helper, color, geometry, ...)
```

### 核心约束（RULE-01 至 RULE-09）
- [ ] RULE-01: 所有 Material 控件必须继承 Qt 原生控件
- [ ] RULE-02: 不得移除、覆盖、修改任何 Qt 原生 public 信号
- [ ] RULE-03: paintEvent 必须完全接管绘制
- [ ] RULE-04: 禁止使用 QSS 实现任何 Material 视觉
- [ ] RULE-05: 禁止依赖 Qt Quick / QML
- [ ] RULE-06: 动画系统必须可整体关闭
- [ ] RULE-07: 主题数据只读访问
- [ ] RULE-08: 渲染组件通过组合注入
- [ ] RULE-09: 通过 Token 访问主题

---

## 二、Layer 1: Core Math & Utility Layer

### 已完成 ✅
- [x] `ui/base/math_helper.h/cpp` - 插值、钳位、曲线求值
- [x] `ui/base/color.h/cpp` - 颜色空间转换、HCT 支持
- [x] `ui/base/color_helper.h/cpp` - 颜色混合、对比度
- [x] `ui/base/easing.h/cpp` - Material 缓动曲线
- [x] `ui/base/geometry_helper.h` - 圆角形状生成
- [x] `ui/base/device_pixel.h` - DPI 单位转换

---

## 三、Layer 2: Theme Engine Layer ✅ 已完成

> 完成归档: [done/99_ui_layer1-4_and_p0_widgets_done.md](done/99_ui_layer1-4_and_p0_widgets_done.md)

### Token 系统 ✅
- [x] Reference Token → System Token → Component Token 三层结构
- [x] 实现 `ICFColorScheme` 接口
- [x] 实现 `MaterialColorScheme`
  - [x] 文件: `ui/core/color_scheme.h` (接口)
  - [x] 文件: `ui/core/material/cfmaterial_scheme.h/cpp` (实现)

### 字体系统 ✅
- [x] 实现 `IFontType` 接口
  - [x] 文件: `ui/core/font_type.h`
- [x] 实现 `MaterialTypography`
  - [x] 文件: `ui/core/material/cfmaterial_fonttype.h/cpp`
  - [x] Material Type Scale 支持

### 形状系统 ✅
- [x] 实现 `Shape` 类
  - [x] 全局 Shape Scale 配置
  - [x] 控件圆角查询

### 动画规格 ✅
- [x] 实现 `MotionSpec` 类
  - [x] 动画时长配置
  - [x] 缓动曲线配置
  - [x] 与控件无关的可复用规格

### 主题聚合 ✅
- [x] 实现 `ICFTheme` 接口
  - [x] 文件: `ui/core/theme.h`
- [x] 实现 `ThemeManager`
  - [x] 文件: `ui/core/theme_manager.h/cpp`
  - [x] 管理当前主题实例
  - [x] 响应切换请求
  - [x] 广播变更信号
- [x] 实现 `ThemeFactory`
  - [x] 文件: `ui/core/material/material_factory.h/cpp`

### Token 访问 ✅
- [x] 实现 Token 字符串定义
  - [x] 文件: `ui/core/token/` 目录 (5个子目录)
  - [x] 颜色 token literals
  - [x] 字体 token literals
  - [x] 形状 token literals
  - [x] 运动 token literals

---

## 四、Layer 3: Animation Engine Layer ✅ 已完成

> 完成归档: [done/99_ui_layer1-4_and_p0_widgets_done.md](done/99_ui_layer1-4_and_p0_widgets_done.md)

### 动画接口 ✅
- [x] `ICFAbstractAnimation` - 抽象动画基类
  - [x] 文件: `ui/components/animation.h/cpp`
  - [x] 状态管理 (Idle/Running/Paused/Finished)
  - [x] 方向控制 (Forward/Backward)
  - [x] 生命周期方法 (start/pause/stop/reverse/tick)

### 时间动画 ✅
- [x] `ICFTimingAnimation` - 基于时间的动画
  - [x] 文件: `ui/components/timing_animation.h/cpp`
  - [x] 使用 MotionSpec 获取时长和缓动
  - [x] 支持值范围 `setRange(from, to)`

### 弹簧动画 ⬜ 预留接口
- [x] `ICFSpringAnimation` - 基于弹簧物理的动画
  - [x] 文件: `ui/components/spring_animation.h` (接口定义)
  - [ ] tick() 实现未完成 (预留接口，未被使用)

### 动画组 ⬜ 预留接口
- [x] `ICFAnimationGroup` - 动画组容器
  - [x] 文件: `ui/components/animation_group.h` (完全内联)

### 工厂管理 ✅
- [x] `ICFAnimationManagerFactory` - 动画工厂管理器接口
  - [x] 文件: `ui/components/animation_factory_manager.h/cpp`
  - [x] Token-based 动画注册与检索
  - [x] 类型安全创建
  - [x] 全局/单个动画开关控制
  - [x] 目标 FPS 设置

### Material 动画工厂 ✅
- [x] `CFMaterialAnimationFactory` - Material Design 3 动画工厂
  - [x] 文件: `ui/components/material/cfmaterial_animation_factory.h/cpp`
  - [x] 实现 `ICFAnimationManagerFactory`
  - [x] Token 映射到具体动画实例
  - [x] 策略模式支持控件级定制

### 动画策略 ✅
- [x] `AnimationStrategy` - 动画策略接口
  - [x] 文件: `ui/components/material/cfmaterial_animation_strategy.h/cpp`
  - [x] 允许控件类型定制动画参数
  - [x] 支持条件性开关逻辑（如无障碍设置）

### 动画 Token ✅
- [x] `AnimationToken` - Material Design 3 动画 token
  - [x] 文件: `ui/components/material/token/animation_token_literals.h`
  - [x] 文件: `ui/components/material/token/animation_token_mapping.h`
  - [x] fadeIn/fadeOut/slideUp/slideDown
  - [x] scaleUp/scaleDown

### Material 动画实现 ✅
- [x] Fade Animation - `cfmaterial_fade_animation.h/cpp`
- [x] Scale Animation - `cfmaterial_scale_animation.h/cpp`
- [x] Slide Animation - `cfmaterial_slide_animation.h/cpp`
- [x] Property Animation - `cfmaterial_property_animation.h/cpp`

---

## 五、Layer 4: Material Behavior Layer ✅ 已完成

> 完成归档: [done/99_ui_layer1-4_and_p0_widgets_done.md](done/99_ui_layer1-4_and_p0_widgets_done.md)

### 目录结构
```
ui/widget/material/base/
├── state_machine.h/cpp           ✅ [状态机，驱动 StateLayer opacity]
├── painter_layer.h/cpp           ✅ [基础颜色叠加层]
├── ripple_helper.h/cpp           ✅ [涟漪效果控制器]
├── elevation_controller.h/cpp    ✅ [海拔阴影控制器]
└── focus_ring.h/cpp              ✅ [焦点环指示器]
```

### StateMachine ✅
- [x] `state_machine.h/cpp`
  - [x] 管理控件视觉状态
  - [x] 驱动 StateLayer 的 opacity 动画
  - [x] 状态透明度值 (Material Design 3 规范)
    - [x] Normal: 0.00
    - [x] Hovered: 0.08
    - [x] Pressed: 0.12
    - [x] Focused: 0.12
    - [x] Dragged: 0.16
    - [x] Checked: 0.08
    - [x] Disabled: 0.00
  - [x] 优先级顺序: Disabled > Pressed > Dragged > Focused > Hovered > Normal

### PainterLayer ✅
- [x] `painter_layer.h/cpp`
  - [x] 提供基础颜色叠加层绘制功能
  - [x] 用于 StateLayer 等需要颜色叠加的场景

### RippleHelper ✅
- [x] `ripple_helper.h/cpp`
  - [x] 管理涟漪效果的生命周期与绘制
  - [x] Ripple 生命周期
    - [x] `onPress(pos)` → 创建并开始扩散
    - [x] `onRelease()` → 触发 fade-out
    - [x] 动画结束 → 清除状态
  - [x] 多 Ripple 支持（快速多次点击）

### MdElevationController ✅
- [x] `elevation_controller.h/cpp`
  - [x] 管理控件的 Material 海拔值
  - [x] 绘制对应阴影
  - [x] Material Elevation Levels
    - [x] Level 0 (0dp): 无阴影
    - [x] Level 1 (1dp): blur=2px, offset=1px, opacity=0.15
    - [x] Level 2 (3dp): blur=4px, offset=2px, opacity=0.20
    - [x] Level 3 (6dp): blur=8px, offset=4px, opacity=0.25
    - [x] Level 4 (8dp): blur=12px, offset=6px, opacity=0.30
    - [x] Level 5 (12dp): blur=16px, offset=8px, opacity=0.35
  - [x] Dark Theme 海拔表示
    - [x] 向 Surface 叠加 Primary 色调
    - [x] 使用 `tonalOverlay()` 方法

### MdFocusIndicator ✅
- [x] `focus_ring.h/cpp`
  - [x] 绘制符合 Material 规范的焦点环
  - [x] Material 规范
    - [x] 环宽度: 3dp
    - [x] 内边距: 3dp
    - [x] 动画: 淡入淡出效果

---

## 六、Layer 5: Material Widget Adapter Layer

### 目录结构
```
ui/widget/material/widget/
├── button/                       [按钮控件]
├── checkbox/                     [复选框]
├── textfield/                    [文本输入框]
└── ...
```

### 已完成 ✅ (P0 核心控件)
- [x] Button - 最基础的交互控件
- [x] TextField - 单行文本输入
- [x] TextArea - 多行文本编辑器
- [x] Label - 显示文本或图像
- [x] CheckBox - 二态选择控件
- [x] RadioButton - 互斥选择控件
- [x] GroupBox - 容器/分组框

### P1 - 常用交互控件 (High Priority)
- [ ] **ComboBox** - 下拉列表
  - [ ] 文件: `ui/widget/material/widget/combobox/`
  - [ ] 基类: QComboBox
  - [ ] 从预定义选项中选择
  - [ ] 支持搜索和可编辑模式
- [ ] **Slider** - 滑块
  - [ ] 文件: `ui/widget/material/widget/slider/`
  - [ ] 基类: QSlider
  - [ ] 在范围内选择数值
  - [ ] 支持水平和垂直方向
- [ ] **ProgressBar** - 进度条
  - [ ] 文件: `ui/widget/material/widget/progress_bar/`
  - [ ] 基类: QProgressBar
  - [ ] 显示任务完成进度
  - [ ] 支持确定和不确定状态
- [ ] **Switch** - 开关
  - [ ] 文件: `ui/widget/material/widget/switch/`
  - [ ] 基类: QCheckBox（自定义）
  - [ ] Material 风格开关控件
  - [ ] 带动画效果
- [ ] **ListView** - 列表视图
  - [ ] 文件: `ui/widget/material/widget/list_view/`
  - [ ] 基类: QListView
  - [ ] 显示一列项目
  - [ ] 支持单选/多选
- [ ] **TreeView** - 树视图
  - [ ] 文件: `ui/widget/material/widget/tree_view/`
  - [ ] 基类: QTreeView
  - [ ] 层级数据显示
  - [ ] 支持展开/折叠
- [ ] **TableView** - 表格视图
  - [ ] 文件: `ui/widget/material/widget/table_view/`
  - [ ] 基类: QTableView
  - [ ] 二维数据表格
  - [ ] 支持排序、列宽调整
- [ ] **TabView** - 标签页
  - [ ] 文件: `ui/widget/material/widget/tab_view/`
  - [ ] 基类: QTabWidget
  - [ ] 选项卡式界面
  - [ ] 支持滚动和拖拽重排
- [ ] **ScrollView** - 滚动区域
  - [ ] 文件: `ui/widget/material/widget/scroll_view/`
  - [ ] 基类: QScrollArea
  - [ ] 可滚动的内容容器
- [ ] **Separator** - 分隔线
  - [ ] 文件: `ui/widget/material/widget/separator/`
  - [ ] 基类: QFrame
  - [ ] 水平或垂直分隔线
- [ ] **SpinBox** - 微调框
  - [ ] 文件: `ui/widget/material/widget/spin_box/`
  - [ ] 基类: QSpinBox
  - [ ] 数值输入，带增减按钮
- [ ] **DoubleSpinBox** - 双精度微调框
  - [ ] 文件: `ui/widget/material/widget/double_spin_box/`
  - [ ] 基类: QDoubleSpinBox
  - [ ] 浮点数数值输入

### P2 - 高级功能控件 (Medium Priority)
- [ ] **DatePicker** - 日期选择器
  - [ ] 文件: `ui/widget/material/widget/date_picker/`
  - [ ] 基类: QCalendarWidget（控件化）
  - [ ] 日期选择，支持日历弹出
- [ ] **TimePicker** - 时间选择器
  - [ ] 文件: `ui/widget/material/widget/time_picker/`
  - [ ] 基类: QTimeEdit
  - [ ] 时间选择，支持时钟盘
- [ ] **ColorPicker** - 颜色选择器
  - [ ] 文件: `ui/widget/material/widget/color_picker/`
  - [ ] 基类: QColorDialog（控件化）
  - [ ] 颜色选择，支持色盘、吸管
- [ ] **MenuBar** - 菜单栏
  - [ ] 文件: `ui/widget/material/widget/menu_bar/`
  - [ ] 基类: QMenuBar
  - [ ] 顶部菜单栏，支持下拉菜单
- [ ] **ContextMenu** - 上下文菜单
  - [ ] 文件: `ui/widget/material/widget/context_menu/`
  - [ ] 基类: QMenu
  - [ ] 右键菜单，支持动态生成
- [ ] **ToolBar** - 工具栏
  - [ ] 文件: `ui/widget/material/widget/tool_bar/`
  - [ ] 基类: QToolBar
  - [ ] 快速访问常用操作
- [ ] **StatusBar** - 状态栏
  - [ ] 文件: `ui/widget/material/widget/status_bar/`
  - [ ] 基类: QStatusBar
  - [ ] 显示状态信息和进度
- [ ] **Dialog** - 对话框基类
  - [ ] 文件: `ui/widget/material/widget/dialog/`
  - [ ] 基类: QDialog
  - [ ] 模态/非模态对话框
- [ ] **Snackbar** - 消息提示
  - [ ] 文件: `ui/widget/material/widget/snackbar/`
  - [ ] 基类: QWidget（自定义）
  - [ ] Material 风格轻量级提示
- [ ] **Card** - 卡片
  - [ ] 文件: `ui/widget/material/widget/card/`
  - [ ] 基类: QFrame（自定义）
  - [ ] 带圆角、阴影的内容容器
- [ ] **FloatingActionButton** - 悬浮操作按钮
  - [ ] 文件: `ui/widget/material/widget/fab/`
  - [ ] 基类: QPushButton（自定义）
  - [ ] 圆形悬浮按钮
- [ ] **BottomNavigation** - 底部导航栏
  - [ ] 文件: `ui/widget/material/widget/bottom_navigation/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 底部导航标签
- [ ] **Drawer** - 侧边抽屉
  - [ ] 文件: `ui/widget/material/widget/drawer/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 从屏幕边缘滑出
- [ ] **SearchBar** - 搜索框
  - [ ] 文件: `ui/widget/material/widget/search_bar/`
  - [ ] 基类: QLineEdit（自定义）
  - [ ] 带搜索图标和清除按钮
- [ ] **Dial** - 拨盘
  - [ ] 文件: `ui/widget/material/widget/dial/`
  - [ ] 基类: QDial
  - [ ] 旋转式数值选择
- [ ] **Stepper** - 步进器
  - [ ] 文件: `ui/widget/material/widget/stepper/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 步骤指示器
- [ ] **Chip** - 芯片/标签
  - [ ] 文件: `ui/widget/material/widget/chip/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 小型交互元素
- [ ] **Badge** - 徽章
  - [ ] 文件: `ui/widget/material/widget/badge/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 数字或状态指示器
- [ ] **Tooltip** - 工具提示
  - [ ] 文件: `ui/widget/material/widget/tooltip/`
  - [ ] 基类: QToolTip
  - [ ] 鼠标悬停显示提示
- [ ] **Popover** - 气泡弹出框
  - [ ] 文件: `ui/widget/material/widget/popover/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 锚定到其他控件的弹出内容
- [ ] **CircularProgressBar** - 环形进度条
  - [ ] 文件: `ui/widget/material/widget/circular_progress_bar/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 圆形进度指示器

### P3 - 专业/高级控件 (Low Priority)
- [ ] **SplitView** - 分割窗口
- [ ] **ChartView** - 图表视图
- [ ] **DrawingArea** - 绘图区域
- [ ] **Canvas** - 画布
- [ ] **FileList** - 文件列表
- [ ] **FileTree** - 文件树
- [ ] **CodeEditor** - 代码编辑器
- [ ] **Calendar** - 日历
- [ ] **Wizard** - 向导对话框
- [ ] **PropertyEditor** - 属性编辑器
- [ ] **TreeTable** - 树形表格
- [ ] **Gauge** - 仪表盘
- [ ] **Timeline** - 时间轴
- [ ] **GridView** - 网格视图
- [ ] **SwipeableItem** - 滑动删除
- [ ] **BottomSheet** - 底部表单
- [ ] **NotificationList** - 通知列表
- [ ] **Breadcrumbs** - 面包屑导航
- [ ] **Pagination** - 分页控件
- [ ] **RatingBar** - 评分控件
- [ ] **RangeSlider** - 范围滑块
- [ ] **SliderCaptcha** - 滑动验证
- [ ] **InputValidator** - 输入校验器
- [ ] **AutoComplete** - 自动完成
- [ ] **DropZone** - 拖放区域
- [ ] **RichTextToolBar** - 富文本工具栏

---

## 七、性能与嵌入式支持

### PerformanceProfile
- [ ] 实现性能档位枚举
  ```cpp
  enum class PerformanceProfile {
      Desktop,     // 全功能：60fps，阴影，Ripple
      Embedded,    // 精简：30fps，无阴影，无 Ripple
      UltraLow     // 极简：无动画，无阴影
  };
  ```
- [ ] ThemeManager 支持性能档位
  - [ ] `setPerformanceProfile(PerformanceProfile)`
  - [ ] `performanceProfile()`

### 组件降级配置
- [ ] Desktop 模式
  - [ ] Ripple ✅
  - [ ] Shadow ✅
  - [ ] StateAnim ✅
  - [ ] FocusAnim ✅
- [ ] Embedded 模式
  - [ ] Ripple ❌
  - [ ] Shadow ❌
  - [ ] StateAnim ✅
  - [ ] FocusAnim ✅
- [ ] UltraLow 模式
  - [ ] Ripple ❌
  - [ ] Shadow ❌
  - [ ] StateAnim ❌
  - [ ] FocusAnim ❌

---

## 八、代码生成规范

### 继承规范
- [ ] 继承时必须先调用父类方法
- [ ] 所有 `event override` 第一行必须是 `QParentClass::xxxEvent(event)`

### paintEvent 规范
- [ ] 禁止 native 绘制
- [ ] 不得出现 `style()->drawControl`
- [ ] 不得出现 `initStyleOption`
- [ ] 不得出现 `QStylePainter`

### 组件组合规范
- [ ] 组件通过构造函数创建，parent 设为 this
  ```cpp
  m_animationFactory = Application::animationFactory();
  m_stateMachine = new StateMachine(m_animationFactory, this);
  m_ripple = new RippleHelper(m_animationFactory, this);
  m_elevation = new MdElevationController(m_animationFactory, this);
  m_focusIndicator = new MdFocusIndicator(m_animationFactory, this);
  ```

### 颜色访问规范
- [ ] 颜色全部来自 Theme
- [ ] 不得出现硬编码颜色值
- [ ] 通过 Token 字符串动态访问

### 单位转换规范
- [ ] 单位全部通过 CanvasUnitHelper 换算
- [ ] 不得硬编码像素值
- [ ] 间距和圆角均以 dp 为单位

### 动画创建规范
- [ ] 动画通过 CFMaterialAnimationFactory 创建
- [ ] 不得在控件内直接 `new QPropertyAnimation`
- [ ] 不得直接调用 `QTimer`

### Theme 监听标准写法
```cpp
connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
        this, [this](const ICFTheme&) { update(); });
```

### sizeHint 规范
- [ ] sizeHint 必须基于 Token 计算
- [ ] 通过 `CanvasUnitHelper::dpToPx()` 换算

---

## 九、相关文档

- 架构规范: [../../ui/MaterialRules.md](../../ui/MaterialRules.md)
- 依赖: [00_project_skeleton.md](00_project_skeleton.md), [02_base_library.md](02_base_library.md), [03_input_layer.md](03_input_layer.md)

---

*最后更新: 2026-03-11*

---

## 已完成归档

**Layer 1-4 及 P0 核心控件已完成并归档**: [done/99_ui_layer1-4_and_p0_widgets_done.md](done/99_ui_layer1-4_and_p0_widgets_done.md)
