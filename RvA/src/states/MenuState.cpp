#include "MenuState.h"
#include "Game.h"
#include "states/PlayState.h"
#include "states/CreditsState.h"

MenuState::MenuState(Game& game)
	:btnSize({ 200.f, 60.f })
{}

void MenuState::draw(Game& game)
{
	const auto halfBtnSize = btnSize.y / 2;
	const auto halfRenderTargetY = game.getTexSize().y / 2;
	if (GuiButton({ game.getTexSize().x / 2 - btnSize.x / 2, (halfRenderTargetY + btnSize.y) - halfBtnSize, btnSize.x, btnSize.y }, "Exit"))
	{
		exit(0);
	}

	if (GuiButton({ game.getTexSize().x / 2 - btnSize.x / 2, (halfRenderTargetY) - halfBtnSize, btnSize.x, btnSize.y }, "Credits"))
	{
		game.setState(std::make_unique<CreditsState>());
	}

	if (GuiButton({ game.getTexSize().x / 2 - btnSize.x / 2, (halfRenderTargetY - btnSize.y) - halfBtnSize, btnSize.x, btnSize.y }, "Play"))
	{
		game.getGUI().reset();
		game.setState(std::make_unique<PlayState>(game));
	}
}