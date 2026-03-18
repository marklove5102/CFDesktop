/**
 * @file tabview.cpp
 * @brief Material Design 3 TabView Implementation
 *
 * @author Material Design Framework Team
 * @date 2026-03-18
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "tabview.h"
#include "private/materialtabbar.h"

#include <QTabWidget>

namespace cf::ui::widget::material {

namespace {
constexpr int DEFAULT_TAB_HEIGHT_DP = 48;
constexpr int DEFAULT_TAB_MIN_WIDTH_DP = 120;
} // namespace

// ============================================================================
// TabView Implementation
// ============================================================================

TabView::TabView(QWidget* parent) : QTabWidget(parent), m_tabBar(nullptr) {

    m_tabBar = new MaterialTabBar(this);
    setTabBar(m_tabBar);

    setTabPosition(QTabWidget::North);
    setDocumentMode(true);

    // Connect close button signal
    connect(m_tabBar, &MaterialTabBar::tabCloseRequested, this, [this](int index) {
        removeTab(index);
        emit tabCloseRequested(index);
    });
}

TabView::~TabView() {
    // TabBar is parented to this, Qt will delete it
}

int TabView::tabHeight() const {
    return m_tabBar ? m_tabBar->tabHeightDp() : DEFAULT_TAB_HEIGHT_DP;
}

void TabView::setTabHeight(int height) {
    if (m_tabBar) {
        m_tabBar->setTabHeightDp(height);
    }
}

int TabView::tabMinWidth() const {
    return m_tabBar ? m_tabBar->tabMinWidthDp() : DEFAULT_TAB_MIN_WIDTH_DP;
}

void TabView::setTabMinWidth(int width) {
    if (m_tabBar) {
        m_tabBar->setTabMinWidthDp(width);
    }
}

bool TabView::showIndicator() const {
    return m_tabBar ? m_tabBar->showIndicator() : true;
}

void TabView::setShowIndicator(bool show) {
    if (m_tabBar) {
        m_tabBar->setShowIndicator(show);
    }
}

void TabView::setTabCloseable(int index, bool closeable) {
    if (m_tabBar) {
        m_tabBar->setTabCloseable(index, closeable);
    }
}

QSize TabView::sizeHint() const {
    QSize contentSize = QTabWidget::sizeHint();
    int tabBarHeight = m_tabBar ? m_tabBar->sizeHint().height() : 48;
    return QSize(contentSize.width(), contentSize.height() + tabBarHeight);
}

} // namespace cf::ui::widget::material
