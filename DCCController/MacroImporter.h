// MacroImporter.h

#pragma once

#include <string>
#include <vector>

class MacroDef {
	std::wstring name;
	std::wstring ip;
	std::wstring channel;
	std::wstring address;
	std::vector<std::wstring> commands;
public:
	MacroDef();
	const std::wstring& GetName() const;
	void SetName(const std::wstring&);
	const std::wstring& GetIP() const;
	void SetIP(const std::wstring&);
	const std::wstring& GetChannel() const;
	void SetChannel(const std::wstring&);
	const std::wstring& GetAddress() const;
	void SetAddress(const std::wstring&);
	const std::vector<std::wstring>& GetCommands() const;
	void AddCommand(const std::wstring&);
	bool IsValid() const;
};

class MacroDefContainer {
	std::vector<MacroDef> macros;
public:
	MacroDefContainer();
	const std::vector<MacroDef>& Get() const;
	void Add(const MacroDef& def);
};


class MacroImporter {
	MacroDefContainer container;
	MacroImporter();
	bool ImportFile(const std::wstring& path, const std::wstring& file);
	bool ParseMacro(const std::string& text, MacroDef& macroDef) const;

	static MacroImporter* instance;
	int ExecuteCommand(int index, const std::wstring& cmd) const;
	int SendCommand(const std::wstring& ipAddress,
					const std::wstring& channel,
					const std::string& command) const;

public:
	uint32_t Import();
	int ExecuteMacro(const wchar_t*);
	const MacroDefContainer& GetContainer() const;
	static MacroImporter* GetInsance();
};