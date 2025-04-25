#include "htn/core/htn_parser.h"
#include <queue>

namespace HTN::Core
{
	void HTNParser::LoadFromXML(const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS) return;

		current_id = 0;
		float startX = 300.0f;
		float startY = 0.0f;

		if (auto* root = doc.RootElement()->FirstChildElement()) {
			ParseElement(root, -1, startX, startY, 0);
		}
	}

	NodeType HTNParser::StringToType(const std::string& typeStr) {
		static const std::unordered_map<std::string, NodeType> typeMap = {
			{"Method", NodeType::Method},
			{"Selector", NodeType::Selector},
			{"Sequence", NodeType::Sequence},
			{"Task", NodeType::Task},
			{"Condition", NodeType::Condition},
			{"Action", NodeType::Action}
		};
		return typeMap.at(typeStr);
	}

	void HTNParser::ParseElement(tinyxml2::XMLElement* elem, int parentID, float& x, float& y, int depth) {
		if (!elem) return;

		Node node;
		node.id = current_id++;
		node.inputPinID = current_id++;
		node.outputPinID = current_id++;
		node.type = StringToType(elem->Name());
		node.name = elem->Attribute("name") ? elem->Attribute("name") : "";
		node.position = ImVec2(x, y + depth * 150.0f);

		// Properties
		for (const auto* attr = elem->FirstAttribute(); attr; attr = attr->Next()) {
			if (strcmp(attr->Name(), "name") != 0)
				node.properties[attr->Name()] = attr->Value();
		}

		nodes.push_back(node);

		// Parent-child link
		if (parentID != -1) {

			const auto& parentNode = *std::find_if(nodes.begin(), nodes.end(),
				[&parentID](const Node& n) { return n.id == parentID; });

			links.push_back(Link{
				current_id++,
				parentNode.outputPinID,
				node.inputPinID
			});
		}

		// Children
		const int childCount = elem->ChildElementCount();
		float startX = x - (childCount * 100.0f) / 2.0f;
		for (auto* child = elem->FirstChildElement(); child; child = child->NextSiblingElement()) {
			ParseElement(child, node.id, startX, y, depth + 1);
			startX += 100.0f;
		}
	}
}
