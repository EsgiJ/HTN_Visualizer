#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <algorithm>
#include <iostream>

#include "htn/editor/editor_ui.h"
#include "htn/app/application.h"

namespace HTN::App
{
	bool Application::Initialize()
	{
		m_XMLGenerator.Run();

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
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 5.0f;
		style.FrameRounding = 3.0f;

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
			// Canvas setup
			ImVec2 canvasPos = ImGui::GetCursorScreenPos();
			ImVec2 canvasSize = ImGui::GetContentRegionAvail();

			if (canvasSize.x <= 0 || canvasSize.y <= 0) {
				canvasSize = ImVec2(1920, 1080); // Fallback size
			}

			ImGui::SetCursorPos(ImVec2(0, 0));
			ImGui::InvisibleButton("canvas", canvasSize,
				ImGuiButtonFlags_MouseButtonLeft |
				ImGuiButtonFlags_MouseButtonRight | 
				ImGuiButtonFlags_MouseButtonMiddle
			);

			ImGui::SetWindowFontScale(m_Zoom);
			/*ImGui::GetWindowDrawList()->PushClipRect(
				ImGui::GetCursorScreenPos(),
				ImVec2(ImGui::GetCursorScreenPos().x + canvasSize.x, ImGui::GetCursorScreenPos().y + canvasSize.y)
			);*/

			// Grid drawing
			m_EditorUI.DrawGrid(canvasSize, m_ViewOffset, m_Zoom);

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
					m_EditorUI.DrawNode(*nodePtr, m_ViewOffset, m_Zoom);
					//m_EditorUI.HandleNodeInteraction(nodePtr, m_ViewOffset, m_SelectedNodes);
				}
			}

			// Link drawing
			for (const auto& link : m_Parser.links) {
				m_EditorUI.DrawLink(link, m_Parser.depthMap, m_ViewOffset, m_Zoom);
			}

			DrawTreeNode();
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
			ImGui::EndMainMenuBar();
		}
	}

	void HTN::App::Application::HandleInput()
	{
		//Zoom In/Out
		ImGuiIO& io = ImGui::GetIO();
		if (io.WantCaptureMouse)
		{
			float zoomDelta = io.MouseWheel * 0.1f;
			if (zoomDelta != 0)
			{
				m_Zoom += zoomDelta;
				m_Zoom = std::clamp(m_Zoom, 0.3f, 3.0f);
			}
		}

		//Dragging
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
		{
			if (!m_IsDragging)
			{
				m_IsDragging = true;
				m_LastMousePosition = ImGui::GetMousePos();
			}
			//Fix later, write your own vec2 class
			ImVec2 delta = ImVec2(ImGui::GetMousePos().x - m_LastMousePosition.x, ImGui::GetMousePos().y - m_LastMousePosition.y);
			m_ViewOffset.x += delta.x / m_Zoom;
			m_ViewOffset.y += delta.y / m_Zoom;

			m_LastMousePosition = ImGui::GetMousePos();
		}
		else
		{
			m_IsDragging = false;
		}
	}

	void Application::AutoArrangeNodes() {
		const float nodeWidth = 200.0f;
		const float horizontalSpacing = 50.0f;
		const float verticalSpacing = 200.0f;
		const float canvasWidth = ImGui::GetWindowWidth();

		float currentY = 50.0f;

		for (auto& [depth, nodePtrs] : m_Parser.depthMap) {
			float totalWidth = (nodeWidth + horizontalSpacing) * nodePtrs.size() - horizontalSpacing;
			float startX = (canvasWidth - totalWidth) / 2.0f;
			float currentX = startX;

			
			for (auto& nodePtr : nodePtrs) {
				nodePtr->position.x = currentX;
				nodePtr->position.y = currentY;

				currentX += nodeWidth + horizontalSpacing;
			}

			currentY += verticalSpacing;
		}
	}

	void Application::DrawTreeNode()
	{
		if (ImGui::Begin("Node Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse))
		{
			HTN:Core::Node* rootNode = m_Parser.rootNode;
			DrawTreeNodeRecursive(rootNode);
			ImGui::End();
		}
	}

	void Application::DrawTreeNodeRecursive(const HTN::Core::Node* node)
	{
		if (!node)
		{
			return;
		}

		ImGui::PushID(node->id);
		ImGuiTreeNodeFlags flags = node->children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

		bool isOpen = ImGui::TreeNodeEx(node->name.c_str(), flags);

		ImGui::SameLine();
		if (ImGui::SmallButton("Focus"))
		{
			//FocusNode(node);
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
		if (ImGui::Begin("Node Properties", nullptr, ImGuiWindowFlags_NoCollapse))
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

	std::string Application::GetResourcePath(const std::string& resourceFile)
	{
		std::filesystem::path projectPath = std::filesystem::path("C:\\GameProjects\\HTN_Visualizer\\HTNVisualizer");
		std::filesystem::path resourcePath = projectPath / "resources" / resourceFile;

		std::cout << resourcePath.string();
		return resourcePath.string();
	}
}

