#pragma once
#include <sstream>
#include <memory>
#include <unordered_map>

namespace HTN::XML
{
	struct Task
	{
		std::string name;
		std::unordered_map<std::string, std::string> properties;
		std::vector<std::shared_ptr<Task>> children;

		Task() = default;
		Task(const std::string& taskName)
			: name(taskName) { }
	};

	class XMLGenerator
	{
		int indentLevel = 0;
		std::ostringstream xml;

		void XMLGenerator::AddIndent();
	public:
		void BeginTree();
		void AddTask(const Task& task);
		void EndTree();
		std::string GetXML();
		void Run();
	};
}
