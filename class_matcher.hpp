
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

class ClassInfo : public MatchFinder::MatchCallback {
public:
    explicit ClassInfo(processing_options options) : _options(std::move(options)) {
        _j["class"] = json::array();
    }

    void run(const MatchFinder::MatchResult& Result) override;

    json getJSON() { return _j; }

    static DeclarationMatcher GetMatcher() { return cxxRecordDecl().bind("class"); }

private:
    processing_options _options;
    json _j;
};

/**************************************************************************************************/

} // namespace hyde

/**************************************************************************************************/

enum class signature_options : std::uint8_t {
    none = 0,
    fully_qualified = 1 << 0,
    named_args = 1 << 1,
};

template <typename T>
bool flag_set(const T& value, const T& flag) {
    using type = std::underlying_type_t<T>;
    return (static_cast<type>(value) & static_cast<type>(flag)) != 0;
}

/**************************************************************************************************/
// See DeclPrinter::VisitFunctionDecl in clang/lib/AST/DeclPrinter.cpp for hints
// on how to make this routine better.
std::string GetSignature(const ASTContext* n,
                         const FunctionDecl* function,
                         signature_options options = signature_options::none) {
    if (!function) return "";

    bool fully_qualified = flag_set(options, signature_options::fully_qualified);
    bool named_args = flag_set(options, signature_options::named_args);
    bool isTrailing = false;
    std::stringstream signature;

    if (const auto* fp = function->getType()->getAs<FunctionProtoType>()) {
        isTrailing = fp->hasTrailingReturn();
    }

    if (auto template_decl = function->getDescribedFunctionTemplate()) {
        signature << to_string(n, template_decl);
    }

    if (auto ctor_decl = llvm::dyn_cast_or_null<CXXConstructorDecl>(function)) {
        auto specifier = ctor_decl->getExplicitSpecifier();
        if (specifier.isExplicit()) signature << "explicit ";
    } else if (auto conversion_decl = llvm::dyn_cast_or_null<CXXConversionDecl>(function)) {
        auto specifier = conversion_decl->getExplicitSpecifier();
        if (specifier.isExplicit()) signature << "explicit ";
    }

    if (!isa<CXXConstructorDecl>(function) && !isa<CXXDestructorDecl>(function) &&
        !isa<CXXConversionDecl>(function)) {
        if (function->isConstexpr()) {
            signature << "constexpr ";
        }

        switch (function->getStorageClass()) {
            case SC_Static:
                signature << "static ";
                break;
            case SC_Extern:
                signature << "extern ";
                break;
            default:
                break;
        }

        if (isTrailing) {
            signature << "auto ";
        } else {
            signature << hyde::to_string(function, function->getReturnType()) << " ";
        }
    }

    if (fully_qualified) {
        bool first{true};

        for (const auto& ns : hyde::GetParentNamespaces(n, function)) {
            if (!first) signature << "::";
            first = false;
            signature << static_cast<const std::string&>(ns);
        }

        for (const auto& p : hyde::GetParentCXXRecords(n, function)) {
            if (!first) signature << "::";
            first = false;
            signature << static_cast<const std::string&>(p);
        }

        if (!first) signature << "::";
    }

    if (auto conversionDecl = llvm::dyn_cast_or_null<CXXConversionDecl>(function)) {
        signature << "operator "
                  << hyde::to_string(conversionDecl, conversionDecl->getConversionType());
    } else {
        signature << hyde::PostProcessType(function, function->getNameInfo().getAsString());
    }

    signature << "(";

    for (int i = 0, paramsCount = function->getNumParams(); i < paramsCount; ++i) {
        if (i) signature << ", ";
        auto* paramDecl = function->getParamDecl(i);
        signature << hyde::to_string(paramDecl, paramDecl->getType());
        if (named_args) {
            auto arg_name = function->getParamDecl(i)->getNameAsString();
            if (!arg_name.empty()) {
                signature << " " << std::move(arg_name);
            }
        }
    }
