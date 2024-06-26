
// identity
#include "namespace_matcher.hpp"

// stdc++
#include <iostream>

// clang/llvm
// clang-format off
#include "_clang_include_prefix.hpp" // must be first to disable warnings for clang headers
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "_clang_include_suffix.hpp" // must be last to re-enable warnings
// clang-format on

// application
#include "json.hpp"
#include "matchers/utilities.hpp"

using namespace clang;
using namespace clang::ast_matchers;

/**************************************************************************************************/

namespace hyde {

/**************************************************************************************************/

void NamespaceInfo::run(const MatchFinder::MatchResult& Result) {
    auto ns = Result.Nodes.getNodeAs<NamespaceDecl>("ns");
    auto info_opt = StandardDeclInfo(_options, ns);
    if (!info_opt) return;
    auto info = std::move(*info_opt);

    _j["namespaces"].push_back(std::move(info));
}

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/
