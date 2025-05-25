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
		float LayoutNodeRecursive(HTN::Core::Node* node, float depth);
		void AutoArrangeNodes();
		void DrawMinimap();
		void DrawTreeNode();
		void DrawTreeNodeRecursive(const HTN::Core::Node* node);
		void DrawNodeProperties();
		void GetSelectedNode();
		void FocusNode(const HTN::Core::Node* node);
		bool IsSearchMatch(const std::string& nodeName) const;
		bool HasMatchingChildRecursive(const HTN::Core::Node* node) const;
		std::string GetResourcePath(const std::string& resourceFile);

		GLFWwindow* m_Window = nullptr;
		HTN::Core::HTNParser m_Parser;
		HTN::Editor::EditorUI m_EditorUI;
		HTN::XML::XMLGenerator m_XMLGenerator;

		ImVec2 m_CanvasSize{ 1920, 1080 };
		ImVec2 m_CanvasPosition{ 0, 0 };
		char m_NodeSearchBuffer[128] = "";
		float m_Zoom = 1.0f;
		float m_MinimapZoom = 0.1f;
		ImVec2 m_ViewOffset{ 0, 0 };
		bool m_IsDraggingView = false;
		ImVec2 m_LastMousePosition{ 0, 0 };
		bool m_IsDragging = false;
		bool m_FirstFrame = true;

		float m_NodeWidth = 200.0f;
		float m_HorizontalSpacing = 50.0f;
		float m_VerticalSpacing = 200.0f;
		float m_LayoutCursorX = 0.0f;

		HTN::Core::Node* m_SelectedNode = nullptr;
	};
}
