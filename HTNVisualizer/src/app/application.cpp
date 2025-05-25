#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "htn/editor/editor_ui.h"
#include "htn/editor/imgui_utils.h"
#include "htn/app/application.h"
#include "htn/editor/theme_manager.h"

namespace HTN::App
{
	bool Application::Initialize()
	{
		//m_XMLGenerator.Run();

		// GLFW init
		if (!glfwInit()) return false;

		// Window setup
		m_Window = glfwCreateWindow(1920, 1080, "HTN Visualizer", nullptr, nullptr);
		if (!m_Window) {
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(m_Window);
		glfwSwapInterval(1);

		// ImGui setup
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Style setup
		SetupImGuiStyle(false, 1.0f);

		// Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		// Load data
		m_Parser.LoadFromXML(GetResourcePath("behavior.xml").c_str());
		HTN::Editor::EditorUI::boldFont = io.Fonts->AddFontFromFileTTF(GetResourcePath("fonts\\harabara\\Harabara.ttf").c_str(), 16.0f);
		return true;
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window)) {
			glfwPollEvents();

			// Start the ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

			HandleInput();
			DrawMenuBar();

			// Main editor window
			ImGui::Begin("HTN Editor", nullptr,
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse);


			ImGui::InvisibleButton("canvas", m_CanvasSize,
				ImGuiButtonFlags_MouseButtonLeft |
				ImGuiButtonFlags_MouseButtonRight | 
				ImGuiButtonFlags_MouseButtonMiddle
			);

			// Canvas setup
			ImVec2 m_CanvasSize = ImGui::GetContentRegionAvail();
			if (m_CanvasSize.x <= 0 || m_CanvasSize.y <= 0) {
				m_CanvasSize = ImVec2(1920, 1080); // Fallback size
			}
			ImVec2 m_CanvasPosition = ImGui::GetCursorScreenPos();

			ImGui::SetCursorPos(ImVec2(0, 0));
			ImGui::SetWindowFontScale(m_Zoom);
			/*ImGui::GetWindowDrawList()->PushClipRect(
				ImGui::GetCursorScreenPos(),
				ImVec2(ImGui::GetCursorScreenPos().x + m_CanvasSize.x, ImGui::GetCursorScreenPos().y + m_CanvasSize.y)
			);*/

			// Grid drawing
			m_EditorUI.DrawGrid(m_CanvasSize, m_ViewOffset, m_Zoom);

			if (m_FirstFrame)
			{
				AutoArrangeNodes();
				m_FirstFrame = false;
			}

			// Node Logic
			for (auto& [depth, nodePtrs] : m_Parser.depthMap) 
			{
				for (auto& nodePtr: nodePtrs)
				{
					m_EditorUI.DrawNode(*nodePtr, m_ViewOffset, m_Zoom, false);
					//m_EditorUI.HandleNodeInteraction(nodePtr, m_ViewOffset, m_SelectedNodes);
				}
			}

			// Link drawing
			for (const auto& link : m_Parser.links) {
				m_EditorUI.DrawLink(link, m_Parser.depthMap, m_ViewOffset, m_Zoom, false);
			}

			DrawTreeNode();
			DrawMinimap();
			GetSelectedNode();
			DrawNodeProperties();

			//ImGui::GetWindowDrawList()->PopClipRect();
			ImGui::End();

			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(m_Window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Multi-viewport support
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow* backup_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_context);
			}

