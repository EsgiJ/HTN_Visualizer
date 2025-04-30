#include "htn/xmlgenerator/xml_generator.h"
#include <fstream>

namespace HTN::XML
{
	void XMLGenerator::AddIndent()
	{
		xml << std::string(2 * indentLevel, ' ');
	}

	void XMLGenerator::BeginTree()
	{
		xml << "<HTN>\n";
		indentLevel++;
	}

	void XMLGenerator::AddTask(const Task& task)
	{
		AddIndent();
		xml << "<Task name=\"" << task.name << "\">\n";

		indentLevel++;
		for (const auto& property: task.properties)
		{
			AddIndent();
			xml << "<Property name=\"" << property.first << "\" " << "value=\"" << property.second << "\"/>\n";
		}
		
		for (const auto& childTask : task.children)
		{
			AddTask(*childTask);
		}
		indentLevel--;
		AddIndent();
		xml << "</Task>\n";

		if (indentLevel == 1)
		{
			EndTree();
		}
	}

	void XMLGenerator::EndTree()
	{
		xml << "</HTN>\n";
	}

	std::string XMLGenerator::GetXML()
	{
		return xml.str();
	}

	void XMLGenerator::Run()
	{
		// ----------------- EXPLORATION -------------------
		auto exploration = std::make_shared<Task>("Exploration");
		exploration->properties["Priority"] = "High";

		auto scanArea = std::make_shared<Task>("ScanArea");
		scanArea->properties["ScanRadius"] = "500.0";

		auto moveToNextZone = std::make_shared<Task>("MoveToNextZone");
		moveToNextZone->properties["Speed"] = "150.0";

		exploration->children.push_back(scanArea);
		exploration->children.push_back(moveToNextZone);

		// ----------------- COMBAT -------------------
		auto combat = std::make_shared<Task>("Combat");
		combat->properties["EngagementRange"] = "300.0";

		auto fireWeapon = std::make_shared<Task>("FireWeapon");
		fireWeapon->properties["WeaponType"] = "Laser";
		fireWeapon->properties["AmmoCost"] = "5";

		combat->children.push_back(fireWeapon);

		// ----------------- RESOURCE COLLECTION -------------------
		auto resourceCollection = std::make_shared<Task>("ResourceCollection");
		resourceCollection->properties["ResourceType"] = "Energy";

		auto locateResource = std::make_shared<Task>("LocateResource");
		locateResource->properties["ScanDepth"] = "100.0";

		auto collectResource = std::make_shared<Task>("CollectResource");
		collectResource->properties["Efficiency"] = "0.9";

		resourceCollection->children.push_back(locateResource);
		resourceCollection->children.push_back(collectResource);

		// ----------------- SUPPORT ALLY -------------------
		auto supportAlly = std::make_shared<Task>("SupportAlly");
		supportAlly->properties["SupportType"] = "Heal";

		auto navigateToAlly = std::make_shared<Task>("NavigateToAlly");
		navigateToAlly->properties["Pathfinding"] = "AStar";

		auto performSupport = std::make_shared<Task>("PerformSupportAction");
		performSupport->properties["HealAmount"] = "50";

		supportAlly->children.push_back(navigateToAlly);
		supportAlly->children.push_back(performSupport);

		// ----------------- ROOT TASK -------------------
		auto root = std::make_shared<Task>("RootTask");
		root->children.push_back(exploration);
		root->children.push_back(combat);
		root->children.push_back(resourceCollection);
		root->children.push_back(supportAlly);

		// ----------------- XML GENERATION -------------------
		XMLGenerator generator;
		generator.BeginTree();
		generator.AddTask(*root);
		
		std::ofstream out("C:\\GameProjects\\HTN_Visualizer\\HTNVisualizer\\resources\\behavior.xml");
		out << generator.GetXML();
	}
}

