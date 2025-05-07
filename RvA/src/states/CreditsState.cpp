#include "CreditsState.h"

#include "Game.h"

#include <raylib.h>
#include <string>
#include <vector>

struct CreditsItem {
	std::string label;
	Color color;
};

// TODO(Gerark) - This list could be defined in data ( maybe a json file to easily edit it without recompiling )
const std::vector<CreditsItem> creditsItems{
	{"PROGRAMMING", WHITE},
	{"Erzis", DARKGRAY},
	{"Zazeraz", DARKGRAY},
	{"Gerark", DARKGRAY},
	{},
	{"PIXEL ART", WHITE},
	{"Marvin Gorner", DARKGRAY},
	{},
	{"CONCEPT ART", WHITE},
	{"yeahno", DARKGRAY},
	{},
	{"MUSIC", WHITE},
	{"Trim", DARKGRAY},
	{"POG LIZARD", DARKGRAY},
};

CreditsState::CreditsState(Game& game) : m_game(game) {}

flow::FsmAction CreditsState::enter() {
	auto builder = m_game.getGUI().buildScreen("Credits");
	builder.vertical_stack(2, 100.f);

	builder.medium_text({.text = "CREDITS", .color = WHITE, .hAlign = HAlign::Center});

	for (auto& item : creditsItems) {
		if (!item.label.empty()) {
			builder.small_text({.text = item.label.c_str(), .color = item.color, .hAlign = HAlign::Center});
		} else {
			builder.space({0, 10.f});
		}
	}

	builder.space({0, 20.f});
	builder.button({"Back", {}, {autoSize, 40.f}, [this]() { m_nextTransition = "back"; }});
	builder.end();

	return flow::FsmAction::none();
}

flow::FsmAction CreditsState::update(float dt) {
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	return flow::FsmAction::none();
}

void CreditsState::exit() {
	m_game.getGUI().destroyScreen("Credits");
}
