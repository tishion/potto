#include <iterator>
#include <fstream>
#include <streambuf>
#include <vector>
#include <utility>
#include <rapidxml/rapidxml.hpp>
#include "PottoModulePoolManager.h"

namespace Potto
{
	PottoModulePoolManager& PottoModulePoolManager::GetInstance()
	{
		// Get the singleton instance
		static PottoModulePoolManager s_instance;
		return s_instance;
	}

	bool PottoModulePoolManager::InitFromLibraryFile(const std::string& libraryPath, const std::string& moduleRootPath)
	{
		// Validate the input parameters
		if (libraryPath.empty() || moduleRootPath.empty())
			return false;

		// Save the module root path
		m_moduleRootPath = moduleRootPath;
		if (moduleRootPath.at(moduleRootPath.size() - 1) != '\\')
			m_moduleRootPath += '\\';

		// Open file stream
		std::ifstream file(libraryPath);
		if (!file.is_open())
			return false;

		// Read file content into buffer
		std::vector<char> content(
			(std::istreambuf_iterator<char>(file)), 
			std::istreambuf_iterator<char>());
		content.push_back(0);

		// Create the xml document object
		rapidxml::xml_document<> xmlDoc;
		try
		{
			// Parse the document content
			xmlDoc.parse<0>(content.data());
		}
		catch (rapidxml::parse_error&)
		{
			// Failed to parse the file content
			return false;
		}

		// Parse the module database
		auto pLib = xmlDoc.first_node("PottoModuleLib");
		for (auto pModule = pLib->first_node("Module");
			nullptr != pModule; pModule = pModule->next_sibling())
		{
			// Get module id
			PottoUuid moduleId;
			auto attrId = pModule->first_attribute("id");
			if (attrId)
			{
				moduleId = attrId->value();
				if (moduleId.empty())
				{
					continue;
				}
			}

			// Get module name
			std::string name;
			auto attrName = pModule->first_attribute("name");
			if (attrName)
			{
				name = attrName->value();
				if (name.empty())
				{
					continue;
				}
			}

			// Get module path
			std::string modulePath;
			auto attrPath = pModule->first_attribute("path");
			if (attrPath)
			{
				modulePath = attrPath->value();
				if (modulePath.empty())
				{
					continue;
				}
			}
			if (modulePath.at(0) == '\\')
				modulePath.erase(modulePath.begin());

			// Insert path into list
			m_modulePathList.push_back(modulePath);
			size_t index = m_modulePathList.size() - 1;

			// Build lookup map for the classes in this module
			for (auto pClass = pModule->first_node("Class");
				nullptr != pClass; pClass = pClass->next_sibling())
			{
				PottoUuid classId;
				auto attrId = pClass->first_attribute("id");
				if (attrId)
				{
					classId = attrId->value();
					if (classId.empty())
					{
						continue;
					}
				}

				m_modulePathLookupMap.insert(
					std::pair<const PottoUuid, const std::string&>(classId, m_modulePathList[index]));
			}
		}

		return true;
	}

	const std::string PottoModulePoolManager::GetModulePathByClassId(const PottoUuid& id) const
	{
		// Find the module path from the lookup map
		static std::string empty;
		auto it = m_modulePathLookupMap.end();
		{
			std::lock_guard<std::mutex> lock(m_mtxFormodulePathLookupMap);
			it = m_modulePathLookupMap.find(id);
		}

		// If the module was found then build and return the full path
		if (it != m_modulePathLookupMap.end())
		{
			std::string modulePath = m_moduleRootPath + it->second;
			return modulePath;
		}
		
		// Return empty string if the module was not found
		return empty;
	}

	PottoModulePoolManager::PottoModulePoolManager()
	{
	}


	PottoModulePoolManager::~PottoModulePoolManager()
	{
		// Clean the lookup map
		{
			std::lock_guard<std::mutex> lock(m_mtxFormodulePathLookupMap);
			m_modulePathLookupMap.clear();
		}

		// Clean the module path list
		m_modulePathList.clear();
	}
}