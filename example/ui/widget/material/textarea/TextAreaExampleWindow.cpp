/**
 * @file TextAreaExampleWindow.cpp
 * @brief Material Design 3 TextArea Example - Implementation
 */

#include "TextAreaExampleWindow.h"

#include "ui/widget/material/widget/textarea/textarea.h"

#include <QApplication>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

using namespace cf::ui::widget::material;
using TextAreaVariant = cf::ui::widget::material::TextArea::TextAreaVariant;

namespace cf::ui::example {

TextAreaExampleWindow::TextAreaExampleWindow(QWidget* parent) : QMainWindow(parent) {

    setupUI();
    createHeader();
    createTextAreaVariantsSection();
    createTextAreaStatesSection();
    createCharacterCounterSection();
    createMinMaxLinesSection();
    createInteractionDemoSection();

    // Let window auto-adjust to content size
    adjustSize();
}

TextAreaExampleWindow::~TextAreaExampleWindow() = default;

void TextAreaExampleWindow::setupUI() {
    // Window setup
    setWindowTitle("Material TextArea Example");
    resize(900, 800);
    setMinimumSize(600, 500);

    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(24, 24, 24, 24);
    mainLayout_->setSpacing(16);

    // Scroll area for content
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setSpacing(24);
    scrollLayout_->setContentsMargins(0, 0, 0, 0);

    scrollArea_->setWidget(scrollContent_);
    mainLayout_->addWidget(scrollArea_);
}

void TextAreaExampleWindow::createHeader() {
    // Title
    titleLabel_ = new QLabel("Material Design 3 Text Areas", this);
    QFont titleFont("Segoe UI", 24, QFont::Bold);
    titleLabel_->setFont(titleFont);
    scrollLayout_->addWidget(titleLabel_);

    // Subtitle
    subtitleLabel_ = new QLabel(
        "Explore Material Design 3 text area variants with floating labels, "
        "character counters, and helper text.",
        this);
    QFont subtitleFont("Segoe UI", 11);
    subtitleLabel_->setFont(subtitleFont);
    subtitleLabel_->setWordWrap(true);
    scrollLayout_->addWidget(subtitleLabel_);

    // Add some spacing
    scrollLayout_->addSpacing(16);
}

void TextAreaExampleWindow::createTextAreaVariantsSection() {
    QGroupBox* groupBox = new QGroupBox("TextArea Variants (文本域变体)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(16);
    layout->setContentsMargins(16, 24, 16, 16);

    // Filled TextArea
    QHBoxLayout* filledLayout = new QHBoxLayout();
    QLabel* filledLabel = new QLabel("Filled:", this);
    filledLabel->setMinimumWidth(100);
    TextArea* filledTextArea = new TextArea(TextAreaVariant::Filled, this);
    filledTextArea->setLabel("Label");
    filledTextArea->setPlaceholderText("Enter text here...");
    filledTextArea->setMinLines(3);
    filledLayout->addWidget(filledLabel);
    filledLayout->addWidget(filledTextArea, 1);
    layout->addLayout(filledLayout);

    // Outlined TextArea
    QHBoxLayout* outlinedLayout = new QHBoxLayout();
    QLabel* outlinedLabel = new QLabel("Outlined:", this);
    outlinedLabel->setMinimumWidth(100);
    TextArea* outlinedTextArea = new TextArea(TextAreaVariant::Outlined, this);
    outlinedTextArea->setLabel("Label");
    outlinedTextArea->setPlaceholderText("Enter text here...");
    outlinedTextArea->setMinLines(3);
    outlinedLayout->addWidget(outlinedLabel);
    outlinedLayout->addWidget(outlinedTextArea, 1);
    layout->addLayout(outlinedLayout);

    scrollLayout_->addWidget(groupBox);
}

void TextAreaExampleWindow::createTextAreaStatesSection() {
    QGroupBox* groupBox = createGroupBox("TextArea States (文本域状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled) text areas
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(100);
    TextArea* normalTextArea = new TextArea("Enabled text area", TextAreaVariant::Filled, this);
    normalTextArea->setLabel("Message");
    normalTextArea->setMinLines(2);
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalTextArea, 1);
    layout->addLayout(normalLayout);

    // Disabled text areas
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(100);
    TextArea* disabledFilled = new TextArea("Disabled filled text area", TextAreaVariant::Filled, this);
    disabledFilled->setLabel("Message");
    disabledFilled->setEnabled(false);
    disabledFilled->setMinLines(2);
    TextArea* disabledOutlined = new TextArea("Disabled outlined text area", TextAreaVariant::Outlined, this);
    disabledOutlined->setLabel("Message");
    disabledOutlined->setEnabled(false);
    disabledOutlined->setMinLines(2);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledFilled, 1);
    disabledLayout->addWidget(disabledOutlined, 1);
    layout->addLayout(disabledLayout);

    // Error state
    QHBoxLayout* errorLayout = new QHBoxLayout();
    QLabel* errorLabel = new QLabel("Error:", this);
    errorLabel->setMinimumWidth(100);
    TextArea* errorTextArea = new TextArea(TextAreaVariant::Outlined, this);
    errorTextArea->setLabel("Email");
    errorTextArea->setErrorText("Please enter a valid email address");
    errorTextArea->setMinLines(2);
    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(errorTextArea, 1);
    layout->addLayout(errorLayout);

    scrollLayout_->addWidget(groupBox);
}

void TextAreaExampleWindow::createCharacterCounterSection() {
    QGroupBox* groupBox = createGroupBox("Character Counter (字符计数器)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // TextArea with character counter
    QHBoxLayout* counterLayout = new QHBoxLayout();
    QLabel* counterLabel = new QLabel("With Counter:", this);
    counterLabel->setMinimumWidth(100);
    TextArea* counterTextArea = new TextArea(TextAreaVariant::Filled, this);
    counterTextArea->setLabel("Bio");
    counterTextArea->setPlaceholderText("Tell us about yourself...");
    counterTextArea->setMaxLength(150);
    counterTextArea->setShowCharacterCounter(true);
    counterTextArea->setMinLines(3);
    counterLayout->addWidget(counterLabel);
    counterLayout->addWidget(counterTextArea, 1);
    layout->addLayout(counterLayout);

    // Connect text changed to update counter display
    connect(counterTextArea, &QTextEdit::textChanged, this, &TextAreaExampleWindow::onFilledTextChanged);

    scrollLayout_->addWidget(groupBox);
}

void TextAreaExampleWindow::createMinMaxLinesSection() {
    QGroupBox* groupBox = createGroupBox("Min/Max Lines (最小/最大行数)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Fixed 3 lines
    QHBoxLayout* fixed3Layout = new QHBoxLayout();
    QLabel* fixed3Label = new QLabel("Fixed 3 lines:", this);
    fixed3Label->setMinimumWidth(100);
    TextArea* fixed3TextArea = new TextArea(TextAreaVariant::Outlined, this);
    fixed3TextArea->setLabel("Short description");
    fixed3TextArea->setMinLines(3);
    fixed3TextArea->setMaxLines(3);
    fixed3TextArea->setPlaceholderText("Exactly 3 lines visible...");
    fixed3Layout->addWidget(fixed3Label);
    fixed3Layout->addWidget(fixed3TextArea, 1);
    layout->addLayout(fixed3Layout);

    // Auto-grow (1-5 lines)
    QHBoxLayout* autoGrowLayout = new QHBoxLayout();
    QLabel* autoGrowLabel = new QLabel("Auto-grow (1-5):", this);
    autoGrowLabel->setMinimumWidth(100);
    TextArea* autoGrowTextArea = new TextArea(TextAreaVariant::Filled, this);
    autoGrowTextArea->setLabel("Description");
    autoGrowTextArea->setMinLines(1);
    autoGrowTextArea->setMaxLines(5);
    autoGrowTextArea->setPlaceholderText("Starts at 1 line, grows up to 5 lines...");
    autoGrowLayout->addWidget(autoGrowLabel);
    autoGrowLayout->addWidget(autoGrowTextArea, 1);
    layout->addLayout(autoGrowLayout);

    // Multi-line (5 lines min)
    QHBoxLayout* multiLineLayout = new QHBoxLayout();
    QLabel* multiLineLabel = new QLabel("Multi-line (5+):", this);
    multiLineLabel->setMinimumWidth(100);
    TextArea* multiLineTextArea = new TextArea(TextAreaVariant::Outlined, this);
    multiLineTextArea->setLabel("Comments");
    multiLineTextArea->setHelperText("Share your thoughts with us");
    multiLineTextArea->setMinLines(5);
    multiLineTextArea->setPlaceholderText("Type your message here...");
    multiLineLayout->addWidget(multiLineLabel);
    multiLineLayout->addWidget(multiLineTextArea, 1);
    layout->addLayout(multiLineLayout);

    scrollLayout_->addWidget(groupBox);
}

void TextAreaExampleWindow::createInteractionDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interaction Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Live character count demo
    QHBoxLayout* demoLayout = new QHBoxLayout();
    QLabel* demoLabel = new QLabel("Type to test:", this);
    demoLabel->setMinimumWidth(100);
    TextArea* demoTextArea = new TextArea(TextAreaVariant::Filled, this);
    demoTextArea->setLabel("Your Message");
    demoTextArea->setMaxLength(200);
    demoTextArea->setShowCharacterCounter(true);
    demoTextArea->setMinLines(4);
    demoTextArea->setPlaceholderText("Start typing to see the character counter update...");
    demoLayout->addWidget(demoLabel);
    demoLayout->addWidget(demoTextArea, 1);
    layout->addLayout(demoLayout);

    // Character count display
    QHBoxLayout* countDisplayLayout = new QHBoxLayout();
    QLabel* countLabel = new QLabel("Character count:", this);
    countLabel->setMinimumWidth(100);
    charCountLabel_ = new QLabel("0 / 200", this);
    QFont countFont("Segoe UI", 12, QFont::Bold);
    charCountLabel_->setFont(countFont);
    countDisplayLayout->addWidget(countLabel);
    countDisplayLayout->addWidget(charCountLabel_);
    countDisplayLayout->addStretch();
    layout->addLayout(countDisplayLayout);

    // Connect text changed signal
    connect(demoTextArea, &QTextEdit::textChanged, this, &TextAreaExampleWindow::onDemoTextChanged);

    scrollLayout_->addWidget(groupBox);

    // Add stretch at the end
    scrollLayout_->addStretch();
}

void TextAreaExampleWindow::onFilledTextChanged() {
    // This can be used for additional filled text area handling
}

void TextAreaExampleWindow::onOutlinedTextChanged() {
    // This can be used for additional outlined text area handling
}

void TextAreaExampleWindow::onDemoTextChanged() {
    // Update character count display
    TextArea* textArea = qobject_cast<TextArea*>(sender());
    if (textArea && charCountLabel_) {
        int count = textArea->toPlainText().length();
        int max = textArea->maxLength();
        charCountLabel_->setText(QString("%1 / %2").arg(count).arg(max));
    }
}

QGroupBox* TextAreaExampleWindow::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
