#define IMGUI_DEFINE_MATH_OPERATORS

#include "htn/core/htn_model.h"
#include <imgui_extra_math.h>
#include <imgui_extra_math.inl>

#include "htn/editor/editor_ui.h"


void HTN::Editor::EditorUI::DrawGrid(const ImVec2& canvasSize)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const float gridStep = 64.0f;
	const ImColor gridColor = ImColor(60, 60, 60, 100);
	const float gridThickness = 2.0f;

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImVec2 origin = ImGui::GetCursorScreenPos(); // pencere içi baþlangýç
	for (float x = fmodf(origin.x, gridStep); x < origin.x + windowSize.x; x += gridStep)
	{
		drawList->AddLine(ImVec2(x, origin.y), ImVec2(x, origin.y + windowSize.y), gridColor, gridThickness);
	}
	for (float y = fmodf(origin.y, gridStep); y < origin.y + windowSize.y; y += gridStep)
	{
		drawList->AddLine(ImVec2(origin.x, y), ImVec2(origin.x + windowSize.x, y), gridColor, gridThickness);
	}
}

void HTN::Editor::EditorUI::DrawNode(HTN::Core::Node& node)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImColor nodeColor = HTN::Core::GetNodeColor(node.type);  
	const std::string& nodeType = HTN::Core::NodeTypeToString(node.type);

	//Node body
	drawList->AddRectFilled(
		node.position,
		ImVec2(node.position.x + node.size.x, node.position.y + node.size.y),
		nodeColor,
		4.0f
	);

	const float pinPadding = node.size.x * 0.1f;
	//Top pin
	drawList->AddRectFilled(
		ImVec2(node.position.x + pinPadding, node.position.y + 5),
		ImVec2(node.position.x + node.size.x - pinPadding, node.position.y + 15),
		nodeColor,
		4.0f
	);

	//Bottom pin
	drawList->AddRectFilled(
		ImVec2(node.position.x + pinPadding, node.position.y + node.size.y - 15),
		ImVec2(node.position.x + node.size.x - pinPadding, node.position.y + node.size.y - 5),
		nodeColor,
		4.0f
	);

	ImGui::SetCursorScreenPos(ImVec2(node.position.x + 5, node.position.y + 5));
	ImGui::BeginGroup();
	ImGui::Text("%s : %s", nodeType.c_str(), node.name.c_str());
	ImGui::EndGroup();
}

void HTN::Editor::EditorUI::DrawLink(const HTN::Core::Link& link, const std::vector<HTN::Core::Node>& nodes)
{
	auto find_node = [&](int id) -> const HTN::Core::Node* 
	{
		for (const auto& node : nodes)
		{
			if (node.id == id)
			{
				return &node;
			}
		}
		return nullptr;
	};

	const HTN::Core::Node* startNode = find_node(link.startNodeId);
	const HTN::Core::Node* endNode = find_node(link.endNodeId);

	if (!startNode || !endNode) return;

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const ImColor linkColor = ImColor(255, 255, 255, 255);
	const float linkThickness = 2.0f;
	const float arrowSize = 8.0f;

	ImVec2 startPos = {
		startNode->position.x + startNode->position.x * 0.5f,
		endNode->position.y + endNode->size.y
	};

	ImVec2 endPos = {
		endNode->position.x + startNode->size.x * 0.5f,
		startNode->position.y
	};

	drawList->AddLine(startPos, endPos, linkColor, linkThickness);

	ImVec2 dir = ImNormalized(ImVec2(endPos.x - startPos.x, endPos.y - startPos.y));
	ImVec2 perpendicular(-dir.y, dir.x);
	ImVec2 arrowHead = ImVec2((endPos.x - dir.x * arrowSize), (endPos.y - dir.y * arrowSize));
	drawList->AddTriangleFilled(
		ImVec2((arrowHead.x + perpendicular.x * arrowSize * 0.5f), (arrowHead.y + perpendicular.y* arrowSize * 0.5f)),
		ImVec2((arrowHead.x - perpendicular.x * arrowSize * 0.5f), (arrowHead.y - perpendicular.y * arrowSize * 0.5f)),
		endPos,
		linkColor
	);
}
