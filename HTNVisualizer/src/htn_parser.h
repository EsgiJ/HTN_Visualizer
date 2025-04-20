#pragma once
#include <tinyxml2.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <imgui.h>
#include <imnodes.h>

enum class NodeType
{
	Method,
	Selector,
	Sequence,
	Task,
	Condition,
	Action
};
struct HTNNode
{
	size_t id = 0;
	std::string name;
	NodeType type = NodeType::Task;
	ImVec2 pos;
	std::vector<size_t> children;
	std::unordered_map<std::string, std::string> properties;
};

class HTNParser
{
public:
	std::vector<HTNNode> nodes;
	std::vector<std::pair<size_t, size_t>> links;

	void LoadFromXML(const char* filename);
private:
	size_t current_id = 0;

	NodeType StringToType(const std::string& typeStr);
	void ParseELement(tinyxml2::XMLElement* elem, const size_t parentID, float& x, float& y, int depth);
	void GetChildIndex(tinyxml2::XMLElement* elem);
};