#include "htn_parser.h"
#include "htn_parser.h"
#include <queue>
void HTNParser::LoadFromXML(const char* filename)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS) return;
	
	current_id = 0;
	float startX = 0.0f;
	float startY = 0.0f;

	if (auto* root = doc.RootElement()->FirstChildElement())
	{
		ParseELement(root, SIZE_MAX, startX, startY, 0);
	}
}

NodeType HTNParser::StringToType(const std::string& typeStr)
{
	static const std::unordered_map<std::string, NodeType> typeMap = {
		{"Method", NodeType::Method},
		{"Selector", NodeType::Selector},
		{"Sequence", NodeType::Sequence},
		{"Task", NodeType::Task},
		{"Condition", NodeType::Condition},
		{"Action", NodeType::Action}
	};
	auto it = typeMap.find(typeStr);
	return (it != typeMap.end() ? it->second : NodeType::Task);
}

void HTNParser::ParseELement(tinyxml2::XMLElement* elem, const size_t parentID, float& x, float& y, int depth)
{
	if (!elem) return;
	HTNNode node;

	node.id = current_id++;
	node.type = StringToType(elem->Name());
	node.name = elem->Attribute("name") ? elem->Attribute("name") : "";

	const float verticalSpacing = 100.0f;   
	const float horizontalSpacing = 100.0f; 
	node.pos = ImVec2(x, y + depth * verticalSpacing);

	for (const auto* attr = elem->FirstAttribute(); attr; attr = attr = attr->Next())
	{
		if (strcmp(attr->Name(), "name") != 0)
		{
			node.properties[attr->Name()] = attr->Value();
		}
	}

	nodes.push_back(node);
	if (parentID != SIZE_MAX)
	{
		links.emplace_back(parentID, node.id);
	}

	const int childCount = elem->ChildElementCount();
	if (childCount > 0)
	{
		float startX = x - (childCount * horizontalSpacing) / 2.0f;

		for (auto* child = elem->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			ParseELement(child, node.id, startX, y, depth + 1);
			startX += horizontalSpacing; 
		}
	}
}

void HTNParser::GetChildIndex(tinyxml2::XMLElement* elem)
{
}
