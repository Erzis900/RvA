#include "MenuState.h"
#include "Game.h"
#include "states/PlayState.h"
#include "states/CreditsState.h"

MenuState::MenuState()
	:btnSize({ 200.f, 60.f })
{}

void MenuState::draw(Game& game)
{
	const auto halfBtnSize = btnSize.y / 2;
	const auto halfRenderTargetY = TEX_HEIGHT / 2;
	if (GuiButton({TEX_WIDTH / 2 - btnSize.x / 2, (halfRenderTargetY + btnSize.y) - halfBtnSize, btnSize.x, btnSize.y }, "Exit"))
	{
		exit(0);
	}

	if (GuiButton({TEX_WIDTH / 2 - btnSize.x / 2, (halfRenderTargetY) - halfBtnSize, btnSize.x, btnSize.y }, "Credits"))
	{
		game.setState(std::make_unique<CreditsState>());
	}

	if (GuiButton({TEX_WIDTH / 2 - btnSize.x / 2, (halfRenderTargetY - btnSize.y) - halfBtnSize, btnSize.x, btnSize.y }, "Play"))
	{
		game.getGUI().reset();
		game.setState(std::make_unique<PlayState>(game));
	}
}