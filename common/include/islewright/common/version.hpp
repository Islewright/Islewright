#pragma once

#include <string_view>

namespace islewright::common {
	std::string_view project_name() noexcept;
	std::string_view project_version() noexcept;
} // namespace islewright::common
