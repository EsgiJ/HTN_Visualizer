#pragma once

#include "htn/core/htn_parser.h"
#include "htn/editor/editor_ui.h"

#include <GLFW/glfw3.h>

namespace HTN::App
{
	class Application
	{
	public:
		bool Initialize();
		void Run();
		void Shutdown();

	private:
		void DrawMenuBar();
		void HandleInput();
		void AutoArrangeNodes();

		GLFWwindow* m_Window = nullptr;
		HTN::Core::HTNParser m_Parser;
		HTN::Editor::EditorUI m_EditorUI;

		ImVec2 m_ViewOffset{ 0, 0 };
		bool m_IsDraggingView = false;
		ImVec2 m_DragStartPos{ 0, 0 };

		std::vector<int> m_SelectedNodes;
	};
}
