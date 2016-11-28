// PottoGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <string.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <potto/pottouuid.hpp>
#include <potto/internal/pottodef.hpp>


bool GetModuleIdAndClassInfo(const std::string& inputModuleFile, Potto::PottoUuid& moduleId, Potto::ClassInfoList& classInfoList)
{
	HMODULE hMod = ::LoadLibraryA(inputModuleFile.c_str());
	if (nullptr == hMod)
		return false;

	Potto::TypeRegisterModule pfnRegisterModule = (Potto::TypeRegisterModule)
		::GetProcAddress(hMod, RegisterModuleName);
	if (!pfnRegisterModule)
	{
		::FreeLibrary(hMod);
		return false;
	}

	POTTO_ERROR error = pfnRegisterModule(moduleId, classInfoList);
	if (POTTO_E_OK != error)
		return false;

	return true;
}

void GenerateClassIdHeaderFile(const std::string& inputFile, const std::string outputFolder)
{
	if (inputFile.empty() || outputFolder.empty())
		return;

	Potto::PottoUuid moduleId;
	Potto::ClassInfoList classInfoList;
	GetModuleIdAndClassInfo(inputFile, moduleId, classInfoList);

	std::tr2::sys::path inputFilePath = inputFile;
	std::string fileName = inputFilePath.filename().replace_extension("").string() + "_CLSID";

	std::stringstream oss;
	oss << "#ifndef " << fileName << "_H_" << std::endl;
	oss << "#define " << fileName << "_H_" << std::endl;
	oss << "#pragma once" << std::endl;
	for (const Potto::ClassInfo& classInfo : classInfoList)
	{
		oss << "const char* const CLSID_" << classInfo.Name << " = " << "\"" << classInfo.Id.ToString() << "\";" << std::endl;
		std::cout << "+++ PROCESSED: Class " << classInfo.Name << ", id = " << classInfo.Id.ToString() << std::endl;
	}
	oss << std::endl;
	oss << "#endif // " << fileName << "_H_";

	fileName += ".h";
	std::tr2::sys::path outputFilePath = inputFilePath.replace_filename(fileName);

	std::cout << "+++ Output File: " << outputFilePath << std::endl;
	std::ofstream outputFile;
	outputFile.open(outputFilePath);
	outputFile << oss.str();
	outputFile.close();

	return;
}

void GenerateModuleLibXmlFile(const std::string& inputFolder, const std::string& outputFile)
{
	if (inputFolder.empty() || outputFile.empty())
		return;

	std::string input = inputFolder;
	auto lastChar = *(input.rbegin());
	if (lastChar != '\\')
		input.append("\\");

	std::tr2::sys::path inputFolderPath = input;

	rapidxml::xml_document<> doc;

	// xml declaration
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	// root node
	rapidxml::xml_node<>* pLib = doc.allocate_node(rapidxml::node_element, "PottoModuleLib");
	doc.append_node(pLib);

	char* atrribute_value = nullptr;

	for (std::tr2::sys::recursive_directory_iterator it(inputFolderPath);
		it != std::tr2::sys::recursive_directory_iterator(); it++)
	{
		std::string extension = it->path().extension().string();
		if (_stricmp(extension.c_str(), ".dll"))
			continue;

		std::string fileName = it->path().filename().string();

		std::string relativePath = it->path().string();
		relativePath.erase(relativePath.begin(), relativePath.begin() + inputFolderPath.string().size());

		Potto::PottoUuid moduleId;
		Potto::ClassInfoList classInfoList;
		GetModuleIdAndClassInfo(it->path().string(), moduleId, classInfoList);

		rapidxml::xml_node<>* pModule = doc.allocate_node(rapidxml::node_element, "Module");
		atrribute_value = doc.allocate_string(moduleId.c_str());
		pModule->append_attribute(doc.allocate_attribute("id", atrribute_value));

		atrribute_value = doc.allocate_string(fileName.c_str());
		pModule->append_attribute(doc.allocate_attribute("name", atrribute_value));

		atrribute_value = doc.allocate_string(relativePath.c_str());
		pModule->append_attribute(doc.allocate_attribute("path", atrribute_value));

		pLib->append_node(pModule);

		for (const Potto::ClassInfo& classInfo : classInfoList)
		{
			rapidxml::xml_node<>* pClass = doc.allocate_node(rapidxml::node_element, "Class");

			atrribute_value = doc.allocate_string(classInfo.Id.c_str());
			pClass->append_attribute(doc.allocate_attribute("id", atrribute_value));

			atrribute_value = doc.allocate_string(classInfo.Name.c_str());
			pClass->append_attribute(doc.allocate_attribute("name", atrribute_value));
			pModule->append_node(pClass);
		}
	}

	std::ofstream of(outputFile);
	of << doc;
	of.close();
}

#define ARG_CLASSID		"-classid"
#define ARG_MODULELIB	"-modulelib"
#define ARG_ROOT		"-root"

// pg.exe -classid <input file> <output folder>
// pg.exe -modulelib <input folder> <output file>
int main(int argc, char *argv[])
{
	if (argc <= 1) return -1;

	for (int i = 1; i < argc; i++)
	{
		if (0 == _stricmp(argv[1], ARG_CLASSID))
		{
			if (argc == 4)
			{
				std::string inputFile = argv[2];
				std::string outputFolder = argv[3];
				std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
				std::cout << "+++                              Potto code generator                               " << std::endl;
				GenerateClassIdHeaderFile(inputFile, outputFolder);
				std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
				return 0;
			}
			return -1;
		}
		else if (0 == _stricmp(argv[1], ARG_MODULELIB))
		{
			if (argc == 4)
			{
				std::string inputFolder = argv[2];
				std::string outputFile = argv[3];
				std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
				std::cout << "+++                              Potto code generator                               " << std::endl;
				GenerateModuleLibXmlFile(inputFolder, outputFile);
				std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
				return 0;
			}
			return -1;
		}
		else
			return -1;
	}

    return 0;
}

