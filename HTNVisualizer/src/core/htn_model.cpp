#include "htn/core/htn_model.h"

namespace HTN::Core
{
	ImColor GetNodeColor(NodeType nodeType)
	{
		static const std::unordered_map<NodeType, ImColor> colors
		{
			{NodeType::Method,    ImColor(0, 174, 239)},
			{NodeType::Selector,  ImColor(124, 187, 0)},
			{NodeType::Sequence,  ImColor(243, 156, 18)},
			{NodeType::Task,      ImColor(231, 76, 60)},
			{NodeType::Condition, ImColor(255, 221, 87)},
			{NodeType::Action,    ImColor(155, 89, 182)}
		};
		return colors.at(nodeType);
	}

	const std::string& NodeTypeToString(NodeType type)
	{
		static std::string names[] = { "Method", "Selector", "Sequence", "Task", "Condition", "Action" };
		return names[static_cast<int>(type)];
	}
}

