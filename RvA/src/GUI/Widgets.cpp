#include "Widgets.h"

Screen::Screen(const char *name) : m_name(name)
{
    m_rootNode.type = WidgetType::Root;
}

WidgetHandle Screen::create(UIButton button, WidgetHandle* handleResult) {
    auto handle = m_buttonPool.createItem(std::move(button));
    m_buttonPool.getItem(handle)->handle = handle;

    if (handleResult) {
        *handleResult = handle;
    }

    return handle;
}

WidgetHandle Screen::create(UIText text, WidgetHandle* handleResult) {
    auto handle = m_textPool.createItem(std::move(text));
    m_textPool.getItem(handle)->handle = handle;

    if (handleResult) {
        *handleResult = handle;
    }

    return handle;
}

WidgetHandle Screen::create(UIStack stack) {
    auto handle = m_stackPool.createItem(std::move(stack));
    m_stackPool.getItem(handle)->handle = handle;
    return handle;
}

WidgetHandle Screen::create(UISpace space) {
    auto handle = m_spacePool.createItem(std::move(space));
    m_spacePool.getItem(handle)->handle = handle;
    return handle;
}

ScreenBuilder::ScreenBuilder(Screen &screen) : m_screen(screen) {
    m_nodeStack.push(&m_screen.getRootNode());
}

ScreenBuilder &ScreenBuilder::stack(UIStack stack) {
    auto handle = m_screen.create(stack);

    auto node = std::make_unique<UINode>();
    node->handle = handle;
    node->type = WidgetType::Stack;
    node->parent = m_nodeStack.top();
    auto nodePtr = node.get();
    m_nodeStack.top()->children.push_back(std::move(node));
    m_nodeStack.push(nodePtr);

    return *this;
}

ScreenBuilder& ScreenBuilder::end() {
    assert(m_nodeStack.size() > 1 && "Cannot end root stack");
    m_nodeStack.pop();
    return *this;
}

ScreenBuilder& ScreenBuilder::button(UIButton button, WidgetHandle* handleResult) {
    auto handle = m_screen.create(button, handleResult);

    auto node = std::make_unique<UINode>();
    node->handle = handle;
    node->type = WidgetType::Button;
    node->parent = m_nodeStack.top();
    m_nodeStack.top()->children.push_back(std::move(node));

    return *this;
}

ScreenBuilder& ScreenBuilder::text(UIText text, WidgetHandle* handleResult) {
    auto handle = m_screen.create(text, handleResult);

    auto node = std::make_unique<UINode>();
    node->handle = handle;
    node->type = WidgetType::Text;
    node->parent = m_nodeStack.top();
    m_nodeStack.top()->children.push_back(std::move(node));

    return *this;
}

ScreenBuilder &ScreenBuilder::space(UISpace space) {
    auto handle = m_screen.create(space);

    auto node = std::make_unique<UINode>();
    node->handle = handle;
    node->type = WidgetType::Space;
    node->parent = m_nodeStack.top();
    m_nodeStack.top()->children.push_back(std::move(node));
    return *this;
}
