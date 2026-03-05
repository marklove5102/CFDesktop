# 05 - Testing System Reference

## Module Overview

The Testing System provides comprehensive testing infrastructure for CFDesktop, including unit tests, integration tests, and UI component tests. The system is built on Google Test framework and supports cross-platform testing.

**Module Purpose:**
- Provide unit testing framework for core utilities
- Enable UI component testing
- Support system information query testing
- Facilitate boot sequence testing

---

## Current Implementation Status

**Completion: 40%**

### Completed Components
- Basic test framework integration (Google Test)
- Base utility tests (math, color, geometry)
- UI component behavior tests
- System query tests
- Boot tests

### Missing Components
- Widget integration tests
- UI automation tests
- Performance benchmarks
- Fuzzing tests
- Mock hardware framework

---

## Test Directory Structure

```
test/
├── CMakeLists.txt                    # Test build configuration
├── base/                             # Base utility tests
│   ├── weak_ptr/
│   │   └── weak_ptr_test.cpp        # WeakPtr implementation tests
│   ├── expected/
│   │   └── expected_test.cpp        # Expected/Result type tests
│   ├── hash/
│   │   └── constexpr_fnv1a_test.cpp # Compile-time hash tests
│   └── scope_guard/
│       └── scope_guard_test.cpp     # RAII scope guard tests
├── ui/                               # UI layer tests
│   ├── base/
│   │   ├── device_pixel_test.cpp    # Device pixel conversion tests
│   │   ├── easing_test.cpp          # Easing curve tests
│   │   ├── math_helper_test.cpp     # Math helper tests (comprehensive)
│   │   ├── color_test.cpp           # Color utility tests
│   │   ├── color_helper_test.cpp    # Color helper tests
│   │   └── geometry_helper_test.cpp # Geometry helper tests
│   ├── components/
│   │   ├── elevation_controller_test.cpp  # Elevation shadow tests
│   │   ├── focus_ring_test.cpp            # Focus indicator tests
│   │   ├── painter_layer_test.cpp         # Painter layer tests
│   │   ├── ripple_helper_test.cpp         # Ripple effect tests
│   │   └── state_machine_test.cpp         # State machine tests
│   └── core/
│       └── token_test.cpp            # Theme token tests
├── system/                           # System query tests
│   ├── test_memory_info_query.cpp   # Memory info query tests
│   └── test_cpu_info_query.cpp      # CPU info query tests
└── boot_test/                        # Boot sequence tests
    ├── boot_detect.cpp               # Boot detection
    ├── boot_test_core.cpp            # Core boot tests
    ├── boot_test_gui.cpp             # GUI boot tests
    ├── CMakeLists.txt
    └── README.md                     # Boot test documentation
```

---

## Completed Test Files

### Base Utility Tests

#### `test/base/weak_ptr/weak_ptr_test.cpp`
- Tests for `cf::WeakPtr` implementation
- Ownership semantics validation
- Lifecycle management tests

#### `test/base/expected/expected_test.cpp`
- Tests for `cf::Expected<T, E>` type
- Success/Error path validation
- Monadic operations tests

#### `test/base/hash/constexpr_fnv1a_test.cpp`
- Compile-time FNV-1a hash tests
- Collision resistance tests
- Performance validation

#### `test/base/scope_guard/scope_guard_test.cpp`
- RAII scope guard tests
- Exception safety validation

### UI Base Tests

#### `test/ui/base/math_helper_test.cpp`
**Coverage:** Comprehensive
- `lerp` - Linear interpolation (7 test cases)
- `clamp` - Value clamping (6 test cases)
- `remap` - Range remapping (8 test cases)
- `cubicBezier` - Bezier curve evaluation (7 test cases)
- `springStep` - Spring physics (7 test cases)
- `lerpAngle` - Angle interpolation (10 test cases)

#### `test/ui/base/device_pixel_test.cpp`
- Device pixel ratio conversion tests
- DP to pixel conversion tests

#### `test/ui/base/easing_test.cpp`
- QEasingCurve wrapper tests
- Material easing curve validation

#### `test/ui/base/color_test.cpp` & `color_helper_test.cpp`
- Color space conversion tests
- Material color system tests
- Contrast ratio calculation tests

#### `test/ui/base/geometry_helper_test.cpp`
- Rounded rect path generation
- Rectangle clipping tests

### UI Component Tests

#### `test/ui/components/state_machine_test.cpp`
**Coverage:** Material Behavior Layer
- State transitions (8 test cases)
- Combined states (2 test cases)
- Opacity calculation (6 test cases)
- State priority (2 test cases)

**Validates:**
- Normal, Hovered, Pressed, Focused, Disabled, Checked states
- Material Design 3 opacity values (0.00, 0.08, 0.12)
- Priority: Disabled > Pressed > Dragged > Focused > Hovered > Normal

