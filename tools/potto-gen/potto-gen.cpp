// PottoGen.cpp : Defines the entry point for the console application.
//

#include <Windows.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include <potto/module/pottodef.hpp>
#include <potto/pottouuid.hpp>

bool GetModuleIdAndClassInfo(const std::string& inputModuleFile, Potto::PottoUuid& moduleId,
                             Potto::ClassInfoList& classInfoList) {
  HMODULE hMod = ::LoadLibraryA(inputModuleFile.c_str());
  if (nullptr == hMod)
    return false;

  Potto::TypeRegisterModule pfnRegisterModule =
      (Potto::TypeRegisterModule)::GetProcAddress(hMod, RegisterModuleName);
  if (!pfnRegisterModule) {
    ::FreeLibrary(hMod);
    return false;
  }

  Potto::POTTO_ERROR error = pfnRegisterModule(moduleId, classInfoList);
  if (Potto::POTTO_E_OK != error)
    return false;

  return true;
}

void GenerateClassIdHeaderFile(const std::string& inputFile, const std::string outputFolder,
                               const std::string outputFileName) {
  if (inputFile.empty())
    return;

  std::tr2::sys::path inputFilePath = inputFile;
  std::string fileName = outputFileName.empty()
                             ? inputFilePath.filename().replace_extension("").string() + "_CLSID.h"
                             : outputFileName;

  Potto::PottoUuid moduleId;
  Potto::ClassInfoList classInfoList;
  GetModuleIdAndClassInfo(inputFile, moduleId, classInfoList);

  std::cout << "+++ Found " << classInfoList.size() << " class(es) in module "
            << moduleId.ToString() << std::endl;

  std::string headerMacroName;
  std::string headerMacro = std::string("POTTO_MODULE_") + moduleId.ToString() + "_H_";
  std::for_each(headerMacro.begin(), headerMacro.end(), [&headerMacroName](const char& c) {
    headerMacroName.push_back('-' == c ? '_' : std::toupper(c));
  });

  std::stringstream oss;
  oss << "#ifndef " << headerMacroName << std::endl;
  oss << "#define " << headerMacroName << std::endl;
  oss << "#pragma once" << std::endl;
  for (const Potto::ClassInfo& classInfo : classInfoList) {
    oss << "const char* const CLSID_" << classInfo.Name << " = "
        << "\"" << classInfo.Id.ToString() << "\";" << std::endl;
    std::cout << "+++   " << classInfo.Id.ToString() << " -> " << classInfo.Name << std::endl;
  }
  oss << std::endl;
  oss << "#endif // " << headerMacroName;

  std::tr2::sys::path outputFilePath;
  if (outputFolder.empty())
    outputFilePath = inputFilePath.replace_filename(fileName);
  else {
    outputFilePath = outputFolder;
    outputFilePath /= fileName;
  }

  std::cout << "+++ Output File: " << outputFilePath << std::endl;
  try {
    std::ofstream ofs(outputFilePath, std::ios_base::trunc);
    ofs << oss.str();
    ofs.close();
  } catch (const std::exception& e) {
    std::cout << "+++ Failed to write file: " << e.what() << std::endl;
  }

  return;
}

