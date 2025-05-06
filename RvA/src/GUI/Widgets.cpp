#include "Widgets.h"

#include "constants.h"

Screen::Screen(const char* name) : m_name(name) {
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

WidgetHandle Screen::create(UIShape shape, WidgetHandle* handleResult) {
	auto handle = m_shapePool.createItem(std::move(shape));
	m_shapePool.getItem(handle)->handle = handle;
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

WidgetHandle Screen::create(UICustom custom) {
	auto handle = m_customPool.createItem(std::move(custom));
	m_customPool.getItem(handle)->handle = handle;
	return handle;
}

WidgetHandle Screen::create(UIBorder border, WidgetHandle* handleResult) {
	auto handle = m_borderPool.createItem(std::move(border));
	m_borderPool.getItem(handle)->handle = handle;
	if (handleResult) {
		*handleResult = handle;
	}
	return handle;
}

ScreenBuilder::ScreenBuilder(Screen& screen) : m_screen(screen) {
	m_nodeStack.push(&m_screen.getRootNode());
}

ScreenBuilder& ScreenBuilder::stack(UIStack stack) {
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

ScreenBuilder& ScreenBuilder::vertical_stack(float padding, float size, VAlign vAlignment, HAlign hAlignment, ContentAlign cAlignment) {
	return stack({.orientation = GUIOrientation::Vertical, .padding = {0, padding}, .hAlign = hAlignment, .vAlign = vAlignment, .size = {size, autoSize}, .contentAlignment = cAlignment});
}

ScreenBuilder& ScreenBuilder::horizontal_stack(float padding, float size, HAlign hAlignment, VAlign vAlignment, ContentAlign cAlignment) {
	return stack({.orientation = GUIOrientation::Horizontal, .padding = {padding, 0}, .hAlign = hAlignment, .vAlign = vAlignment, .size = {autoSize, size}, .contentAlignment = cAlignment});
}

ScreenBuilder& ScreenBuilder::end() {
	assert(m_nodeStack.size() > 1 && "Cannot end root stack");
	m_nodeStack.pop();
	return *this;
}

ScreenBuilder& ScreenBuilder::button(UIButton button, WidgetHandle* handleResult) {
	auto handle = m_screen.create(std::move(button), handleResult);

	auto node = std::make_unique<UINode>();
	node->handle = handle;
	node->type = WidgetType::Button;
	node->parent = m_nodeStack.top();
	m_nodeStack.top()->children.push_back(std::move(node));

	return *this;
}

ScreenBuilder& ScreenBuilder::text(UIText text, WidgetHandle* handleResult) {
	auto handle = m_screen.create(std::move(text), handleResult);

	auto node = std::make_unique<UINode>();
	node->handle = handle;
	node->type = WidgetType::Text;
	node->parent = m_nodeStack.top();
	m_nodeStack.top()->children.push_back(std::move(node));

	return *this;
}

ScreenBuilder& ScreenBuilder::small_text(UIText text, WidgetHandle* handleResult) {
	text.fontSize = FONT_SMALL;
	return this->text(std::move(text), handleResult);
}

ScreenBuilder& ScreenBuilder::medium_text(UIText text, WidgetHandle* handleResult) {
	text.fontSize = FONT_MEDIUM;
	return this->text(std::move(text), handleResult);
}

ScreenBuilder& ScreenBuilder::big_text(UIText text, WidgetHandle* handleResult) {
	text.fontSize = FONT_BIG;
	return this->text(std::move(text), handleResult);
}

ScreenBuilder& ScreenBuilder::shape(UIShape shape, WidgetHandle* handleResult) {
	auto handle = m_screen.create(std::move(shape), handleResult);

	auto node = std::make_unique<UINode>();
	node->handle = handle;
	node->type = WidgetType::Shape;
	node->parent = m_nodeStack.top();
	m_nodeStack.top()->children.push_back(std::move(node));
	return *this;
}

ScreenBuilder& ScreenBuilder::rect(const Rectangle& rect, Color color, WidgetHandle* handleResult) {
	auto shapeData = UIShape{{rect.x, rect.y}, {rect.width, rect.height}, color, ShapeType::Rectangle};
	return shape(shapeData);
}

ScreenBuilder& ScreenBuilder::space(UISpace space) {
	auto handle = m_screen.create(std::move(space));

	auto node = std::make_unique<UINode>();
	node->handle = handle;
	node->type = WidgetType::Space;
	node->parent = m_nodeStack.top();
	m_nodeStack.top()->children.push_back(std::move(node));
	return *this;
}

ScreenBuilder& ScreenBuilder::custom(UICustom custom) {
	auto handle = m_screen.create(std::move(custom));

	auto node = std::make_unique<UINode>();
	node->handle = handle;
	node->type = WidgetType::Custom;
	node->parent = m_nodeStack.top();
	m_nodeStack.top()->children.push_back(std::move(node));
	return *this;
}

ScreenBuilder& ScreenBuilder::border(UIBorder border, WidgetHandle* handleResult) {
	auto handle = m_screen.create(std::move(border), handleResult);

	auto node = std::make_unique<UINode>();
	node->handle = handle;
	node->type = WidgetType::Border;
	node->parent = m_nodeStack.top();
	auto nodePtr = node.get();
	m_nodeStack.top()->children.push_back(std::move(node));
	m_nodeStack.push(nodePtr);
	return *this;
}
