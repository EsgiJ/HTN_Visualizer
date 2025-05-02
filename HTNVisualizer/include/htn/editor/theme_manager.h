#pragma once

#include <imgui.h>

namespace HTN::Editor
{
	enum class Theme
	{
		Dark,
		Light
	};

	struct ThemeColors
	{
		ImColor background;
		ImColor grid;
		ImColor link;
		ImColor nodeBackground;
		ImColor nodeText;
	};

	class ThemeManager
	{
	public:
		static void ApplyTheme(Theme theme);
		static ThemeColors GetColors();

	private:
		static Theme s_CurrentTheme;
	};
}