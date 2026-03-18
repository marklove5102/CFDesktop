/**
 * @file treeviewdelegate.cpp
 * @brief Material Design 3 TreeView Delegate Implementation
 *
 * Implements custom rendering and size hints for TreeView items.
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "treeviewdelegate.h"
#include "application_support/application.h"
#include "base/device_pixel.h"
#include "base/geometry_helper.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionViewItem>
#include <QTreeView>

namespace cf::ui::widget::material {

using namespace cf::ui::base;
using namespace cf::ui::base::device;
using namespace cf::ui::base::geometry;
using namespace cf::ui::core;
using namespace cf::ui::core::token::literals;
using namespace cf::ui::widget::application_support;

// ============================================================================
// Fallback Colors
// ============================================================================

namespace {
inline CFColor fallbackSurface() {
    return CFColor(253, 253, 253);
}
inline CFColor fallbackOnSurface() {
    return CFColor(27, 27, 31);
}
inline CFColor fallbackOnSurfaceVariant() {
    return CFColor(115, 119, 128);
}
inline CFColor fallbackPrimaryContainer() {
    return CFColor(234, 221, 255);
}
inline CFColor fallbackOutlineVariant() {
    return CFColor(120, 124, 132);
}
} // namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

TreeViewItemDelegate::TreeViewItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent), m_itemHeight(TreeItemHeight::Standard),
      m_indentStyle(TreeIndentStyle::Material), m_showTreeLines(true) {

    // Calculate initial sizes
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    m_itemHeightPx = helper.dpToPx(56.0f); // Standard height
    m_indentPx = helper.dpToPx(28.0f);     // Material indent (28dp per level)
}

TreeViewItemDelegate::~TreeViewItemDelegate() {
    // Qt handles cleanup
}

// ============================================================================
// QStyledItemDelegate Interface
// ============================================================================

void TreeViewItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const {
    if (!index.isValid() || !painter) {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);

    // Get the view for tree-specific operations
    const QTreeView* view = qobject_cast<const QTreeView*>(option.widget);
    if (!view) {
        // Fallback to default painting if not a TreeView
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QRectF itemRect = option.rect;

    // Check if this index is hovered by accessing the view's hovered index property
    // The view stores the hovered index as a dynamic property
    QStyleOptionViewItem opt = option;
    QVariant hoveredVariant = view->property("_q_hoveredIndex");
    if (hoveredVariant.isValid()) {
        QPersistentModelIndex hoveredIndex = hoveredVariant.value<QPersistentModelIndex>();
        if (hoveredIndex.isValid() && hoveredIndex == index) {
            opt.state |= QStyle::State_MouseOver;
        } else {
            opt.state &= ~QStyle::State_MouseOver;
        }
    } else {
        // Fallback: check cursor position
        QPoint cursorPos = view->viewport()->mapFromGlobal(QCursor::pos());
        QModelIndex indexAtCursor = view->indexAt(cursorPos);
        if (indexAtCursor.isValid() && indexAtCursor == index) {
            opt.state |= QStyle::State_MouseOver;
        } else {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }

    // Apply custom background from model if present
    QVariant bgVar = index.data(Qt::BackgroundRole);
    if (bgVar.isValid()) {
        QBrush bgBrush = bgVar.value<QBrush>();
        painter->fillRect(itemRect, bgBrush);
    }

    // 1. Draw background (selection, hover, state layer)
    drawBackground(painter, opt, index);

    // 2. Draw tree lines if enabled
    if (m_showTreeLines && m_indentStyle == TreeIndentStyle::Material) {
        drawTreeLines(painter, index, itemRect, view);
    }

    // 3. Calculate content area with indentation
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float hPadding = horizontalPadding();
    int depth = getDepth(index, view);
    float indent = m_indentPx;
    float iconSz = iconSize();
    float checkGap = helper.dpToPx(8.0f);
    float checkSize = helper.dpToPx(18.0f);

    // Account for expand/collapse icon space
    float contentLeft = hPadding + (indent * depth);
    if (index.model()->hasChildren(index)) {
        contentLeft += iconSz;
    }

    // Account for checkbox space if present
    QVariant checkVar = index.data(Qt::CheckStateRole);
    bool hasCheckbox = checkVar.isValid();
    if (hasCheckbox) {
        contentLeft += checkSize + checkGap;
    }

    QRectF contentRect = itemRect.adjusted(contentLeft, 0, 0, 0);

    // 4. Draw expand/collapse icon
    if (index.model()->hasChildren(index)) {
        QRectF expandRect(hPadding + (indent * depth), itemRect.center().y() - iconSz / 2.0f,
                          iconSz, iconSz);

        bool expanded = view->isExpanded(index);
        CFColor iconColor = onSurfaceVariantColor();
        drawExpandCollapseIcon(painter, expandRect, expanded, iconColor);
    }

    // 5. Draw checkbox if present
    if (hasCheckbox) {
        float checkX = hPadding + (indent * depth);
        if (index.model()->hasChildren(index)) {
            checkX += iconSz;
        }
        QRectF checkRect(checkX, itemRect.center().y() - checkSize / 2.0f, checkSize, checkSize);
        Qt::CheckState checkState = static_cast<Qt::CheckState>(checkVar.toInt());
        drawCheckBox(painter, checkRect, checkState, opt);
    }

    // 6. Draw item content (text and decoration)
    drawItemContent(painter, contentRect, opt, index);
}

QSize TreeViewItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const {
    if (!index.isValid()) {
        return QSize(-1, static_cast<int>(std::ceil(m_itemHeightPx)));
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float hPadding = horizontalPadding();
    float iconSz = iconSize();
    float iconGap = helper.dpToPx(12.0f);
    float checkGap = helper.dpToPx(8.0f);
    float checkSize = helper.dpToPx(18.0f);

    // Get tree view for depth calculation
    const QTreeView* view = qobject_cast<const QTreeView*>(option.widget);
    int depth = 0;
    if (view) {
        depth = getDepth(index, view);
    }

    // Calculate base width from indentation
    float width = hPadding + (m_indentPx * depth);

    // Add expand/collapse icon space if item has children
    if (index.model()->hasChildren(index)) {
        width += iconSz;
    }

    // Add checkbox space if present
    QVariant checkVar = index.data(Qt::CheckStateRole);
    if (checkVar.isValid()) {
        width += checkSize + checkGap;
    }

    // Add decoration icon space if present
    QVariant decoration = index.data(Qt::DecorationRole);
    if (decoration.isValid()) {
        QIcon icon = decoration.value<QIcon>();
        if (!icon.isNull()) {
            width += iconSz + iconGap;
        }
    }

    // Get font (use custom font if provided)
    QFont font = option.font;
    QVariant fontVar = index.data(Qt::FontRole);
    if (fontVar.isValid()) {
        font = fontVar.value<QFont>();
    }
    if (font.pixelSize() <= 0) {
        font.setPixelSize(static_cast<int>(helper.dpToPx(14.0f)));
    }

    // Calculate text width - use full text, not elided
    QString text = index.data(Qt::DisplayRole).toString();
    QFontMetrics fm(font);
    width += fm.horizontalAdvance(text);

    // Add right padding
    width += hPadding;

    return QSize(static_cast<int>(std::ceil(width)), static_cast<int>(std::ceil(m_itemHeightPx)));
}

// ============================================================================
// Property Setters
// ============================================================================

void TreeViewItemDelegate::setItemHeight(TreeItemHeight height) {
    if (m_itemHeight != height) {
        m_itemHeight = height;
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        if (height == TreeItemHeight::Compact) {
            m_itemHeightPx = helper.dpToPx(48.0f);
        } else {
            m_itemHeightPx = helper.dpToPx(56.0f);
        }
    }
}

void TreeViewItemDelegate::setIndentStyle(TreeIndentStyle style) {
    if (m_indentStyle != style) {
        m_indentStyle = style;
        CanvasUnitHelper helper(qApp->devicePixelRatio());
        if (style == TreeIndentStyle::Material) {
            m_indentPx = helper.dpToPx(28.0f);
        } else {
            m_indentPx = helper.dpToPx(20.0f);
        }
    }
}

void TreeViewItemDelegate::setShowTreeLines(bool show) {
    m_showTreeLines = show;
}

float TreeViewItemDelegate::itemHeightValue() const {
    return m_itemHeightPx;
}

float TreeViewItemDelegate::indentPerLevel() const {
    return m_indentPx;
}

// ============================================================================
// Drawing Helpers
// ============================================================================

void TreeViewItemDelegate::drawBackground(QPainter* painter, const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const {
    QRectF itemRect = option.rect;
    bool selected = option.state & QStyle::State_Selected;
    bool hovered = (option.state & QStyle::State_MouseOver);
    bool enabled = option.state & QStyle::State_Enabled;

    if (selected) {
        // Draw selected background with rounded corners
        CFColor containerColor = primaryContainerColor();
        QColor bgColor = containerColor.native_color();
        bgColor.setAlphaF(0.12f); // 12% opacity

        QPainterPath path = roundedRect(itemRect, 8.0f); // ShapeSmall
        painter->fillPath(path, bgColor);
    } else if (hovered && enabled) {
        // Draw hover state
        CFColor onSurface = onSurfaceColor();
        QColor hoverColor = onSurface.native_color();
        hoverColor.setAlphaF(0.08f); // 8% opacity

        QPainterPath path = roundedRect(itemRect, 8.0f);
        painter->fillPath(path, hoverColor);
    }
}

void TreeViewItemDelegate::drawTreeLines(QPainter* painter, const QModelIndex& index,
                                         const QRectF& itemRect, const QTreeView* view) const {
    if (!m_showTreeLines || m_indentStyle != TreeIndentStyle::Material) {
        return;
    }

    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float lineThickness = helper.dpToPx(1.0f);
    float indent = m_indentPx;
    float hPadding = horizontalPadding();
    float iconSz = iconSize();

    CFColor lineColor = outlineVariantColor();
    QColor lineQtColor = lineColor.native_color();
    lineQtColor.setAlphaF(0.12f); // 12% opacity for tree lines

    painter->save();
    painter->setPen(QPen(lineQtColor, lineThickness));
    painter->setRenderHint(QPainter::Antialiasing, false);

    int depth = getDepth(index, view);

    // Draw vertical lines for each ancestor level
    // Only draw lines that should continue (not for last siblings)
    for (int level = 0; level < depth; level++) {
        if (!shouldContinueLineAtLevel(index, level, view)) {
            continue;
        }

        float x = hPadding + (indent * level) + iconSz / 2.0f;
        float y1 = itemRect.top();
        float y2 = itemRect.bottom();
        painter->drawLine(QPointF(x, y1), QPointF(x, y2));
    }

    // Draw horizontal connector line to current item
    if (depth > 0) {
        float x1 = hPadding + (indent * (depth - 1)) + iconSz / 2.0f;
        float x2 = hPadding + (indent * depth) + iconSz / 2.0f;
        float y = itemRect.center().y();
        painter->drawLine(QPointF(x1, y), QPointF(x2, y));
    }

    painter->restore();
}

void TreeViewItemDelegate::drawExpandCollapseIcon(QPainter* painter, const QRectF& iconRect,
                                                  bool expanded, const CFColor& color) const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float strokeThickness = helper.dpToPx(2.0f);

    painter->setPen(QPen(color.native_color(), strokeThickness));
    painter->setRenderHint(QPainter::Antialiasing);

    QPointF center = iconRect.center();
    float iconSz = iconSize();
    float halfSize = iconSz * 0.25f;

    // Draw chevron (right arrow pointing down when expanded)
    QPainterPath path;
    if (expanded) {
        // Downward chevron
        path.moveTo(center.x() - halfSize, center.y() - halfSize / 2.0f);
        path.lineTo(center.x(), center.y() + halfSize / 2.0f);
        path.lineTo(center.x() + halfSize, center.y() - halfSize / 2.0f);
    } else {
        // Right chevron
        path.moveTo(center.x() - halfSize / 2.0f, center.y() - halfSize);
        path.lineTo(center.x() + halfSize / 2.0f, center.y());
        path.lineTo(center.x() - halfSize / 2.0f, center.y() + halfSize);
    }

    painter->drawPath(path);
}

void TreeViewItemDelegate::drawCheckBox(QPainter* painter, const QRectF& checkRect,
                                        Qt::CheckState state,
                                        const QStyleOptionViewItem& option) const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float strokeThickness = helper.dpToPx(2.0f);
    float cornerRadius = helper.dpToPx(2.0f);

    painter->setRenderHint(QPainter::Antialiasing);

    // Get checkbox color
    CFColor boxColor = onSurfaceVariantColor();
    CFColor checkColorCf = onSurfaceColor();

    bool enabled = option.state & QStyle::State_Enabled;
    QColor borderColor = boxColor.native_color();
    QColor checkColor = checkColorCf.native_color();

    if (!enabled) {
        borderColor.setAlpha(100);
        checkColor.setAlpha(100);
    }

    // Draw checkbox border
    QPainterPath boxPath;
    boxPath.addRoundedRect(checkRect, cornerRadius, cornerRadius);

    painter->setPen(QPen(borderColor, strokeThickness));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(boxPath);

    // Draw checkmark or partial state
    if (state == Qt::Checked) {
        // Draw checkmark
        QPainterPath checkPath;
        float w = checkRect.width();
        float h = checkRect.height();
        float inset = helper.dpToPx(4.0f);
        float midY = checkRect.center().y();

        // Checkmark path
        checkPath.moveTo(checkRect.left() + inset, midY);
        checkPath.lineTo(checkRect.center().x() - helper.dpToPx(1.0f), checkRect.bottom() - inset);
        checkPath.lineTo(checkRect.right() - inset, checkRect.top() + inset);

        painter->setPen(
            QPen(checkColor, strokeThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawPath(checkPath);
    } else if (state == Qt::PartiallyChecked) {
        // Draw minus sign for partial state
        float hPadding = helper.dpToPx(4.0f);
        float lineY = checkRect.center().y();
        painter->setPen(QPen(checkColor, strokeThickness, Qt::SolidLine, Qt::RoundCap));
        painter->drawLine(QPointF(checkRect.left() + hPadding, lineY),
                          QPointF(checkRect.right() - hPadding, lineY));
    }
}

void TreeViewItemDelegate::drawItemContent(QPainter* painter, const QRectF& contentRect,
                                           const QStyleOptionViewItem& option,
                                           const QModelIndex& index) const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    float iconSz = helper.dpToPx(24.0f);
    float iconGap = helper.dpToPx(12.0f);

    // Get text color - check for custom foreground color first
    QVariant fgVar = index.data(Qt::ForegroundRole);
    QColor textColor;
    bool enabled = option.state & QStyle::State_Enabled;

    if (fgVar.isValid()) {
        QBrush fgBrush = fgVar.value<QBrush>();
        textColor = fgBrush.color();
        if (!enabled) {
            textColor.setAlphaF(textColor.alphaF() * 0.38f);
        }
    } else {
        CFColor defaultColor = onSurfaceColor();
        textColor = defaultColor.native_color();
        if (!enabled) {
            textColor.setAlphaF(0.38f);
        }
    }
    painter->setPen(textColor);

    // Get font - check for custom font first
    QFont font = painter->font();
    QVariant fontVar = index.data(Qt::FontRole);
    if (fontVar.isValid()) {
        font = fontVar.value<QFont>();
    } else {
        font.setPixelSize(static_cast<int>(helper.dpToPx(14.0f)));
    }
    painter->setFont(font);

    // Check for decoration icon
    QVariant decoration = index.data(Qt::DecorationRole);
    bool hasIcon = false;
    if (decoration.isValid()) {
        QIcon icon = decoration.value<QIcon>();
        hasIcon = !icon.isNull();
    }

    // Get text alignment - check for custom alignment
    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;
    QVariant alignVar = index.data(Qt::TextAlignmentRole);
    if (alignVar.isValid()) {
        alignment = static_cast<Qt::Alignment>(alignVar.toInt());
    }

    // Calculate available width for text
    float availableTextWidth = contentRect.width();
    float currentX = contentRect.left();
    if (hasIcon && (alignment & Qt::AlignLeft)) {
        availableTextWidth -= (iconSz + iconGap);
    }

    // Get display text
    QString text = index.data(Qt::DisplayRole).toString();

    QFontMetrics fm(font);

    // Draw decoration icon first (left side)
    if (hasIcon && (alignment & Qt::AlignLeft)) {
        QIcon icon = decoration.value<QIcon>();
        QRectF iconRect(currentX, contentRect.center().y() - iconSz / 2.0f, iconSz, iconSz);
        icon.paint(painter, iconRect.toRect());
        currentX += iconSz + iconGap;
        availableTextWidth = contentRect.right() - currentX;
    }

    // Draw text - NOT elided, show full text
    QRectF textRect(currentX, contentRect.top(), availableTextWidth, contentRect.height());
    painter->drawText(textRect, alignment, text);

    // Draw decoration icon on right side if right-aligned
    if (hasIcon && (alignment & Qt::AlignRight)) {
        QIcon icon = decoration.value<QIcon>();
        float textWidth = fm.horizontalAdvance(text);
        float iconX = contentRect.left() + textWidth + iconGap;
        QRectF iconRect(iconX, contentRect.center().y() - iconSz / 2.0f, iconSz, iconSz);
        icon.paint(painter, iconRect.toRect());
    }
}

// ============================================================================
// Tree Structure Helpers
// ============================================================================

int TreeViewItemDelegate::getDepth(const QModelIndex& index, const QTreeView* view) const {
    if (!index.isValid()) {
        return 0;
    }

    // Count ancestors by walking up the model hierarchy
    // This is more reliable than using visualRect.x() which can be affected
    // by Qt's internal layout calculations
    int depth = 0;
    QModelIndex current = index;
    while (current.isValid()) {
        current = current.parent();
        if (current.isValid()) {
            depth++;
        }
    }
    return depth;
}

bool TreeViewItemDelegate::isLastSibling(const QModelIndex& index, const QTreeView* view) const {
    if (!index.isValid() || !view) {
        return true;
    }

    QModelIndex parent = index.parent();
    // Use custom property instead of view->rootIsDecorated() (non-virtual)
    bool rootDecorated = view->property("_q_rootIsDecorated").toBool();
    if (!parent.isValid() && !rootDecorated) {
        // For root items without decoration, consider row count
        const QAbstractItemModel* model = index.model();
        if (model && index.row() == model->rowCount() - 1) {
            return true;
        }
        return false;
    }

    // Check if this is the last child of its parent
    if (!parent.isValid()) {
        // Top level item
        const QAbstractItemModel* model = index.model();
        if (!model) {
            return true;
        }
        return index.row() == model->rowCount() - 1;
    }

    return index.row() == parent.model()->rowCount(parent) - 1;
}

bool TreeViewItemDelegate::shouldContinueLineAtLevel(const QModelIndex& index, int level,
                                                     const QTreeView* view) const {
    // Determine if the tree line should continue at this level
    // The line continues if the ancestor at this level is NOT the last sibling

    if (!index.isValid() || !view) {
        return false;
    }

    // Walk up the tree to find the ancestor at the given level
    QModelIndex current = index;
    int currentDepth = getDepth(index, view);

    // We need to go up (currentDepth - level) levels
    int levelsUp = currentDepth - level;
    if (levelsUp <= 0) {
        return false;
    }

    for (int i = 0; i < levelsUp && current.isValid(); i++) {
        current = current.parent();
    }

    if (!current.isValid()) {
        return false;
    }

    // The line continues if this ancestor is NOT the last sibling
    return !isLastSibling(current, view);
}

// ============================================================================
// Layout Helpers
// ============================================================================

float TreeViewItemDelegate::horizontalPadding() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(16.0f);
}

float TreeViewItemDelegate::iconSize() const {
    CanvasUnitHelper helper(qApp->devicePixelRatio());
    return helper.dpToPx(24.0f);
}

// ============================================================================
// Color Access Methods
// ============================================================================

CFColor TreeViewItemDelegate::surfaceColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(SURFACE));
    } catch (...) {
        return fallbackSurface();
    }
}

CFColor TreeViewItemDelegate::onSurfaceColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurface();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ON_SURFACE));
    } catch (...) {
        return fallbackOnSurface();
    }
}

CFColor TreeViewItemDelegate::onSurfaceVariantColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOnSurfaceVariant();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(ON_SURFACE_VARIANT));
    } catch (...) {
        return fallbackOnSurfaceVariant();
    }
}

CFColor TreeViewItemDelegate::primaryContainerColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackPrimaryContainer();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(PRIMARY_CONTAINER));
    } catch (...) {
        return fallbackPrimaryContainer();
    }
}

CFColor TreeViewItemDelegate::outlineVariantColor() const {
    auto* app = Application::instance();
    if (!app) {
        return fallbackOutlineVariant();
    }

    try {
        const auto& theme = app->currentTheme();
        auto& colorScheme = theme.color_scheme();
        return CFColor(colorScheme.queryColor(OUTLINE_VARIANT));
    } catch (...) {
        return fallbackOutlineVariant();
    }
}

// ============================================================================
// Public Accessors for TreeView Hit Testing
// ============================================================================

float TreeViewItemDelegate::horizontalPaddingValue() const {
    return horizontalPadding();
}

float TreeViewItemDelegate::iconSizeValue() const {
    return iconSize();
}

} // namespace cf::ui::widget::material
