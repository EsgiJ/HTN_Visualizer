#pragma once

#include "htn/core/htn_model.h"

#include <tinyxml2.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <imgui.h>

namespace HTN::Core
{
	class HTNParser {
	public:
		std::vector<Node> nodes;
		std::vector<Link> links;
		void LoadFromXML(const char* filename);

	private:
		int current_id = 0;
		NodeType StringToType(const std::string& typeStr);
		void ParseElement(tinyxml2::XMLElement* elem, int parentID, float& x, float& y, int depth);
	};
}
