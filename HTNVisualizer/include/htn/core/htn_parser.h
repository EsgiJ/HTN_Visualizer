#pragma once

#include "htn/core/htn_model.h"

#include <tinyxml2.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <imgui.h>

namespace HTN::Core
{
	class HTNParser {
	public:
		std::unordered_map<int, std::vector<std::unique_ptr<Node>>> depthMap;
		std::vector<Link> links;
		Node* rootNode = nullptr;

		void LoadFromXML(const char* filename);

	private:
		int current_id = 0;

		void ParseElement(tinyxml2::XMLElement* elem, Node* parent, float& x, float& y, int depth);
	};
}
