#pragma once

#include <string>

namespace FileDialog {

	// Returns empty string when dialog cancelled
	std::string Open();

	// Returns empty string when dialog cancelled
	std::string Open(char* buffer, size_t size);

	// Returns empty string when dialog cancelled
	std::string Save();

}
