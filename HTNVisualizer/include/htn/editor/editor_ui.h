#pragma once

#include "htn/core/htn_model.h"

#include <imgui.h>

namespace HTN::Editor
{
	class EditorUI
	{
	public:
		void DrawGrid(const ImVec2& canvasSize, ImVec2 ViewOffset, float Zoom);
		void DrawNode(HTN::Core::Node& node, ImVec2 ViewOffset, float Zoom);
		void DrawLink(const HTN::Core::Link& link, const std::unordered_map<int, std::vector<std::unique_ptr<HTN::Core::Node>>>& depthMap, ImVec2 ViewOffset, float Zoom);


		static inline ImFont* boldFont = nullptr;
	};
}