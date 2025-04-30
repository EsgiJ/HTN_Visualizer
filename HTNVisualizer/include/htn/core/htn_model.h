#pragma once

#include <imgui.h>
#include <string>
#include <unordered_map>

namespace HTN::Core
{
	enum class NodeType {
		Root,
		Task,
		Condition,
		Invalid
	};

	struct Node {
		int id;
		int inputPinID;
		int outputPinID;
		std::string name;
		NodeType type = NodeType::Task;
		ImVec2 position{ 0,0 };
		ImVec2 size{ 200,120 };
		std::vector<Node*> children;
		std::unordered_map<std::string, std::string> properties;
	};

	struct Link {
		int id;
		int startNodeId;
		int endNodeId;
	};

	const float nodeWidth = 200.0f;
	const float horizontalSpacing = 100.0f;

	ImColor GetNodeColor(NodeType nodeType);

	const std::string& NodeTypeToString(NodeType type);

	NodeType StringToNodeType(const std::string& typeStr);
}