void GenerateModuleLibXmlFile(const std::string& inputFolder, const std::string& outputFile) {
  if (inputFolder.empty())
    return;

  std::tr2::sys::path inputFolderPath = inputFolder;

  rapidxml::xml_document<> doc;

  // xml declaration
  rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
  decl->append_attribute(doc.allocate_attribute("version", "1.0"));
  decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
  doc.append_node(decl);

  // root node
  rapidxml::xml_node<>* pLib = doc.allocate_node(rapidxml::node_element, "PottoModuleLib");
  doc.append_node(pLib);

  for (std::tr2::sys::recursive_directory_iterator it(inputFolderPath);
       it != std::tr2::sys::recursive_directory_iterator(); it++) {
    std::string extension = it->path().extension().string();
    if (_stricmp(extension.c_str(), ".dll"))
      continue;

    std::string fileName = it->path().filename().string();

    std::cout << "+++ Proceeding module: " << fileName << std::endl;

    std::string relativePath = it->path().string();
    relativePath.erase(relativePath.begin(),
                       relativePath.begin() + inputFolderPath.string().size());

    Potto::PottoUuid moduleId;
    Potto::ClassInfoList classInfoList;
    GetModuleIdAndClassInfo(it->path().string(), moduleId, classInfoList);

    if (moduleId.ToString().empty())
      continue;

    std::cout << "+++   Found " << classInfoList.size() << " class(es) in module "
              << moduleId.ToString() << std::endl;

    char* atrribute_value = nullptr;

    rapidxml::xml_node<>* pModule = doc.allocate_node(rapidxml::node_element, "Module");
    atrribute_value = doc.allocate_string(moduleId.ToString().c_str());
    pModule->append_attribute(doc.allocate_attribute("id", atrribute_value));

    atrribute_value = doc.allocate_string(fileName.c_str());
    pModule->append_attribute(doc.allocate_attribute("name", atrribute_value));

    atrribute_value = doc.allocate_string(relativePath.c_str());
    pModule->append_attribute(doc.allocate_attribute("path", atrribute_value));

    pLib->append_node(pModule);

    for (const Potto::ClassInfo& classInfo : classInfoList) {
      rapidxml::xml_node<>* pClass = doc.allocate_node(rapidxml::node_element, "Class");

      atrribute_value = doc.allocate_string(classInfo.Id.ToString().c_str());
      pClass->append_attribute(doc.allocate_attribute("id", atrribute_value));

      atrribute_value = doc.allocate_string(classInfo.Name.c_str());
      pClass->append_attribute(doc.allocate_attribute("name", atrribute_value));
      pModule->append_node(pClass);

      std::cout << "+++       " << classInfo.Id.ToString() << " -> " << classInfo.Name << std::endl;
    }
  }

  std::tr2::sys::path outputFilePath;
  if (outputFile.empty()) {
    outputFilePath = inputFolderPath;
    outputFilePath /= "modulelib.xml";
  } else
    outputFilePath = outputFile;

  std::cout << "+++ Output File: " << outputFilePath << std::endl;
  try {
    std::ofstream ofs(outputFilePath, std::ios_base::trunc);
    ofs << doc;
    ofs.close();
  } catch (const std::exception& e) {
    std::cout << "+++ Failed to write file: " << e.what() << std::endl;
  }

  return;
}

#define ARG_CLASSID "-classid"
#define ARG_MODULELIB "-modulelib"
#define ARG_ROOT "-root"

// pg.exe -classid {input file} {output folder} [output file name]
// pg.exe -modulelib <input folder> <output file>
int main(int argc, char* argv[]) {
  if (argc > 1) {
    if (0 == _stricmp(argv[1], ARG_CLASSID)) {
      if (argc >= 3) {
        std::string inputFile = argv[2];
        std::string outputFolder;
        std::string outputFileName;
        if (argc >= 4)
          outputFolder = argv[3];
        if (argc >= 5)
          outputFileName = argv[4];
        std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                     "+++++++"
                  << std::endl;
        std::cout << "+++                              Potto code generator                        "
                     "       "
                  << std::endl;
        GenerateClassIdHeaderFile(inputFile, outputFolder, outputFileName);
        std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                     "+++++++"
                  << std::endl;
        return 0;
      }
    } else if (0 == _stricmp(argv[1], ARG_MODULELIB)) {
      if (argc >= 3) {
        std::string inputFolder = argv[2];
        std::string outputFile;
        if (argc >= 4)
          outputFile = argv[3];
        std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                     "+++++++"
                  << std::endl;
        std::cout << "+++                              Potto code generator                        "
                     "       "
                  << std::endl;
        GenerateModuleLibXmlFile(inputFolder, outputFile);
        std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                     "+++++++"
                  << std::endl;
        return 0;
      }
    }
  }

  return 1;
}
