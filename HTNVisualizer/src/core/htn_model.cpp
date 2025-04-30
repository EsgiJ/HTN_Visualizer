#include "htn/core/htn_model.h"

namespace HTN::Core
{
	ImColor GetNodeColor(NodeType nodeType)
	{
		static const std::unordered_map<NodeType, ImColor> colors
		{
			{NodeType::Root,		 ImColor(52, 152, 219, 255)},
			{NodeType::Task,		 ImColor(98, 46, 163, 255)},
			{NodeType::Condition,	 ImColor(155, 89, 182, 255)},
			{NodeType::Invalid,		 ImColor(230, 126, 34, 255)}
		};
		return colors.at(nodeType);
	}

	const std::string& NodeTypeToString(NodeType type)
	{
		static std::string names[] = { "Root", "Task", "Condition"};
		return names[static_cast<int>(type)];
	}

	NodeType StringToNodeType(const std::string& typeStr) {
		static const std::unordered_map<std::string, NodeType> typeMap = {
			{"Root", NodeType::Root},
			{"Task", NodeType::Task},
			{"Condition", NodeType::Condition },
		};

		auto it = typeMap.find(typeStr);
		return (it != typeMap.end()) ? it->second : NodeType::Invalid;
	}
}

