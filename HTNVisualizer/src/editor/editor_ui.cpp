#define IMGUI_DEFINE_MATH_OPERATORS

#include "htn/core/htn_model.h"
#include <imgui_extra_math.h>
#include <imgui_extra_math.inl>
#include <iostream>
#include <algorithm>
#include "htn/editor/editor_ui.h"


void HTN::Editor::EditorUI::DrawGrid(const ImVec2& canvasSize, ImVec2 ViewOffset, float Zoom)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const float gridStep = 32.0f;
	const ImColor gridColor = ImColor(60, 60, 60, 100);
	const float gridThickness = 2.0f * Zoom;

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImVec2 origin = ImGui::GetCursorScreenPos();
	for (float x = fmodf(windowPos.x, gridStep); x < windowSize.x; x += gridStep)
	{
		drawList->AddLine(ImVec2(x, 0), ImVec2(x, windowSize.y), gridColor);
	}
	for (float y = fmodf(windowPos.y, gridStep); y < windowSize.y; y += gridStep)
	{
		drawList->AddLine(ImVec2(0, y), ImVec2(windowSize.x, y), gridColor);
	}
}

void HTN::Editor::EditorUI::DrawNode(HTN::Core::Node& node, ImVec2 ViewOffset, float Zoom)
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const std::string& nodeType = HTN::Core::NodeTypeToString(node.type);

	//HTN::Core::GetNodeColor(node.type);  
	const ImColor bodyColor(50, 50, 50, 255);
	const ImColor pinColor(30, 30, 30, 255);
	const ImColor borderColor(100, 100, 100, 200);
	const ImColor labelBgColor = HTN::Core::GetNodeColor(node.type);

	const float initialWidth = 200.f * Zoom;
	const float baseHeight = 80.f * Zoom;
	const float pinHeight = 15.0f * Zoom;
	const float padding = 10.0f * Zoom;
	const float pinPadding = 50.0f * Zoom;


	ImVec2 textSize = ImGui::CalcTextSize(node.name.c_str());

	ImVec2 scaledPos = ImVec2(node.position.x * Zoom, node.position.y * Zoom) + ViewOffset;
	ImVec2 scaledSize = ImVec2(node.size.x * Zoom, node.size.y * Zoom);

	//Node body
	drawList->AddRectFilled(
		scaledPos,
		ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y),
		bodyColor,
		4.0f * Zoom
	);

	// Border
	drawList->AddRect(
		scaledPos, 
		ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y), 
		borderColor, 
		4.0f, 
		0, 
		2.0f
	); 

	//Top pin
	drawList->AddRectFilled(
		ImVec2(scaledPos.x + pinPadding, scaledPos.y),
		ImVec2(scaledPos.x + scaledSize.x - pinPadding, scaledPos.y + pinHeight),
		pinColor,
		2.0f
	);

	//Top pin border
	drawList->AddRect(
		ImVec2(scaledPos.x + pinPadding, scaledPos.y),
		ImVec2(scaledPos.x + scaledSize.x - pinPadding, scaledPos.y + pinHeight),
		borderColor,
		2.0f,
		0,
		2.0f
	);

	//Bottom pin
	drawList->AddRectFilled(
		ImVec2(scaledPos.x + pinPadding, scaledPos.y + scaledSize.y - pinHeight),
		ImVec2(scaledPos.x + scaledSize.x - pinPadding, scaledPos.y + scaledSize.y),
		pinColor,
		1.0f
	);

	//Bottom pin border
	drawList->AddRect(
		ImVec2(scaledPos.x + pinPadding, scaledPos.y + scaledSize.y - pinHeight),
		ImVec2(scaledPos.x + scaledSize.x - pinPadding, scaledPos.y + scaledSize.y),
		borderColor,
		2.0f,
		0,
		2.0f
	);

	// Label Background
	drawList->AddRectFilled(
		ImVec2(scaledPos.x + padding / 2, scaledPos.y + pinHeight * 1.5f),
		ImVec2(scaledPos.x + scaledSize.x - padding / 2, scaledPos.y + scaledSize.y - pinHeight * 1.5f),
		labelBgColor,
		2.0f
	);

	// Label Background Border
	drawList->AddRect(
		ImVec2(scaledPos.x + padding / 2, scaledPos.y + pinHeight * 1.5f),
		ImVec2(scaledPos.x + scaledSize.x - padding / 2, scaledPos.y + scaledSize.y - pinHeight * 1.5f),
		borderColor,
		2.0f,
		0,
		2.0f
	);

	// Draw Text (Centered)
	std::string label = node.name;
	//ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

	ImVec2 textPos = ImVec2(
		scaledPos.x + (scaledSize.x - textSize.x) * 0.5f,
		scaledPos.y + (scaledSize.y - textSize.y) * 0.5f
	);

	if (EditorUI::boldFont)
		ImGui::PushFont(EditorUI::boldFont);

	// Draw label text
	drawList->AddText(textPos, ImColor(230, 230, 230, 255), label.c_str());

	if (EditorUI::boldFont)
		ImGui::PopFont();
}

void HTN::Editor::EditorUI::DrawLink(const HTN::Core::Link& link, const std::unordered_map<int, std::vector<std::unique_ptr<HTN::Core::Node>>>& depthMap, ImVec2 ViewOffset, float Zoom)
{
	auto find_node = [&](int id) -> const HTN::Core::Node*
		{
			for (const auto& [depth, nodePtrs] : depthMap)
			{
				for (const auto& nodePtr : nodePtrs)
				{
					if (nodePtr->inputPinID == id || nodePtr->outputPinID == id)
					{
						return nodePtr.get();
					}
				}
			}
			return nullptr;
		};

	const HTN::Core::Node* startNode = find_node(link.startNodeId);
	const HTN::Core::Node* endNode = find_node(link.endNodeId);

	if (!startNode || !endNode) return;

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImColor linkColor = ImColor(255, 255, 255, 200);
	const float linkThickness = 4.0f * Zoom; 
	const float arrowSize = 16.0f * Zoom;    

	ImVec2 scaledStartPos = ImVec2(
		(startNode->position.x * Zoom) + ViewOffset.x,
		(startNode->position.y * Zoom) + ViewOffset.y
	);
	ImVec2 scaledEndPos = ImVec2(
		(endNode->position.x * Zoom) + ViewOffset.x,
		(endNode->position.y * Zoom) + ViewOffset.y
	);

	ImVec2 startPos = ImVec2(
		scaledStartPos.x + (startNode->size.x * 0.5f * Zoom),
		scaledStartPos.y + (startNode->size.y * Zoom)
	);
	ImVec2 endPos = ImVec2(
		scaledEndPos.x + (endNode->size.x * 0.5f * Zoom),
		scaledEndPos.y
	);

	ImVec2 dir = ImNormalized(endPos - startPos);
	ImVec2 perpendicular(-dir.y, dir.x);
	ImVec2 arrowHead = endPos - (dir * arrowSize);

	drawList->AddLine(startPos, arrowHead, linkColor, linkThickness);

	drawList->AddTriangleFilled(
		arrowHead + (perpendicular * arrowSize * 0.5f),
		arrowHead - (perpendicular * arrowSize * 0.5f),
		endPos,
		linkColor
	);
}
