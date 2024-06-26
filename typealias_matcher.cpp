
// identity
#include "typealias_matcher.hpp"

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

void TypeAliasInfo::run(const MatchFinder::MatchResult& Result) {
    auto node = Result.Nodes.getNodeAs<TypeAliasDecl>("typealias");

    auto info_opt = StandardDeclInfo(_options, node);
    if (!info_opt) return;
    auto info = std::move(*info_opt);

    // do not process class type aliases here.
    if (!info["parents"].empty()) return;

    info["type"] = hyde::to_string(node, node->getUnderlyingType());

    if (auto template_decl = node->getDescribedAliasTemplate()) {
        info["template_parameters"] = GetTemplateParameters(Result.Context, template_decl);
    }

    _j["typealiases"].push_back(std::move(info));
}

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/
