

#pragma once

// clang/llvm
// clang-format off
#include "_clang_include_prefix.hpp" // must be first to disable warnings for clang headers
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "_clang_include_suffix.hpp" // must be last to re-enable warnings
// clang-format on

// application
#include "json.hpp"
#include "matchers/matcher_fwd.hpp"

using namespace clang;
using namespace clang::ast_matchers;

/**************************************************************************************************/

namespace hyde {

/**************************************************************************************************/

class FunctionInfo : public MatchFinder::MatchCallback {
public:
    FunctionInfo(processing_options options) : _options(std::move(options)) {
        _j["functions"] = json::object();
    }

    void run(const MatchFinder::MatchResult& Result) override;

    json getJSON() { return _j; }

    static DeclarationMatcher GetMatcher() { return functionDecl().bind("func"); }

private:
    processing_options _options;
    json _j;
};

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/
