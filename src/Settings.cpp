#include "Settings.h"

// https://stackoverflow.com/a/17219495
void SkipBOM(std::ifstream& in)
{
	char test[3] = { 0 };
	in.read(test, 3);
	if (static_cast<unsigned char>(test[0]) == 0xEF &&
		static_cast<unsigned char>(test[1]) == 0xBB &&
		static_cast<unsigned char>(test[2]) == 0xBF) {
		return;
	}
	in.seekg(0);
}

void Settings::ReadConfig(const std::string& a_path)
{
	logger::info("\tINI : {}", a_path);

	std::ifstream infile{ a_path };

	if (!infile.good()) {
		logger::error("\t\tcouldn't read INI");
		return;
	}

	SkipBOM(infile);

	std::string line;

	while (std::getline(infile, line)) {
		// Ignore comments and empty lines
		if (line.empty() || line[0] == ';' || line[0] == '#') {
			continue;
		}
		modWhitelist.insert(line);
	}
}

void Settings::LoadWhitelist()
{
	logger::info("{:*^30}", "INI");

	constexpr auto folderPath = R"(Data\HeadpartWhitelist)"sv;

	const std::filesystem::path folder{ folderPath };
	if (!exists(folder)) {
		logger::info("HeadpartWhitelist folder not found...");
		return;
	}

	const auto configs = dist::get_configs(folderPath);

	if (configs.empty()) {
		logger::warn("No .ini files were found in {} folder, aborting...", folderPath);
		return;
	}

	logger::info("{} matching inis found", configs.size());

	for (auto& path : configs) {
		ReadConfig(path);
	}

	logger::info("{} mods whitelisted", modWhitelist.size());
}

void Settings::WhitelistHeadparts() const
{
	std::map<std::uint32_t, std::string> blacklist;

	for (const auto& headpart : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::BGSHeadPart>()) {
		if (const auto file = headpart ? headpart->GetFile(0) : nullptr; file) {
			if (!modWhitelist.contains(file->fileName)) {
				headpart->flags.reset(RE::BGSHeadPart::Flag::kPlayable);
				blacklist.emplace(file->GetPartialIndex(), file->fileName);
			}
		}
	}

	logger::info("{:*^30}", "BLACKLISTED MODS");

	for (auto& file : blacklist | std::views::values) {
		logger::info("{}", file);
	}
}