#### `test/ui/components/ripple_helper_test.cpp`
- Ripple creation and lifecycle
- Multi-ripple support
- Fade animations

#### `test/ui/components/elevation_controller_test.cpp`
- Shadow rendering tests
- Elevation level validation (0-5)
- Dark theme tonal overlay tests

#### `test/ui/components/focus_ring_test.cpp`
- Focus ring rendering
- Material spec compliance (3dp width, 3dp padding)

#### `test/ui/components/painter_layer_test.cpp`
- Color overlay layer tests
- Alpha blending validation

### UI Core Tests

#### `test/ui/core/token_test.cpp`
- Token system validation
- Color scheme token tests
- Typography token tests

### System Tests

#### `test/system/test_memory_info_query.cpp`
- Memory info query tests
- Cross-platform memory API validation

#### `test/system/test_cpu_info_query.cpp`
- CPU info query tests
- Processor count detection

### Boot Tests

#### `test/boot_test/`
- Boot sequence validation
- Core initialization tests
- GUI initialization tests

---

## Missing Test Coverage

### Widget Integration Tests (P0)
**Status:** Not Implemented

Required tests for Material widgets:
- `button_test.cpp` - Button widget tests
- `textfield_test.cpp` - TextField widget tests
- `textarea_test.cpp` - TextArea widget tests
- `checkbox_test.cpp` - CheckBox widget tests
- `radiobutton_test.cpp` - RadioButton widget tests
- `label_test.cpp` - Label widget tests
- `groupbox_test.cpp` - GroupBox widget tests

### Animation Engine Tests (P1)
**Status:** Not Implemented

Required tests:
- `timing_animation_test.cpp`
- `spring_animation_test.cpp`
- `animation_group_test.cpp`
- `animation_factory_test.cpp`

### Theme Engine Tests (P1)
**Status:** Partial (token_test.cpp exists)

Missing:
- `theme_manager_test.cpp`
- `color_scheme_test.cpp`
- `motion_spec_test.cpp`

### UI Automation Tests (P2)
**Status:** Not Implemented

Required:
- Mouse event simulation
- Keyboard event simulation
- Touch event simulation
- Screenshot comparison tests

### Performance Benchmarks (P2)
**Status:** Not Implemented

Required:
- `widget_benchmark.cpp` - Widget rendering performance
- `animation_benchmark.cpp` - Animation frame rate tests
- `memory_benchmark.cpp` - Memory usage profiling

---

## Test Framework Details

### Google Test Integration
- Version: Google Test (via third_party/googletest)
- Build system: CMake
- Test discovery: Automatic via CMake TEST discovery
- Output format: Standard Google Test output

### Running Tests

**Build tests:**
```bash
# From project root
cmake -B out/build_test -DBUILD_TESTING=ON
cmake --build out/build_test
```

**Run all tests:**
```bash
./out/build_test/bin/cf_desktop_tests
```

**Run specific test:**
```bash
./out/build_test/bin/cf_desktop_tests --gtest_filter=MathHelperTest.*
```

---

## Test Coverage Statistics

| Module | Files | Coverage | Status |
|--------|-------|----------|--------|
| Base Utilities | 4 | 90% | Good |
| UI Base | 6 | 85% | Good |
| UI Components | 5 | 60% | Fair |
| UI Core | 1 | 30% | Poor |
| UI Widgets | 0 | 0% | Missing |
| System Queries | 2 | 70% | Fair |
| Boot Tests | 3 | 80% | Good |
| **Overall** | **21** | **40%** | **Fair** |

---

## Key File Paths

| Test Type | Path |
|-----------|------|
| Base Tests | `d:/ProjectHome/CFDesktop/test/base/` |
| UI Tests | `d:/ProjectHome/CFDesktop/test/ui/` |
| System Tests | `d:/ProjectHome/CFDesktop/test/system/` |
| Boot Tests | `d:/ProjectHome/CFDesktop/test/boot_test/` |
| Test Config | `d:/ProjectHome/CFDesktop/test/CMakeLists.txt` |

---

## Next Steps

1. **Add Widget Tests** - Create tests for all P0 Material widgets
2. **Add Animation Tests** - Test timing/spring animations
3. **Create Mock Framework** - For hardware abstraction layer testing
4. **Add UI Automation** - Mouse/keyboard/touch simulation tests
5. **Performance Benchmarks** - Establish baseline performance metrics
6. **CI Integration** - Add automated test running to CI/CD

---

## Related Documents

- `04_simulator_ref.md` - Simulator for hardware mock testing
- `99_ui_material_framework_ref.md` - UI framework being tested
- `01_architecture_ref.md` - Overall architecture

---

*Last Updated: 2026-03-05*
*Status: Partially Implemented (40%)*
