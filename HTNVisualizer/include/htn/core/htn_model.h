#pragma once

#include <imgui.h>
#include <string>
#include <unordered_map>

namespace HTN::Core
{
	enum class NodeType {
		Method,
		Selector,
		Sequence,
		Task,
		Condition,
		Action
	};

	struct Node {
		int id;
		int inputPinID;
		int outputPinID;
		std::string name;
		NodeType type = NodeType::Action;
		ImVec2 position{ 0,0 };
		ImVec2 size{ 100,60 };
		std::unordered_map<std::string, std::string> properties;
	};

	struct Link {
		int id;
		int startNodeId;
		int endNodeId;
	};

	// Helper function to get node colors
	ImColor GetNodeColor(NodeType nodeType);

	const std::string& NodeTypeToString(NodeType type);
}