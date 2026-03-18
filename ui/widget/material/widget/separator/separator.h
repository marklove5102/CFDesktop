/**
 * @file    ui/widget/material/widget/separator/separator.h
 * @brief   Material Design 3 Separator widget.
 *
 * Implements Material Design 3 separator (divider) with support for
 * horizontal and vertical orientations, full-bleed, inset, and
 * middle-inset spacing modes.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include "base/color.h"
#include "export.h"
#include <QFrame>
#include <QWidget>

namespace cf::ui::widget::material {

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Separator spacing mode.
 *
 * @details Defines the inset/spacing behavior of the separator line.
 *          Full-bleed spans the entire width/height, Inset adds margins
 *          on both sides, and Middle-inset adds margin on one side.
 *
 * @since  N/A
 * @ingroup ui_widget_material_widget
 */
enum class SeparatorMode {
    FullBleed,  ///< Spans entire width/height
    Inset,      ///< 16dp margin on both sides
    MiddleInset ///< 16dp margin on leading/start side
};

/**
 * @brief  Material Design 3 Separator widget.
 *
 * @details Implements Material Design 3 separator (divider) with support
 *          for horizontal and vertical orientations. The separator is
 *          drawn as a 1dp line using OUTLINE color with 40%
 *          opacity for better visibility. Supports three spacing modes: Full-bleed, Inset,
 *          and Middle-inset.
 *
 * @since  N/A
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT Separator : public QFrame {
    Q_OBJECT
    Q_PROPERTY(SeparatorMode mode READ mode WRITE setMode)

  public:
    /**
     * @brief  Constructor with orientation.
     *
     * @param[in]     orientation Qt::Horizontal or Qt::Vertical.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Defaults to Full-bleed mode.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    explicit Separator(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    ~Separator() override;

    /**
     * @brief  Gets the spacing mode.
     *
     * @return        Current separator mode.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    SeparatorMode mode() const;

    /**
     * @brief  Sets the spacing mode.
     *
     * @param[in]     mode Separator spacing mode.
     *
     * @throws        None
     * @note          Changing mode triggers a repaint.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setMode(SeparatorMode mode);

    /**
     * @brief  Gets the orientation.
     *
     * @return        Current orientation (Horizontal or Vertical).
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    Qt::Orientation orientation() const;

    /**
     * @brief  Sets the orientation.
     *
     * @param[in]     orientation Qt::Horizontal or Qt::Vertical.
     *
     * @throws        None
     * @note          Changing orientation triggers a repaint.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void setOrientation(Qt::Orientation orientation);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the separator.
     *
     * @throws        None
     * @note          Thickness is 1dp, length expands to parent.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

  protected:
    /**
     * @brief  Paints the separator.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Draws a 1dp line with proper inset based on mode.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /**
     * @brief  Gets the separator color.
     *
     * @return        Outline variant color with 12% opacity.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    CFColor separatorColor() const;

    /**
     * @brief  Calculates the separator line rect.
     *
     * @return        Rectangle for drawing the separator line.
     *
     * @throws        None
     * @note          Applies inset based on mode and orientation.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_widget
     */
    QRectF separatorRect() const;

    SeparatorMode mode_;
    Qt::Orientation orientation_;
};

} // namespace cf::ui::widget::material
