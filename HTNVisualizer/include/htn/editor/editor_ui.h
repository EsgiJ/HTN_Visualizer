#pragma once

#include "htn/core/htn_model.h"

#include <imgui.h>
namespace HTN::Editor
{
	class EditorUI
	{
	public:
		void DrawGrid(const ImVec2& canvasSize);
		void DrawNode(HTN::Core::Node& node);
		void DrawLink(const HTN::Core::Link& link, const std::vector<HTN::Core::Node>& nodes);
	};
}