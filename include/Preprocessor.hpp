#pragma once
#include <vector>
#include <sstream>
#include <filesystem>
#include <codecvt>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <iostream>

void processPreprocessors(std::filesystem::path& mainFilePath, std::u8string& outStr);