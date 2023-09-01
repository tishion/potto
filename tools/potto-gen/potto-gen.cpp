// PottoGen.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32)
#include <Windows.h>
#include <shlwapi.h>

#define PTModHandle HMODULE
#define PTLoadModule ::LoadLibraryA
#define PTGetProcAddr ::GetProcAddress
#define PTFreeModule ::FreeLibrary
#define PTStrCmpNoCase _stricmp
#else
#include <dlfcn.h>
#include <string.h>

#define PTModHandle void*
#define PTLoadModule ::dlopen
#define PTGetProcAddr ::dlsym
#define PTFreeModule ::dlclose
#define PTStrCmpNoCase strcasecmp
#endif

#if defined(_WIN32)
#define PTModuleExtension ".dll"
#elif defined(__APPLE__)
#define PTModuleExtension ".dylib"
#else
#define PTModuleExtension ".so"
#endif

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#if defined(CXX_17_ENABLED)
#include <filesystem>
#define fsns std
#else
#include <ghc/filesystem.hpp>
#define fsns ghc
#endif
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include <potto/module/pottodef.hpp>
#include <potto/pottouuid.hpp>

bool GetModuleIdAndClassInfo(const std::string& inputModuleFile, Potto::PottoUuid& moduleId,
                             Potto::ClassInfoList& classInfoList) {
  PTModHandle hMod = nullptr;
#if defined(_WIN32)
  hMod = PTLoadModule(inputModuleFile.c_str());
#else
  hMod = PTLoadModule(inputModuleFile.c_str(), RTLD_LAZY);
#endif
  if (nullptr == hMod)
    return false;

  Potto::TypeRegisterModule pfnRegisterModule =
      (Potto::TypeRegisterModule)PTGetProcAddr(hMod, RegisterModuleName);
  if (!pfnRegisterModule) {
    PTFreeModule(hMod);
    return false;
  }

  Potto::POTTO_ERROR error = pfnRegisterModule(moduleId, classInfoList);
  if (Potto::POTTO_E_OK != error)
    return false;

  return true;
}

void GenerateClassIdHeaderFile(const std::string& inputFile, const std::string& outputFolder,
                               const std::string& outputFileName) {
  if (inputFile.empty())
    return;

  fsns::filesystem::path inputFilePath = inputFile;
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

  fsns::filesystem::path outputFilePath;
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

  fsns::filesystem::path inputFolderPath = inputFolder;

  rapidxml::xml_document<> doc;

  // xml declaration
  rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
  decl->append_attribute(doc.allocate_attribute("version", "1.0"));
  decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
  doc.append_node(decl);

  // root node
  rapidxml::xml_node<>* pLib = doc.allocate_node(rapidxml::node_element, "PottoModuleLib");
  doc.append_node(pLib);

  for (fsns::filesystem::recursive_directory_iterator it(inputFolderPath);
       it != fsns::filesystem::recursive_directory_iterator(); it++) {
    std::string extension = it->path().extension().string();
    if (PTStrCmpNoCase(extension.c_str(), PTModuleExtension)) {
      continue;
    }

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

  fsns::filesystem::path outputFilePath;
  if (outputFile.empty()) {
    outputFilePath = inputFolderPath;
    outputFilePath /= "modulelib.xml";
  } else {
    outputFilePath = outputFile;
  }

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

void ShowHelp() {
  // clang-format off
  std::cout
      << "Usage:" << std::endl
      << " - Generate header file for specified module file:" << std::endl
      << "pt-gen -classid <input file> [output folder] [output name]" << std::endl
      << "<input file>: " << std::endl 
      << "  mandatory, full path name of the potto module file" << std::endl
      << "[output folder]: " << std::endl 
      << "  optional, full path to the folder for the output header file, if not provided the parent folder of the input file will be used" << std::endl
      << "[output name]:" << std::endl 
      << "  optional, the output file name, if not provided then the output file will be named as the input file name (without extension) with a suffix '_CLSID.h'" << std::endl
      << std::endl
      << " - Generate module library database file from module folder:" << std::endl
      << "pt-gen -modulelib <input folder> <output file>" << std::endl
      << "<input folder>: " << std::endl 
      << "  mandatory, full path to the folder which contains the potto module files" << std::endl
      << "<output file>: " << std::endl 
      << "  mandatory, full path name of the output library database file" << std::endl
      ;
  // clang-format on
}

// pt-gen -classid <input file> [output folder] [output file name]
// pt-gen -modulelib <input folder> <output file>
int main(int argc, char* argv[]) {
  if (argc > 1) {
    if (0 == PTStrCmpNoCase(argv[1], ARG_CLASSID)) {
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
        std::cout << "+++                              Potto Code Generator                        "
                     "       "
                  << std::endl;
        GenerateClassIdHeaderFile(inputFile, outputFolder, outputFileName);
        std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                     "+++++++"
                  << std::endl;
        return 0;
      }
    } else if (0 == PTStrCmpNoCase(argv[1], ARG_MODULELIB)) {
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

  ShowHelp();
  return 1;
}