			glfwSwapBuffers(m_Window);
		}
	}

	void Application::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwTerminate();
	}

	void HTN::App::Application::DrawMenuBar()
	{
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Open file */ }
				if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Save file */ }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Auto Arrange", nullptr, false)) {
					AutoArrangeNodes();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Dark Theme"))
				{
					HTN::Editor::ThemeManager::ApplyTheme(HTN::Editor::Theme::Dark);
				}
				if (ImGui::MenuItem("Light Theme"))
				{
					HTN::Editor::ThemeManager::ApplyTheme(HTN::Editor::Theme::Light);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void HTN::App::Application::HandleInput()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Zoom In/Out when not hovering any ImGui window
		if (io.WantCaptureMouse && ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		{
			float zoomDelta = io.MouseWheel * 0.1f;
			if (zoomDelta != 0.0f)
			{
				ImVec2 mousePos = ImGui::GetMousePos();

				// Calculate mouse position in world space before zoom
				ImVec2 beforeZoomWorldPos;
				beforeZoomWorldPos.x = (mousePos.x - m_ViewOffset.x) / m_Zoom;
				beforeZoomWorldPos.y = (mousePos.y - m_ViewOffset.y) / m_Zoom;

				// Apply zoom delta
				m_Zoom += zoomDelta;
				m_Zoom = std::clamp(m_Zoom, 0.3f, 3.0f);

				// Calculate new screen position of the same world point after zoom
				ImVec2 afterZoomScreenPos;
				afterZoomScreenPos.x = beforeZoomWorldPos.x * m_Zoom + m_ViewOffset.x;
				afterZoomScreenPos.y = beforeZoomWorldPos.y * m_Zoom + m_ViewOffset.y;

				// Calculate the difference and adjust view offset to keep mouse position stable
				ImVec2 zoomFixOffset;
				zoomFixOffset.x = mousePos.x - afterZoomScreenPos.x;
				zoomFixOffset.y = mousePos.y - afterZoomScreenPos.y;

				m_ViewOffset.x += zoomFixOffset.x;
				m_ViewOffset.y += zoomFixOffset.y;
			}
		}

		// Right-click dragging to pan the view
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
		{
			if (!m_IsDragging)
			{
				m_IsDragging = true;
				m_LastMousePosition = ImGui::GetMousePos();
			}

			ImVec2 currentMousePos = ImGui::GetMousePos();
			ImVec2 delta;
			delta.x = (currentMousePos.x - m_LastMousePosition.x);
			delta.y = (currentMousePos.y - m_LastMousePosition.y);

			m_ViewOffset.x += delta.x / m_Zoom;
			m_ViewOffset.y += delta.y / m_Zoom;

			m_LastMousePosition = currentMousePos;
		}
		else
		{
			m_IsDragging = false;
		}
	}


	float Application::LayoutNodeRecursive(HTN::Core::Node* node, float depth)
	{
		if (!node) return 0.0f;

		float subtreeWidth = 0.0f;

		if (!node->children.empty())
		{
			float totalChildWidth = 0.0f;

			for (auto& child : node->children)
			{
				float childSubtreeWidth = LayoutNodeRecursive(child, depth + 1);
				totalChildWidth += childSubtreeWidth + m_HorizontalSpacing;
			}

			if (totalChildWidth > 0.0f)
				totalChildWidth -= m_HorizontalSpacing;

			subtreeWidth = totalChildWidth;
			float firstChildX = node->children.front()->position.x;
			float lastChildX = node->children.back()->position.x;
			node->position.x = (firstChildX + lastChildX) / 2.0f;
		}
		else
		{
			node->position.x = m_LayoutCursorX;
			m_LayoutCursorX += m_NodeWidth + m_HorizontalSpacing;
			subtreeWidth = m_NodeWidth;
		}

		node->position.y = depth * m_VerticalSpacing;
		return subtreeWidth;
	}


	void Application::AutoArrangeNodes()
	{
		m_LayoutCursorX = 0.0f;
		if (m_Parser.rootNode)
		{
			LayoutNodeRecursive(m_Parser.rootNode, 0);
		}
	}

	void Application::DrawMinimap()
	{
		if (ImGui::Begin("Mini Map", nullptr,
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar
		))
		{
			ImGui::SliderFloat("Mini Map Zoom", &m_MinimapZoom, 0.02f, 0.3f, "%.2f");

			ImVec2 contentSize = ImGui::GetContentRegionAvail();
			ImVec2 minimapOffset = ImGui::GetCursorScreenPos();
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			const float minimapScale = m_MinimapZoom;

			// Scroll zoom with mouse position preserved
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
			{
				float wheel = ImGui::GetIO().MouseWheel;
				if (wheel != 0.0f)
				{
					ImVec2 mouse = ImGui::GetMousePos();
					ImVec2 beforeZoomWorld;
					beforeZoomWorld.x = (mouse.x - minimapOffset.x) / m_MinimapZoom;
					beforeZoomWorld.y = (mouse.y - minimapOffset.y) / m_MinimapZoom;

					m_MinimapZoom += wheel * 0.01f;
					m_MinimapZoom = std::clamp(m_MinimapZoom, 0.02f, 0.3f);

					ImVec2 afterZoomScreen;
					afterZoomScreen.x = beforeZoomWorld.x * m_MinimapZoom + minimapOffset.x;
					afterZoomScreen.y = beforeZoomWorld.y * m_MinimapZoom + minimapOffset.y;

					ImVec2 offsetFix;
					offsetFix.x = mouse.x - afterZoomScreen.x;
					offsetFix.y = mouse.y - afterZoomScreen.y;

					// You can store offsetFix here if you later implement panning
				}
			}

			// Draw nodes
			for (auto& [depth, nodePtrs] : m_Parser.depthMap)
			{
				for (auto& nodePtr : nodePtrs)
				{
					m_EditorUI.DrawNode(*nodePtr, minimapOffset, minimapScale, true);
				}
			}

			// Draw links
			for (const auto& link : m_Parser.links)
			{
				m_EditorUI.DrawLink(link, m_Parser.depthMap, minimapOffset, minimapScale, true);
			}

			// Viewport indicator
			ImVec2 viewMin;
			viewMin.x = minimapOffset.x + (-m_ViewOffset.x / m_Zoom) * minimapScale;
			viewMin.y = minimapOffset.y + (-m_ViewOffset.y / m_Zoom) * minimapScale;

			ImVec2 viewMax;
			viewMax.x = viewMin.x + m_CanvasSize.x * minimapScale;
			viewMax.y = viewMin.y + m_CanvasSize.y * minimapScale;

			drawList->AddRect(viewMin, viewMax, IM_COL32(255, 255, 0, 255));

			// Navigation click
			ImGui::InvisibleButton("minimap_canvas", contentSize);
			if (ImGui::IsItemClicked())
			{
				ImVec2 mouse = ImGui::GetMousePos();
				ImVec2 local;
				local.x = (mouse.x - minimapOffset.x) / m_MinimapZoom;
				local.y = (mouse.y - minimapOffset.y) / m_MinimapZoom;

				m_ViewOffset.x = -local.x * m_Zoom + m_CanvasSize.x / 2.0f;
				m_ViewOffset.y = -local.y * m_Zoom + m_CanvasSize.y / 2.0f;
			}
		}
		ImGui::End();
	}



	void Application::DrawTreeNode()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(250, 200), ImVec2(FLT_MAX, FLT_MAX));
		if (ImGui::Begin("Node Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar))
		{
			ImGui::Text("Search:");
			ImGui::SameLine();
			ImGui::InputText("##NodeSearch", m_NodeSearchBuffer, IM_ARRAYSIZE(m_NodeSearchBuffer));
			HTN:Core::Node* rootNode = m_Parser.rootNode;
			DrawTreeNodeRecursive(rootNode);
			ImGui::End();
		}
	}

	void Application::DrawTreeNodeRecursive(const HTN::Core::Node* node)
	{
		if (!node) return;

		if (!HasMatchingChildRecursive(node)) return;

		ImGui::PushID(node->id);
		ImGuiTreeNodeFlags flags = node->children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_DefaultOpen;

		bool isOpen = ImGui::TreeNodeEx(node->name.c_str(), flags);

		std::stringstream ss;
		ss << "Focus##" << node->id;

		ImGui::SameLine();
		if (ImGui::SmallButton(ss.str().c_str()))
		{
			FocusNode(node);
		}

		if (isOpen)
		{
			for (const auto& child : node->children)
			{
				if (child)
				{
					DrawTreeNodeRecursive(child);
				}
			}
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void Application::DrawNodeProperties()
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(250, 200), ImVec2(FLT_MAX, FLT_MAX));
		if (ImGui::Begin("Node Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar))
		{
			if (m_SelectedNode)
			{
				ImGui::PushItemWidth(-1);
				ImGui::Text("Node Type: %s", HTN::Core::NodeTypeToString(m_SelectedNode->type).c_str());
				ImGui::Text("Node Name: %s", m_SelectedNode->name.c_str());
				ImGui::Separator();

				//Property list
				ImGui::Text(("Properties:"));
				ImGui::Indent();
				for (const auto& [key, value]: m_SelectedNode->properties)
				{
					ImGui::Text("%s: %s", key.c_str(), value.c_str());
					ImGui::Separator();
				}
			}
			else
			{
				ImGui::Text("Select a node to view properties");
			}
			ImGui::End();
		}

	}

	void Application::GetSelectedNode()
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_SelectedNode = nullptr;
			for (auto& [depth, nodePtrs] : m_Parser.depthMap)
			{
				for (auto& nodePtr : nodePtrs)
				{
					ImVec2 nodePos = {
						nodePtr->position.x * m_Zoom + m_ViewOffset.x,
						nodePtr->position.y * m_Zoom + m_ViewOffset.y
					};

					ImVec2 nodeSize = {
						nodePtr->size.x * m_Zoom,
						nodePtr->size.y * m_Zoom
					};

					if (mousePos.x >= nodePos.x && mousePos.x <= nodePos.x + nodeSize.x &&
						mousePos.y >= nodePos.y && mousePos.y <= nodePos.y + nodeSize.y)
					{
						m_SelectedNode = nodePtr.get();
						break;
					}
				}
				if (m_SelectedNode) break;
			}
		}
	}

	void Application::FocusNode(const HTN::Core::Node* node) {
		if (!node) return;

		ImVec2 canvasCenter(
			m_CanvasPosition.x + m_CanvasSize.x * 0.5f,
			m_CanvasPosition.y + m_CanvasSize.y * 0.5f
		);

		ImVec2 nodeCenter(
			node->position.x + node->size.x * 0.5f,
			node->position.y + node->size.y * 0.5f
		);

		m_ViewOffset.x = (canvasCenter.x - m_CanvasPosition.x) - (nodeCenter.x * m_Zoom);
		m_ViewOffset.y = (canvasCenter.y - m_CanvasPosition.y) - (nodeCenter.y * m_Zoom);
	}

	bool Application::IsSearchMatch(const std::string& nodeName) const
	{
		// If the search string is empty, match everything
		if (strlen(m_NodeSearchBuffer) == 0) return true;

		std::string nodeNameLower = nodeName;
		std::string searchLower = m_NodeSearchBuffer;

		std::transform(nodeNameLower.begin(), nodeNameLower.end(), nodeNameLower.begin(), ::tolower);
		std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

		return nodeNameLower.find(searchLower) != std::string::npos;
	}

	bool Application::HasMatchingChildRecursive(const HTN::Core::Node* node) const
	{
		if (!node) return false;

		if (IsSearchMatch(node->name)) return true;

		for (const auto& childNode: node->children)
		{
			if (HasMatchingChildRecursive(childNode)) return true;
		}

		return false;
	}

	std::string Application::GetResourcePath(const std::string& resourceFile)
	{
		std::filesystem::path projectPath = std::filesystem::path("C:\\GameProjects\\HTN_Visualizer\\HTNVisualizer");
		std::filesystem::path resourcePath = projectPath / "resources" / resourceFile;

		std::cout << resourcePath.string();
		return resourcePath.string();
	}
}

