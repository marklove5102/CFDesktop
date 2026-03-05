# 99 - UI Material Framework Reference

## Module Overview

The UI Material Framework is a Qt6 Widgets-based implementation of Google Material Design 3. It provides a comprehensive set of UI components following Material Design principles with a focus on embedded systems compatibility and cross-platform support.

**Architecture:**
- 6-Layer inheritance-preserving design
- Token-based theming system
- Animation engine with embedded support
- Hardware-accelerated rendering where available

**Key Principles:**
- Inherit from Qt native widgets (no parallel widget hierarchy)
- Complete paintEvent override (no QSS usage)
- Token-based design system
- Performance profiling support for embedded targets

---

## Current Implementation Status

**Overall Completion: 62%**

### Layer Breakdown

| Layer | Name | Completion | Status |
|-------|------|------------|--------|
| Layer 1 | Core Math & Utility | 100% | Complete |
| Layer 2 | Theme Engine | 100% | Complete |
| Layer 3 | Animation Engine | 100% | Complete |
| Layer 4 | Material Behavior | 100% | Complete |
| Layer 5 | Material Widget | 25% | Partial |
| Layer 6 | Performance Profile | 0% | Not Started |

---

## Layer 1: Core Math & Utility (100% Complete)

### Completed Files

| File | Path | Purpose |
|------|------|---------|
| `math_helper.h` | `ui/base/math_helper.h` | lerp, clamp, remap, cubicBezier, springStep, lerpAngle |
| `color.h` | `ui/base/color.h` | Color types and conversions |
| `color_helper.h` | `ui/base/color_helper.h` | Color space utilities, contrast calculation |
| `easing.h` | `ui/base/easing.h` | Material Design easing curves |
| `geometry_helper.h` | `ui/base/geometry_helper.h` | Rounded paths, clipping shapes |
| `device_pixel.h` | `ui/base/device_pixel.h` | DP/PX conversion |

**Key Features:**
- Zero Qt UI dependency (QtCore only)
- Comprehensive math functions for animation and rendering
- Material Color System support
- Compile-time hash functions (FNV-1a)

---

## Layer 2: Theme Engine (100% Complete)

### Token System Architecture

```
Reference Token (MD3 spec)
       |
       v
System Token (cfmaterial scheme)
       |
       v
Component Token (widget-specific)
```

### Completed Files

**Core Interfaces:**
- `ui/core/theme.h` - ICFTheme interface
- `ui/core/theme_manager.h` - Singleton theme manager
- `ui/core/theme_factory.h` - Theme factory base class

**Color System:**
- `ui/core/color_scheme.h` - ICFColorScheme interface
- `ui/core/material/cfmaterial_scheme.h` - Material color implementation
- `ui/core/token/material_scheme/cfmaterial_token_literals.h` - Color token literals

**Typography:**
- `ui/core/font_type.h` - IFontType interface
- `ui/core/material/cfmaterial_fonttype.h` - Material typography implementation
- `ui/core/token/typography/cfmaterial_typography_token_literals.h` - Type scale tokens

**Motion:**
- `ui/core/motion_spec.h` - Motion specification interface
- `ui/core/material/cfmaterial_motion.h` - Material motion implementation
- `ui/core/token/motion/cfmaterial_motion_token_literals.h` - Motion tokens

**Shape:**
- `ui/core/radius_scale.h` - Shape scale interface
- `ui/core/material/cfmaterial_radius_scale.h` - Material shape implementation
- `ui/core/token/radius_scale/cfmaterial_radius_scale_literals.h` - Radius tokens

**Theme Factory:**
- `ui/core/material/material_factory_class.h` - Material theme factory

**Theme Names:**
- `ui/core/token/theme_name/material_theme_name.h` - Theme name constants

### Usage Pattern
```cpp
// Get theme
auto& theme = ThemeManager::instance().theme("material_light");

// Access tokens
auto color = theme.colorScheme()->getColor("md.sys.color.primary");
auto font = theme.typography()->getFont("md.sys.type.scale.body-large");
auto radius = theme.shape()->getRadius("md.sys.shape.corner.full");
```

---

## Layer 3: Animation Engine (100% Complete)

### Completed Files

