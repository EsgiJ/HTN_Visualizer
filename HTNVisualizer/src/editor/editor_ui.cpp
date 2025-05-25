#define IMGUI_DEFINE_MATH_OPERATORS

#include "htn/core/htn_model.h"
#include <imgui_extra_math.h>
#include <imgui_extra_math.inl>
#include <iostream>
#include <algorithm>
#include "htn/editor/editor_ui.h"
#include "htn/editor/theme_manager.h"

namespace HTN::Editor
{
	void EditorUI::DrawGrid(const ImVec2& canvasSize, ImVec2 viewOffset, float zoom)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		auto colors = ThemeManager::GetColors();

		const float gridStep = 32.0f * zoom;
		const ImColor gridColor = colors.grid;
		const float gridThickness = 1.0f;

		ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();

		ImVec2 startPos = ImVec2(
			fmodf(viewOffset.x, gridStep),
			fmodf(viewOffset.y, gridStep)
		);

		for (float x = startPos.x; x < canvasSize.x; x += gridStep)
		{
			drawList->AddLine(
				ImVec2(canvasOrigin.x + x, canvasOrigin.y),
				ImVec2(canvasOrigin.x + x, canvasOrigin.y + canvasSize.y),
				gridColor,
				gridThickness
			);
		}

		for (float y = startPos.y; y < canvasSize.y; y += gridStep)
		{
			drawList->AddLine(
				ImVec2(canvasOrigin.x, canvasOrigin.y + y),
				ImVec2(canvasOrigin.x + canvasSize.x, canvasOrigin.y + y),
				gridColor,
				gridThickness
			);
		}
	}


	void EditorUI::DrawNode(HTN::Core::Node& node, ImVec2 ViewOffset, float Zoom, bool isMinimap)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const std::string& nodeType = HTN::Core::NodeTypeToString(node.type);

		//HTN::Core::GetNodeColor(node.type);  
		auto colors = ThemeManager::GetColors();

		const ImColor bodyColor = colors.nodeBackground;
		const ImColor pinColor(30, 30, 30, 255);
		const ImColor borderColor(100, 100, 100, 200);
		const ImColor labelBgColor = HTN::Core::GetNodeColor(node.type);
		const ImColor textColor = ImColor(230, 230, 230, 255);

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

		if (isMinimap)
		{
			float fontSize = std::clamp(Zoom * 14.0f, 6.0f, 14.0f);
			ImVec2 scaledTextSize = ImVec2(textSize.x * (fontSize / 14.0f), textSize.y * (fontSize / 14.0f));
			ImVec2 textPos = ImVec2(
				scaledPos.x + (scaledSize.x - scaledTextSize.x) * 0.5f,
				scaledPos.y + (scaledSize.y - scaledTextSize.y) * 0.5f
			);
			drawList->AddText(nullptr, fontSize, textPos, textColor, label.c_str());
		}
		else
		{
			if (boldFont)
				ImGui::PushFont(boldFont);

			drawList->AddText(textPos, textColor, label.c_str());

			if (boldFont)
				ImGui::PopFont();
		}
	}

	void EditorUI::DrawLink(const HTN::Core::Link& link, const std::unordered_map<int, std::vector<std::unique_ptr<HTN::Core::Node>>>& depthMap, ImVec2 viewOffset, float zoom, bool isMinimap)
	{
		auto find_node = [&](int id) -> const HTN::Core::Node*
			{
				for (const auto& [depth, nodePtrs] : depthMap)
				{
					for (const auto& nodePtr : nodePtrs)
					{
						if (nodePtr->inputPinID == id || nodePtr->outputPinID == id)
							return nodePtr.get();
					}
				}
				return nullptr;
			};

		const HTN::Core::Node* startNode = find_node(link.startNodeId);
		const HTN::Core::Node* endNode = find_node(link.endNodeId);

		if (!startNode || !endNode)
			return;

		auto colors = ThemeManager::GetColors();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		const ImColor linkColor = colors.link;
		const float linkThickness = isMinimap ? 1.5f : 4.0f;
		const float arrowSize = isMinimap ? 6.0f : 16.0f;

		// Calculate scaled positions
		ImVec2 scaledStartPos = startNode->position * zoom + viewOffset;
		ImVec2 scaledEndPos = endNode->position * zoom + viewOffset;

		ImVec2 startPos = ImVec2(
			scaledStartPos.x + (startNode->size.x * 0.5f * zoom),
			scaledStartPos.y + (startNode->size.y * zoom)
		);
		ImVec2 endPos = ImVec2(
			scaledEndPos.x + (endNode->size.x * 0.5f * zoom),
			scaledEndPos.y
		);

		// Direction vector and perpendicular for arrow
		ImVec2 dir = ImNormalized(endPos - startPos);
		ImVec2 perpendicular(-dir.y, dir.x);
		ImVec2 arrowHead = endPos - (dir * arrowSize);

		// Draw line
		drawList->AddLine(startPos, arrowHead, linkColor, linkThickness);

		// Draw arrow head
		drawList->AddTriangleFilled(
			arrowHead + (perpendicular * arrowSize * 0.5f),
			arrowHead - (perpendicular * arrowSize * 0.5f),
			endPos,
			linkColor
		);
	}
}