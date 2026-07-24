#include "imgui_utils.h"
#include "theme_manager.h"

namespace HTN::Editor
{
	Theme ThemeManager::s_CurrentTheme = Theme::Light;

	void HTN::Editor::ThemeManager::ApplyTheme(Theme theme)
	{
		s_CurrentTheme = theme;

		if (theme == Theme::Dark) 
		{
			SetupImGuiStyle(true, 1);
		}
		else 
		{
			SetupImGuiStyle(false, 1);
		}
	}

	ThemeColors HTN::Editor::ThemeManager::GetColors()
	{
		if (s_CurrentTheme == Theme::Dark) 
		{
			return {
				ImColor(30, 30, 30),		// background
				ImColor(60, 60, 60, 100),	// grid
				ImColor(255, 255, 255),		// link
				ImColor(50, 50, 50),		// nodeBackground
				ImColor(230, 230, 230)		// nodeText
			};
		}
		else
		{
			return {
				ImColor(240, 240, 240),		// background
				ImColor(160, 160, 160, 80),	// grid
				ImColor(50, 50, 50),		// link
				ImColor(220, 220, 220),		// nodeBackground
				ImColor(20, 20, 20)			// nodeText
			};
		}
	}
}