**Core Interfaces:**
- `ui/components/animation.h` - ICFAbstractAnimation base class
- `ui/components/timing_animation.h` - ICFTimingAnimation (time-based)
- `ui/components/spring_animation.h` - ICFSpringAnimation (physics-based)
- `ui/components/animation_group.h` - ICFAnimationGroup (parallel/sequential)

**Factory System:**
- `ui/components/animation_factory_manager.h` - ICFAnimationManagerFactory interface
- `ui/components/material/cfmaterial_animation_factory.h` - Material factory implementation
- `ui/components/material/cfmaterial_animation_strategy.h` - Animation strategy pattern

**Concrete Animations:**
- `ui/components/material/cfmaterial_fade_animation.h` - Fade in/out
- `ui/components/material/cfmaterial_scale_animation.h` - Scale transformations
- `ui/components/material/cfmaterial_slide_animation.h` - Slide animations
- `ui/components/material/cfmaterial_property_animation.h` - Qt property animation wrapper

**Tokens:**
- `ui/components/material/token/animation_token_literals.h` - Animation token definitions
- `ui/components/material/token/animation_token_mapping.h` - Token-to-animation mapping

### Animation States
```
Idle -> Running -> (Paused | Finished)
         |
         v
     Stopped
```

### Supported Token Types
- `md.animation.fadeIn`
- `md.animation.fadeOut`
- `md.animation.slideUp`
- `md.animation.slideDown`
- `md.animation.scaleUp`
- `md.animation.scaleDown`
- `md.animation.rotateIn`
- `md.animation.rotateOut`

---

## Layer 4: Material Behavior (100% Complete)

### Completed Files

| Component | Header | Source | Purpose |
|-----------|--------|--------|---------|
| StateMachine | `ui/widget/material/base/state_machine.h` | `state_machine.cpp` | Visual state management |
| PainterLayer | `ui/widget/material/base/painter_layer.h` | `painter_layer.cpp` | Color overlay layer |
| RippleHelper | `ui/widget/material/base/ripple_helper.h` | `ripple_helper.cpp` | Ripple effect controller |
| MdElevationController | `ui/widget/material/base/elevation_controller.h` | `elevation_controller.cpp` | Shadow/elevation system |
| MdFocusIndicator | `ui/widget/material/base/focus_ring.h` | `focus_ring.cpp` | Focus ring renderer |

### State Machine States & Opacity

| State | Opacity | Description |
|-------|:-------:|-------------|
| Normal | 0.00 | Default state |
| Hovered | 0.08 | Mouse hover |
| Pressed | 0.12 | Mouse pressed |
| Focused | 0.12 | Keyboard focus |
| Dragged | 0.16 | Being dragged |
| Checked | 0.08 | Selected state |
| Disabled | 0.00 | Disabled state |

**Priority:** Disabled > Pressed > Dragged > Focused > Hovered > Normal

### Elevation Levels

| Level | DP | Shadow (Light) | Tonal (Dark) |
|-------:|---:|---------------|--------------|
| 0 | 0dp | None | None |
| 1 | 1dp | blur=2px, offset=1px | overlay 1 |
| 2 | 3dp | blur=4px, offset=2px | overlay 2 |
| 3 | 6dp | blur=8px, offset=4px | overlay 3 |
| 4 | 8dp | blur=12px, offset=6px | overlay 4 |
| 5 | 12dp | blur=16px, offset=8px | overlay 5 |

---

## Layer 5: Material Widget (25% Complete)

### Completed Widgets (P0 - 7/7)

| Widget | Files | Qt Base | Status |
|--------|-------|---------|--------|
| **Button** | `ui/widget/material/widget/button/button.{h,cpp}` | QPushButton | Complete |
| **TextField** | `ui/widget/material/widget/textfield/textfield.{h,cpp}` | QLineEdit | Complete |
| **TextArea** | `ui/widget/material/widget/textarea/textarea.{h,cpp}` | QTextEdit | Complete |
| **Label** | `ui/widget/material/widget/label/label.{h,cpp}` | QLabel | Complete |
| **CheckBox** | `ui/widget/material/widget/checkbox/checkbox.{h,cpp}` | QCheckBox | Complete |
| **RadioButton** | `ui/widget/material/widget/radiobutton/radiobutton.{h,cpp}` | QRadioButton | Complete |
| **GroupBox** | `ui/widget/material/widget/groupbox/groupbox.{h,cpp}` | QGroupBox | Complete |

