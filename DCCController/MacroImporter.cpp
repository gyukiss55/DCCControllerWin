// MacroImporter.cpp

#include <Windows.h>

#include "MacroImporter.h"
#include "Win32Utilities.h"

MacroImporter* MacroImporter::instance = nullptr;

MacroDef::MacroDef()
{

}

const std::wstring& MacroDef::GetName() const
{
	return name;
}

void MacroDef::SetName(const std::wstring& s)
{
	name = s;
}

const std::wstring& MacroDef::GetIP() const
{
	return ip;
}

void MacroDef::SetIP(const std::wstring& s)
{
	ip = s;
}

const std::wstring& MacroDef::GetChannel() const
{
	return channel;
}

void MacroDef::SetChannel(const std::wstring& s)
{
	channel = s;
}

const std::wstring& MacroDef::GetAddress() const
{
	return address;
}

void MacroDef::SetAddress(const std::wstring& s)
{
	address = s;
}

const std::vector<std::wstring>& MacroDef::GetCommands() const
{
	return commands;
}

void MacroDef::AddCommand(const std::wstring& c)
{
	commands.push_back(c);
}

bool MacroDef::IsValid() const
{
	return true;
}

MacroDefContainer::MacroDefContainer()
{
}

const std::vector<MacroDef>& MacroDefContainer::Get() const
{
	return macros;
}
void MacroDefContainer::Add(const MacroDef& def)
{
	macros.push_back(def);
}


MacroImporter::MacroImporter()
{
}

uint32_t MacroImporter::Import()
{
	std::wstring path = GetCurrentPath();
	path += L"\\..\\Macros";
	OutputDebugString(path.c_str());
	OutputDebugStringA("\r\n");
	std::vector<std::wstring> files;
	EnumerateFilesInDirectory(path.c_str(), files);
	for (auto f : files) {
		OutputDebugString(f.c_str ());
		OutputDebugStringA("\r\n");
		ImportFile(path, f);
	}
	return container.Get().size();
}

int MacroImporter::ExecuteMacro(const wchar_t*name)
{
	for (int i = 0; i < container.Get().size(); ++i)
	{
		const std::wstring& nameItem = container.Get()[i].GetName();
		if (wcscmp(nameItem.c_str(), name) == 0) {
			OutputDebugString(L"Execute:");
			OutputDebugString(name);
			return 0;
		}
	}
	return -1;
}


const MacroDefContainer& MacroImporter::GetContainer() const
{
	return container;
}

bool MacroImporter::ParseMacro(const std::string& text, MacroDef& macroDef) const
{
	macroDef = MacroDef();
	std::vector<std::string> items = SplitString(text, '\n');
	if (items.size() < 5)
		return false;
	size_t pos = items[0].find("#PIKOMacro");
	int status = 0;
	if (pos != std::string::npos) {
		for (int i = 1; i < items.size(); ++i) {
			std::wstring wstr = ConvertUTF8ToUnicode(items[i]);
			pos = wstr.find(L"#Name=");
			size_t pos2 = wstr.find(L"\r");
			if (pos2 != std::string::npos)
				pos2--;
			if (pos != std::string::npos) {
				pos = wstr.find(L"=");
				pos++;
				macroDef.SetName(wstr.substr(pos, pos2));
				status |= 1;
				continue;
			}
			pos = wstr.find(L"#IP=");
			if (pos != std::string::npos) {
				pos = wstr.find(L"=");
				pos++;
				macroDef.SetIP(wstr.substr(pos, pos2));
				status |= 2;
				continue;
			}
			pos = wstr.find(L"#Address=");
			if (pos != std::string::npos) {
				pos = wstr.find(L"=");
				pos++;
				macroDef.SetAddress(wstr.substr(pos, pos2));
				status |= 4;
				continue;
			}
			pos = wstr.find(L"#");
			if (pos != std::string::npos) {
				macroDef.AddCommand(wstr);
				status |= 8;
				continue;
			}
		}
	}
	return status == 0xf;
}

bool MacroImporter::ImportFile(const std::wstring& pathIn, const std::wstring& file)
{
	std::wstring path(pathIn);
	path += L"\\" + file;
	std::string text;
	ReadTextFile(path.c_str(), text);
	OutputDebugStringA(text.c_str());

	MacroDef macroDef;
	bool ret = ParseMacro(text, macroDef);
	if (ret && macroDef.IsValid()) {
		container.Add(macroDef);
	}
	return true;
}


MacroImporter* MacroImporter::GetInsance()
{
	if (instance == nullptr) {
		instance = new MacroImporter;
	}
	return instance;
}

