#pragma once

// clang/llvm
// clang-format off
#include "_clang_include_prefix.hpp" // must be first to disable warnings for clang headers
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/ADT/ArrayRef.h"
#include "_clang_include_suffix.hpp" // must be last to re-enable warnings
// clang-format on

// application
#include "json.hpp"
#include "matchers/matcher_fwd.hpp"

/**************************************************************************************************/

namespace hyde {

/**************************************************************************************************/

json GetParentNamespaces(const clang::ASTContext* n, const clang::Decl* d);

json GetParentCXXRecords(const clang::ASTContext* n, const clang::Decl* d);

json GetTemplateParameters(const clang::ASTContext* n, const clang::TemplateDecl* d);

optional_json DetailFunctionDecl(const hyde::processing_options& options,
                                 const clang::FunctionDecl* f);

optional_json DetailCXXRecordDecl(const hyde::processing_options& options,
                                  const clang::CXXRecordDecl* cxx);

bool PathCheck(const std::vector<std::string>& paths, const clang::Decl* d, clang::ASTContext* n);

bool AccessCheck(ToolAccessFilter hyde_filter, clang::AccessSpecifier clang_access);

bool NamespaceBlacklist(const std::vector<std::string>& blacklist, const json& j);

std::string GetArgumentList(const llvm::ArrayRef<clang::NamedDecl*> args);

std::string ReplaceAll(std::string str, const std::string& substr, const std::string& replacement);

// type-parameter-N-M filtering.
std::string PostProcessType(const clang::Decl* decl, std::string type);

// Doxygen-style comments.
optional_json ProcessComments(const clang::Decl* d);

/**************************************************************************************************/

inline std::string to_string(clang::AccessSpecifier access) {
    switch (access) {
        case clang::AccessSpecifier::AS_public:
            return "public";
        case clang::AccessSpecifier::AS_protected:
            return "protected";
        case clang::AccessSpecifier::AS_private:
            return "private";
        case clang::AccessSpecifier::AS_none:
            return "none";
    }
    return "unknown";
}

/**************************************************************************************************/

inline std::string to_string(const clang::Decl* decl, clang::QualType type) {
    static const clang::PrintingPolicy policy(decl->getASTContext().getLangOpts());
    std::string result = PostProcessType(decl, type.getAsString(policy));
    bool is_lambda = result.find("(lambda at ") == 0;
    return is_lambda ? "__lambda" : result;
}

/**************************************************************************************************/

template <typename DeclarationType>
optional_json StandardDeclInfo(const hyde::processing_options& options, const DeclarationType* d) {
    clang::ASTContext* n = &d->getASTContext();

    if (!PathCheck(options._paths, d, n)) return std::nullopt;

    json info = json::object();

    info["name"] = d->getNameAsString();
    info["namespaces"] = GetParentNamespaces(n, d);
    info["parents"] = GetParentCXXRecords(n, d);
    info["qualified_name"] = d->getQualifiedNameAsString();

    if (NamespaceBlacklist(options._namespace_blacklist, info)) return std::nullopt;

    auto clang_access = d->getAccess();

    if (!AccessCheck(options._access_filter, clang_access)) return std::nullopt;

    if (auto comments = ProcessComments(d)) {
        info["comments"] = std::move(*comments);
    }

    if (clang_access != clang::AccessSpecifier::AS_none) info["access"] = to_string(clang_access);

    info["defined_in_file"] = [&] {
        auto beginLoc = d->getBeginLoc();
        auto location = beginLoc.printToString(n->getSourceManager());
        return location.substr(0, location.find(':'));
    }();

    info["deprecated"] = false;

    if (auto attr = d->template getAttr<clang::DeprecatedAttr>()) {
        info["deprecated"] = true;
        auto message = attr->getMessage();
        info["deprecated_message"] = message.str();
    }

    return info;
}

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/