### Button Variants
- Filled (primary)
- Outlined (secondary)
- Text (tertiary)
- Elevated (elevated)

### Pending Widgets (P1 - 0/12)

| Widget | Qt Base | Priority |
|--------|---------|----------|
| ComboBox | QComboBox | P1 |
| Slider | QSlider | P1 |
| ProgressBar | QProgressBar | P1 |
| Switch | QCheckBox (custom) | P1 |
| ListView | QListView | P1 |
| TreeView | QTreeView | P1 |
| TableView | QTableView | P1 |
| TabView | QTabWidget | P1 |
| ScrollView | QScrollArea | P1 |
| Separator | QFrame | P1 |
| SpinBox | QSpinBox | P1 |
| DoubleSpinBox | QDoubleSpinBox | P1 |

### Pending Widgets (P2 - 0/27)

| Widget | Qt Base | Priority |
|--------|---------|----------|
| DatePicker | QCalendarWidget | P2 |
| TimePicker | QTimeEdit | P2 |
| ColorPicker | QColorDialog | P2 |
| MenuBar | QMenuBar | P2 |
| ContextMenu | QMenu | P2 |
| ToolBar | QToolBar | P2 |
| StatusBar | QStatusBar | P2 |
| Dialog | QDialog | P2 |
| Snackbar | QWidget (custom) | P2 |
| Card | QFrame (custom) | P2 |
| FloatingActionButton | QPushButton (custom) | P2 |
| BottomNavigation | QWidget (custom) | P2 |
| Drawer | QWidget (custom) | P2 |
| SearchBar | QLineEdit (custom) | P2 |
| Dial | QDial | P2 |
| Stepper | QWidget (custom) | P2 |
| Chip | QWidget (custom) | P2 |
| Badge | QWidget (custom) | P2 |
| Tooltip | QToolTip | P2 |
| Popover | QWidget (custom) | P2 |
| CircularProgressBar | QWidget (custom) | P2 |
| [Additional P2 widgets...] | - | P2 |

### Pending Widgets (P3 - 0/25)

Advanced/professional widgets including SplitView, ChartView, DrawingArea, Canvas, FileList, FileTree, CodeEditor, Calendar, Wizard, PropertyEditor, TreeTable, Gauge, Timeline, GridView, SwipeableItem, BottomSheet, NotificationList, Breadcrumbs, Pagination, RatingBar, RangeSlider, SliderCaptcha, InputValidator, AutoComplete, DropZone, RichTextToolBar.

---

## Layer 6: Performance Profile (0% Complete)

### Planned System

```cpp
enum class PerformanceProfile {
    Desktop,   // Full: 60fps, shadows, ripples, all animations
    Embedded,  // Reduced: 30fps, no shadows, no ripples, state anims only
    UltraLow   // Minimal: No animations, no shadows, color only
};
```

### Degradation Strategy

| Feature | Desktop | Embedded | UltraLow |
|---------|:-------:|:--------:|:--------:|
| Ripple | Yes | No | No |
| Shadow | Yes | No | No |
| State Animation | Yes | Yes | No |
| Focus Animation | Yes | Yes | No |
| Target FPS | 60 | 30 | N/A |

---

## Key Design Rules

### RULE-01: Inheritance Only
All Material controls must inherit Qt native controls.

### RULE-02: Signal Preservation
No removal/modification of Qt public signals.

### RULE-03: Complete Paint Override
paintEvent must fully control rendering. No `style()->drawControl()`.

### RULE-04: No QSS
Qt StyleSheet forbidden for Material visuals.

### RULE-05: No Qt Quick
QML/Qt Quick not used.

### RULE-06: Animation Disable
Animation system must be globally disableable.

### RULE-07: Theme Read-Only
Widgets cannot modify global theme state.

### RULE-08: Component Injection
Render components injected via composition, not direct `new`.

### RULE-09: Token Access
Theme access via token strings through context-aware interfaces.

---

## Architecture Specification

