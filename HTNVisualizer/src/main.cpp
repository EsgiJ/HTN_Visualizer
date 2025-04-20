#include "htn_parser.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imnodes.h>
#include <GLFW/glfw3.h>
ImColor GetNodeColor(NodeType nodeType)
{
	static const std::unordered_map<NodeType, ImColor> colors{
		{NodeType::Method,    ImColor(0, 174, 239)},    // Blue
		{NodeType::Selector,  ImColor(124, 187, 0)},    // Green
		{NodeType::Sequence,  ImColor(243, 156, 18)},   // Orange
		{NodeType::Task,      ImColor(231, 76, 60)},    // Red
		{NodeType::Condition, ImColor(255, 221, 87)},   // Yellow
		{NodeType::Action,    ImColor(155, 89, 182)}    // Purple
	};
	auto it = colors.find(nodeType);
	return it != colors.end() ? it->second : ImColor(255, 255, 255);
}

const char* NodeTypeToString(NodeType type)
{
	static const char* names[] = { "Method", "Selector", "Sequence", "Task", "Condition", "Action" };
	return names[static_cast<int>(type)];
}

void DrawNode(const HTNNode& node)
{
	ImNodes::PushColorStyle(ImNodesCol_TitleBar, GetNodeColor(node.type));
	ImNodes::BeginNode(node.id);

	//Title bar
	ImNodes::BeginNodeTitleBar();
	ImGui::TextColored(ImColor(255, 255, 255), "%s #%zu", NodeTypeToString(node.type), node.id);
	if (!node.name.empty())
	{
		ImGui::SameLine();
		ImGui::Text(": %s", node.name.c_str());
	}
	ImNodes::EndNodeTitleBar();

	const int input_attr_id = (node.id << 16) | 0x1;
	ImNodes::BeginInputAttribute(input_attr_id);
	ImGui::Text("↑");
	ImNodes::EndInputAttribute();

	//Properties
	ImGui::BeginChildFrame(ImGui::GetID((void*)(intptr_t)node.id), ImVec2(10, 10));
	for (const auto& [key, value] : node.properties)
	{
		ImGui::TextColored(ImColor(180, 180, 180), "%s:", key.c_str());
		ImGui::SameLine();
		ImGui::Text("%s", value.c_str());
	}
	ImGui::EndChildFrame();


	if (node.type != NodeType::Action && node.type != NodeType::Condition)
	{
		const int output_attr_id = (node.id << 16) | 0x2;
		ImNodes::BeginOutputAttribute(output_attr_id);
		ImGui::Text("↓");
		ImNodes::EndOutputAttribute();
	}

	ImNodes::EndNode();
	ImNodes::PopColorStyle();
}

int main()
{
	//Init GLFW
	if (!glfwInit()) return 1;
	GLFWwindow* window = glfwCreateWindow(1600, 900, "HTN Visualizer", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImNodes::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImNodesStyle& style = ImNodes::GetStyle();
	style.Colors[ImNodesCol_NodeBackground] = ImColor(30, 30, 30);
	ImNodes::GetStyle().GridSpacing = 100.0f;

	HTNParser parser;
	parser.LoadFromXML("C:\\GameProjects\\HTN_Visualizer\\HTNVisualizer\\behavior.xml");

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Tüm ekranı kaplayan ana docking alanı
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoDocking;

		if (ImGui::Begin("HTN Visualizer", nullptr, window_flags))
		{
			ImNodes::BeginNodeEditor();

			for (const auto& node : parser.nodes)
			{
				ImNodes::SetNodeGridSpacePos(node.id, node.pos);
				DrawNode(node);
			}

			static int link_id = 0;
			for (const auto& [from, to] : parser.links)
			{
				const int from_attr_id = (from << 16) | 0x2;  // Output attribute
				const int to_attr_id = (to << 16) | 0x1;      // Input attribute

				ImNodes::Link(
					link_id++, // int ID
					from_attr_id,      // int start_pin_id
					to_attr_id         // int end_pin_id
				);
			}

			ImNodes::EndNodeEditor();

			ImGui::End();
		}


		//Render
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}

	//Cleanup
	ImNodes::DestroyContext();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}