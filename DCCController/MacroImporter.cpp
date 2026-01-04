// MacroImporter.cpp

#include <Windows.h>
#include <cwchar>
#include <format>
#include <string>


#include "MacroImporter.h"
#include "Win32Utilities.h"
#include "SendURL.h"
#include "MapCommand.h"

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
			for (const auto& cmd : container.Get()[i].GetCommands()) {
				OutputDebugString(L"\r\n");
				OutputDebugString(cmd.c_str());
				ExecuteCommand(i, cmd);
			}
			return 0;
		}
	}
	return -1;
}

int MacroImporter::ExecuteCommand(int index, const std::wstring& cmd) const
{
	const std::wstring& ipAddress = container.Get()[index].GetIP();
	const std::wstring& channel = container.Get()[index].GetChannel();
	const std::wstring& devAddress = container.Get()[index].GetAddress();

	int n1 = 0;
	int v1 = 0;
	int ret = 0;
	bool status = false;
	bool direction = false; // false = forward, true = backward
	int channelNr = std::stoi(channel);

	//ret = swscanf_s(cmd.c_str(), L"#Channel=%d", &channelNr);
	//int dirValue = direction ? 0x40 : 0;
	//if (ret == 1) {
	//	return 0;
	//}

	ret = swscanf_s(cmd.c_str(), L"#F%d=On", &n1);
	if (ret == 1) 
		status = true;
	else {
		ret = swscanf_s(cmd.c_str(), L"#F%d=Off", &n1);
		if (ret == 1) status = false;
	}
	if (ret == 1) {
		int devNum = std::stoi(devAddress);
		//std::wstring funcNum = n1;
		std::string func = ToggleFunction(channelNr,n1,status?1:0);
		std::string command = FormatString("%X%s", devNum, func.c_str());
		if (command.length() % 2 == 1)
			command = std::string("0") + command;
		AppendTimeStamp(command);
		return SendCommand(ipAddress, channel, command);
	}

	size_t pos = cmd.find(L"#Forward=True");
	if (pos != std::string::npos) {
		direction = false;
		return 0;
	}
	pos = cmd.find(L"#Forward=False");
	if (pos != std::string::npos) {
		direction = true;
		return 0;
	}

	ret = swscanf_s(cmd.c_str(), L"#Speed=%d", &v1);
	int dirValue = direction ? 0x40 : 0;
	if (ret == 1) {
		if (v1 >= 0 && v1 <= 28) {
			int devAddrNum = std::stoi(devAddress);
			std::string command = FormatString("%02X%02X", devAddrNum, v1 + dirValue);
			AppendTimeStamp(command);
			return SendCommand(ipAddress, channel, command);
		}
		return 0; // speed setting
	}
	double dt = 0.0;
	ret = swscanf_s(cmd.c_str(), L"#Delay=%lf", &dt);
	if (ret == 1) {
		DWORD ti = (DWORD)(dt * 1000.0);
		// speed settingSleep((DWORD)(dt * 1000.0));
		Sleep(ti);
		return 0;
	}

	return -1;
}

int MacroImporter::SendCommand(
	const std::wstring& ipAddress,
	const std::wstring& channel,
	const std::string& command) const
{
	std::string strReceive;
	SendURL(WStringToString(ipAddress).c_str(), WStringToString(channel).c_str(), command.c_str(), strReceive, strReceive);
	return 0;
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
			pos = wstr.find(L"#Channel=");
			if (pos != std::string::npos) {
				pos = wstr.find(L"=");
				pos++;
				macroDef.SetChannel(wstr.substr(pos, pos2));
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