Reference: `ui/MaterialRules.md` - Complete architecture v2.0 specification

### Layer Dependency Rules

```
ui/widget/material  -> ui/components, ui/core, ui/base
ui/components       -> ui/core, ui/base
ui/core             -> ui/base
ui/base             -> QtCore only (no QtWidgets/QtGui)
```

### Dependency Priority
1. Use `ui/base` utilities first
2. Create new base utilities only if existing don't suffice
3. No circular dependencies

---

## Widget Implementation Template

All widgets follow the Button implementation pattern:

```cpp
class Button : public QPushButton {
    Q_OBJECT

public:
    explicit Button(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    StateMachine* m_stateMachine;
    RippleHelper* m_ripple;
    MdElevationController* m_elevation;
    MdFocusIndicator* m_focusIndicator;
};
```

### Standard Event Pattern
```cpp
void Widget::enterEvent(QEnterEvent* event) {
    QPushButton::enterEvent(event);  // Always call parent first
    m_stateMachine->onHoverEnter();
    update();
}
```

### Standard Paint Pattern
```cpp
void Widget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Background
    painter.fillRect(rect(), backgroundColor());

    // 2. State layer
    m_stateMachine->paintStateLayer(&painter, rect());

    // 3. Content
    paintContent(&painter);

    // 4. Elevation shadow
    m_elevation->paintShadow(&painter, rect());

    // 5. Focus ring
    if (hasFocus()) {
        m_focusIndicator->paint(&painter, rect());
    }
}
```

---

## Completion Summary

| Category | Total | Complete | Percent |
|----------|------|----------|--------|
| P0 Core Widgets | 7 | 7 | 100% |
| P1 Common Widgets | 12 | 0 | 0% |
| P2 Advanced Widgets | 27 | 0 | 0% |
| P3 Professional Widgets | 25 | 0 | 0% |
| **Total Widgets** | **71** | **7** | **10%** |
| **Overall Framework** | **** | **** | **62%** |

**Note:** Overall framework completion includes Layer 1-4 (100%) plus Layer 5 widgets (10%), plus Layer 6 (0%).

---

## Key File Paths

### Core Files
| Component | Path |
|-----------|------|
| Architecture Spec | `d:/ProjectHome/CFDesktop/ui/MaterialRules.md` |
| Theme Manager | `d:/ProjectHome/CFDesktop/ui/core/theme_manager.h` |
| Animation Factory | `d:/ProjectHome/CFDesktop/ui/components/animation_factory_manager.h` |
| Base Math | `d:/ProjectHome/CFDesktop/ui/base/math_helper.h` |

### Widget Paths
| Widget | Path |
|--------|------|
| Button | `d:/ProjectHome/CFDesktop/ui/widget/material/widget/button/` |
| TextField | `d:/ProjectHome/CFDesktop/ui/widget/material/widget/textfield/` |
| TextArea | `d:/ProjectHome/CFDesktop/ui/widget/material/widget/textarea/` |
| CheckBox | `d:/ProjectHome/CFDesktop/ui/widget/material/widget/checkbox/` |
| RadioButton | `d:/ProjectHome/CFDesktop/ui/widget/material/widget/radiobutton/` |
| Label | `d:/ProjectHome/CFDesktop/ui/widget/material/widget/label/` |
| GroupBox | `d:/ProjectHome/CFDesktop/ui/widget/material/widget/groupbox/` |

---

## Next Steps

1. **Implement P1 Widgets** - ComboBox, Slider, ProgressBar, Switch (Priority: High)
2. **Add PerformanceProfile** - Implement embedded performance modes (Priority: High)
3. **Implement P2 Widgets** - Dialog, Snackbar, Card, MenuBar (Priority: Medium)
4. **Add Widget Tests** - Unit tests for all implemented widgets
5. **Documentation** - Widget usage examples and API docs
6. **Accessibility** - Screen reader support, keyboard navigation

---

## Related Documents

- `01_architecture_ref.md` - Overall system architecture
- `05_testing_ref.md` - Testing status for UI components
- `MaterialRules.md` - Complete architecture specification

---

*Last Updated: 2026-03-05*
*Status: 62% Complete (Layer 1-4: 100%, Layer 5: 10%, Layer 6: 0%)*
