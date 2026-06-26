#include "islewright/common/version.hpp"

#include <iostream>

int main()
{
	std::cout << islewright::common::project_name() << " server "
		<< islewright::common::project_version() << '\n';
	return 0;
}
