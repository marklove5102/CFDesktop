/**
 * @file    ui/widget/material/widget/tabview/tabview.h
 * @brief   Material Design 3 TabView widget.
 *
 * Implements Material Design 3 tab widget with support for tab labels,
 * selection indicator with sliding animation, and tab scrolling. Includes
 * state layers, focus indicators, and Material Design 3 styling.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include <QTabWidget>
#include <QWidget>

#include "base/color.h"
#include "export.h"

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

// Forward declaration for internal TabBar implementation
class MaterialTabBar;

/**
 * @brief  Material Design 3 TabView widget.
 *
 * @details Implements Material Design 3 tab widget with support for tab labels,
 *          selection indicator with sliding animation, and tab scrolling.
 *          The TabBar implementation is internal and not exposed.
 *
 * @since  N/A
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT TabView : public QTabWidget {
    Q_OBJECT
    Q_PROPERTY(int tabHeight READ tabHeight WRITE setTabHeight)
    Q_PROPERTY(int tabMinWidth READ tabMinWidth WRITE setTabMinWidth)
    Q_PROPERTY(bool showIndicator READ showIndicator WRITE setShowIndicator)

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Initializes with default Material Design styling.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    explicit TabView(QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          Components are parented to this; Qt deletes them.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    ~TabView() override;

    /**
     * @brief  Gets the tab height.
     *
     * @return        Tab height in device-independent pixels.
     *
     * @throws        None
     * @note          Default height follows Material Design 3 specifications.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    int tabHeight() const;

    /**
     * @brief  Sets the tab height.
     *
     * @param[in]     height Tab height in device-independent pixels.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setTabHeight(int height);

    /**
     * @brief  Gets the minimum tab width.
     *
     * @return        Minimum tab width in device-independent pixels.
     *
     * @throws        None
     * @note          Default width follows Material Design 3 specifications.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    int tabMinWidth() const;

    /**
     * @brief  Sets the minimum tab width.
     *
     * @param[in]     width Minimum tab width in device-independent pixels.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setTabMinWidth(int width);

    /**
     * @brief  Gets whether the selection indicator is shown.
     *
     * @return        true if indicator is visible, false otherwise.
     *
     * @throws        None
     * @note          The indicator slides to the selected tab.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    bool showIndicator() const;

    /**
     * @brief  Sets whether the selection indicator is shown.
     *
     * @param[in]     show true to show indicator, false to hide.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setShowIndicator(bool show);

    /**
     * @brief  Sets whether a tab can be closed.
     *
     * @param[in]     index Tab index.
     * @param[in]     closeable true to make tab closeable, false otherwise.
     *
     * @throws        None
     * @note          Closeable tabs display a close button.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setTabCloseable(int index, bool closeable);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the tab view.
     *
     * @throws        None
     * @note          Based on content and tab dimensions.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

  signals:
    /**
     * @brief  Signal emitted when a tab close button is clicked.
     *
     * @param[in]     index Index of the tab to close.
     *
     * @note          Connect to this signal to handle tab closure.
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void tabCloseRequested(int index);

  private:
    MaterialTabBar* m_tabBar; // Internal TabBar implementation
};

} // namespace cf::ui::widget::material
