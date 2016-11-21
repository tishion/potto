#include <iterator>
#include <fstream>
#include <streambuf>
#include <vector>
#include "PottoModulePoolManager.h"
#include "rapidxml/rapidxml.hpp"
#include <utility>

namespace Potto
{
	PottoModulePoolManager& PottoModulePoolManager::GetInstance()
	{
		static PottoModulePoolManager s_instance;
		return s_instance;
	}

	bool PottoModulePoolManager::InitFromLibraryFile(const std::string& libraryPath, const std::string& moduleRootPath)
	{
		if (libraryPath.empty() || moduleRootPath.empty())
			return false;

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

		auto pLib = xmlDoc.first_node("PottoModuleLib");
		for (auto pModule = pLib->first_node("Module");
			nullptr != pModule; pModule = pModule->next_sibling())
		{
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

				m_modulePathLookupMap.insert(std::pair<const PottoUuid, const std::string&>(
					classId, m_modulePathList[index]));
			}
		}

		return true;
	}

	const std::string PottoModulePoolManager::GetModulePathByClassId(const PottoUuid& id) const
	{
		static std::string empty;
		auto it = m_modulePathLookupMap.end();
		{
			std::lock_guard<std::mutex> lock(m_mtxFormodulePathLookupMap);
			it = m_modulePathLookupMap.find(id);
		}

		if (it != m_modulePathLookupMap.end())
		{
			std::string modulePath = m_moduleRootPath + it->second;
			return modulePath;
		}
		
		return empty;
	}

	PottoModulePoolManager::PottoModulePoolManager()
	{
	}


	PottoModulePoolManager::~PottoModulePoolManager()
	{
		{
			std::lock_guard<std::mutex> lock(m_mtxFormodulePathLookupMap);
			m_modulePathLookupMap.clear();
		}

		m_modulePathList.clear();
	}
}