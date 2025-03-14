#pragma once
#include <vector>
#include <sstream>
#include <filesystem>
#include <codecvt>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <stack>

void processPreprocessors(std::filesystem::path& mainFilePath, std::u8string& outStr, std::vector<std::filesystem::path>& includingStack, std::vector<std::filesystem::path>& outLibraries);