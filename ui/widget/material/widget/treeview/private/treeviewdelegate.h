/**
 * @file    treeviewdelegate.h
 * @brief   Material Design 3 TreeView Delegate
 *
 * Provides custom rendering and size hints for TreeView items following
 * Material Design 3 specifications. Handles tree lines, expand/collapse
 * icons, item backgrounds, and proper indentation.
 *
 * @author  Material Design Framework Team
 * @date    2026-03-18
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include "../treeview.h"
#include "base/color.h"
#include "export.h"
#include <QPersistentModelIndex>
#include <QStyledItemDelegate>

class QTreeView;

namespace cf::ui::widget::material {

// TreeItemHeight and TreeIndentStyle are defined in treeview.h

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 TreeView item delegate.
 *
 * @details Handles rendering of individual tree items including:
 *          - Background and selection states
 *          - Tree connection lines
 *          - Expand/collapse icons
 *          - Text and decoration icons
 *          - Hover and pressed states
 *          - Proper indentation per level
 *
 *          The delegate does NOT handle:
 *          - Ripple effects (handled by TreeView)
 *          - Focus indicators (handled by TreeView)
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT TreeViewItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          Defaults to Standard item height and Material indent style.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit TreeViewItemDelegate(QObject* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~TreeViewItemDelegate() override;

    /**
     * @brief  Paints the tree item.
     *
     * @param[in]     painter QPainter to render with.
     * @param[in]     option Style options for the item.
     * @param[in]     index Model index for the item.
     *
     * @throws        None
     * @note          Called by Qt's view framework for each visible item.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    /**
     * @brief  Gets the size hint for the item.
     *
     * @param[in]     option Style options for the item.
     * @param[in]     index Model index for the item.
     *
     * @return        Size hint for the item.
     *
     * @throws        None
     * @note          Height is fixed based on itemHeight setting.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * @brief  Sets the item height mode.
     *
     * @param[in]     height Item height mode to use.
     *
     * @throws        None
     * @note          Triggers layout update.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setItemHeight(TreeItemHeight height);

    /**
     * @brief  Sets the indentation style.
     *
     * @param[in]     style Indentation style to use.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setIndentStyle(TreeIndentStyle style);

    /**
     * @brief  Sets whether tree connection lines are shown.
     *
     * @param[in]     show true to show tree lines, false otherwise.
     *
     * @throws        None
     * @note          Triggers repaint.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setShowTreeLines(bool show);

    /**
     * @brief  Gets the current item height in pixels.
     *
     * @return        Item height in device pixels.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    float itemHeightValue() const;

    /**
     * @brief  Gets the indentation per level in pixels.
     *
     * @return        Indentation width in device pixels.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    float indentPerLevel() const;

    /**
     * @brief  Gets the horizontal padding in pixels.
     *
     * @return        Horizontal padding in device pixels.
     *
     * @throws        None
     * @note          Used by TreeView for expand/collapse hit testing.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    float horizontalPaddingValue() const;

    /**
     * @brief  Gets the expand/collapse icon size in pixels.
     *
     * @return        Icon size in device pixels.
     *
     * @throws        None
     * @note          Used by TreeView for expand/collapse hit testing.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    float iconSizeValue() const;

  private:
    // Drawing helpers
    void drawBackground(QPainter* painter, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const;
    void drawTreeLines(QPainter* painter, const QModelIndex& index, const QRectF& itemRect,
                       const QTreeView* view) const;
    void drawExpandCollapseIcon(QPainter* painter, const QRectF& iconRect, bool expanded,
                                const CFColor& color) const;
    void drawCheckBox(QPainter* painter, const QRectF& checkRect, Qt::CheckState state,
                      const QStyleOptionViewItem& option) const;
    void drawItemContent(QPainter* painter, const QRectF& contentRect,
                         const QStyleOptionViewItem& option, const QModelIndex& index) const;

    // Tree structure helpers
    int getDepth(const QModelIndex& index, const QTreeView* view) const;
    bool isLastSibling(const QModelIndex& index, const QTreeView* view) const;
    bool shouldContinueLineAtLevel(const QModelIndex& index, int level,
                                   const QTreeView* view) const;

    // Layout helpers
    float horizontalPadding() const;
    float iconSize() const;

    // Color access methods
    CFColor surfaceColor() const;
    CFColor onSurfaceColor() const;
    CFColor onSurfaceVariantColor() const;
    CFColor primaryContainerColor() const;
    CFColor outlineVariantColor() const;

    // Properties
    TreeItemHeight m_itemHeight;
    TreeIndentStyle m_indentStyle;
    bool m_showTreeLines;
    float m_itemHeightPx;
    float m_indentPx;
};

} // namespace cf::ui::widget::material
