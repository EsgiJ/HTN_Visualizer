#pragma once

#include "htn/core/htn_parser.h"
#include "htn/editor/editor_ui.h"
#include "htn/xmlgenerator/xml_generator.h"

#include <filesystem>
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
		void DrawTreeNode();
		void DrawTreeNodeRecursive(const HTN::Core::Node* node);
		void DrawNodeProperties();

		std::string GetResourcePath(const std::string& resourceFile);

		GLFWwindow* m_Window = nullptr;
		HTN::Core::HTNParser m_Parser;
		HTN::Editor::EditorUI m_EditorUI;
		HTN::XML::XMLGenerator m_XMLGenerator;

		float m_Zoom = 1.0f;
		ImVec2 m_ViewOffset{ 0, 0 };
		bool m_IsDraggingView = false;
		ImVec2 m_LastMousePosition{ 0, 0 };
		bool m_IsDragging = false;
		bool m_FirstFrame = true;
		HTN::Core::Node* m_SelectedNode = nullptr;
	};
}
