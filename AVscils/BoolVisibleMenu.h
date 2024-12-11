#pragma once

struct BoolVisibleMenu
{
	bool isVisibleMenu{ false };
	bool isVisibleMainMenu{ true };
	bool isVisibleEngineFile{ false };
	bool isVisibleGuide{ false };
};

extern BoolVisibleMenu G_BOOL_VISIBLE;