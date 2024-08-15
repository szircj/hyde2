
#pragma once

// stdc++
#include <string>
#include <vector>

/**************************************************************************************************/

namespace hyde {

/**************************************************************************************************/

enum ToolAccessFilter {
    ToolAccessFilterPrivate,
    ToolAccessFilterProtected,
    ToolAccessFilterPublic,
};

/**************************************************************************************************/

struct processing_options {
    std::vector<std::string> _paths;
    ToolAccessFilter _access_filter;
    std::vector<std::string> _namespace_blacklist;
    bool _process_class_methods;
};

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/
