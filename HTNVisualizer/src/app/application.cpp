#include "htn/app/application.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <algorithm>

namespace HTN::App
{
	bool Application::Initialize()
	{
		// GLFW init
		if (!glfwInit()) return false;

		// Window setup
		m_Window = glfwCreateWindow(1280, 720, "HTN Visualizer", nullptr, nullptr);
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
		m_Parser.LoadFromXML("C:\\GameProjects\\HTN_Visualizer\\HTNVisualizer\\behavior.xml");
		//AutoArrangeNodes();

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
				canvasSize = ImVec2(1280, 720); // Fallback boyut
			}
			ImGui::InvisibleButton("canvas", canvasSize,
				ImGuiButtonFlags_MouseButtonLeft |
				ImGuiButtonFlags_MouseButtonRight);

			// Grid drawing
			m_EditorUI.DrawGrid(canvasSize);

			// Node interactions
			for (auto& node : m_Parser.nodes) {
				m_EditorUI.DrawNode(node);
				//m_EditorUI.HandleNodeInteraction(node, m_ViewOffset, m_SelectedNodes);
			}

			// Link drawing
			for (const auto& link : m_Parser.links) {
				m_EditorUI.DrawLink(link, m_Parser.nodes);
			}

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
				if (ImGui::MenuItem("Auto Arrange", nullptr, false, !m_SelectedNodes.empty())) {
					AutoArrangeNodes();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void HTN::App::Application::HandleInput()
	{
		// View panning with right drag
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
			if (!m_IsDraggingView) {
				m_IsDraggingView = true;
				m_DragStartPos = ImGui::GetMousePos();
			}
			//m_ViewOffset += ImGui::GetIO().MouseDelta;
		}
		else {
			m_IsDraggingView = false;
		}
	}

	void HTN::App::Application::AutoArrangeNodes()
	{
		const float SPACING = 100.0f;
		float currentY = 50.0f;

		for (auto& node : m_Parser.nodes) {
			node.position = ImVec2(
				ImGui::GetWindowWidth() * 0.5f - node.size.x * 0.5f,
				currentY
			);
			currentY += node.size.y + SPACING;
		}
	}
}

