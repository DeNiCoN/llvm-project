//===--- GenericAstMatchCheck.h - clang-tidy --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_GENERICASTMATCHCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_GENERICASTMATCHCHECK_H

#include "../ClangTidyCheck.h"
#include "clang/ASTMatchers/Dynamic/VariantValue.h"
#include <vector>

namespace clang::query {
  class QuerySession;
} // namespace clang::query

namespace clang::tidy::misc {

/// A check that matches on a given ast expression.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/misc/generic-ast-match.html
class GenericAstMatchCheck : public ClangTidyCheck {
public:
  GenericAstMatchCheck(StringRef Name, ClangTidyContext *Context, StringRef Code);

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }
  private:
    void processCode(StringRef Name, ClangTidyContext *Context, StringRef FileContentRef, query::QuerySession& QS);
    std::vector<ast_matchers::dynamic::DynTypedMatcher> Matchers;
    std::string Code;
};

} // namespace clang::tidy::misc

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_GENERICASTMATCHCHECK_H
