#include "islewright/common/version.hpp"

namespace islewright::common {
	std::string_view project_name() noexcept
	{
		return "Islewright";
	}

	std::string_view project_version() noexcept
	{
		return "0.1.0";
	}
} // namespace islewright::common
