#pragma once

class Settings : public ISingleton<Settings>
{
public:
	void LoadWhitelist();
	void WhitelistHeadparts() const;

private:
	void ReadConfig(const std::string& a_path);

    // members
	std::unordered_set<std::string> modWhitelist;
};
