#include "htn/core/htn_parser.h"
#include <queue>
#include <iostream>

namespace HTN::Core
{
	void HTNParser::LoadFromXML(const char* filename) {
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS) return;

		current_id = 0;

		for (auto& [depth, nodes]: depthMap)
		{
			nodes.clear();
		}
		links.clear();

		float startX = 900;
		float startY = 100.0f;

		if (auto* root = doc.RootElement()->FirstChildElement()) {
			ParseElement(root, nullptr, startX, startY, 0);
		}
	}

	void HTNParser::ParseElement(tinyxml2::XMLElement* elem, Node* parent, float& x, float& y, int depth) {
		if (!elem) return;

		auto node = std::make_unique<Node>();
		Node* rawNodePtr = node.get();
		node->id = current_id++;
		node->inputPinID = current_id++;
		node->outputPinID = current_id++;
		node->type = StringToNodeType(elem->Name());
		node->name = elem->Attribute("name") ? elem->Attribute("name") : "";
		node->position = ImVec2(x, y + depth * 150.0f);

		// Properties
		for (const auto* attr = elem->FirstAttribute(); attr; attr = attr->Next()) {
			if (strcmp(attr->Name(), "name") != 0)
				node->properties[attr->Name()] = attr->Value();
		}

		depthMap[depth].push_back(std::move(node));

		if (!parent && !rootNode) {
			rootNode = rawNodePtr;
		}

		if (parent) {
			parent->children.push_back(rawNodePtr);
			links.push_back(Link{ current_id++, parent->outputPinID, rawNodePtr->inputPinID });
		}

		float currentX = x;
		for (auto* child = elem->FirstChildElement(); child; child = child->NextSiblingElement()) {
			if (strcmp(child->Name(), "Property") == 0 || strcmp(child->Name(), "Condition") == 0) continue;

			ParseElement(child, rawNodePtr, currentX, y, depth + 1);
		}
	}
}
