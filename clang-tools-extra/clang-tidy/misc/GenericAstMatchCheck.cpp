//===--- GenericAstMatchCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GenericAstMatchCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "../../clang-query/Query.h"
#include "../../clang-query/QueryParser.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

void GenericAstMatchCheck::processCode(StringRef Name, ClangTidyContext *Context, StringRef CodeRef, query::QuerySession& QS) {
  while (!CodeRef.empty()) {
    query::QueryRef Q = query::QueryParser::parse(CodeRef, QS);
    switch (Q->Kind) {
      case query::QK_Match: {
        const auto& MatchQuerry = llvm::cast<query::MatchQuery>(*Q);
        Matchers.push_back(MatchQuerry.Matcher);
        break;
      }
      case query::QK_Let: {
        const auto& LetQuerry = llvm::cast<query::LetQuery>(*Q);
        LetQuerry.run(llvm::errs(), QS);
        break;
      }
      case query::QK_Invalid: {
        const auto& InvalidQuerry = llvm::cast<query::InvalidQuery>(*Q);
        for (const auto& Line : llvm::split(InvalidQuerry.ErrStr, "\n")) {
          Context->configurationDiag("%0:%1") << Name << Line;
        }
        break;
      }
        //FIXME FileQuerry should also be supported, but what to do with relative paths?
      case query::QK_File:
      case query::QK_DisableOutputKind:
      case query::QK_EnableOutputKind:
      case query::QK_SetOutputKind:
      case query::QK_SetTraversalKind:
      case query::QK_Help:
      case query::QK_NoOp:
      case query::QK_Quit:
      case query::QK_SetBool:
      {
        Context->configurationDiag("%0: unsupported querry kind '%1'") << Name << CodeRef.drop_back(Q->RemainingContent.size()).trim();
      }
    }
    CodeRef = Q->RemainingContent;
  }
  }

GenericAstMatchCheck::GenericAstMatchCheck(StringRef Name, ClangTidyContext *Context, StringRef Code)
    : ClangTidyCheck(Name, Context),
      Code(Code.str()) {
  query::QuerySession QS({});
  processCode(Name, Context, Code, QS);
}

void GenericAstMatchCheck::registerMatchers(MatchFinder *Finder) {
  for (const auto& Matcher : Matchers) {
    assert(Finder->addDynamicMatcher(Matcher, this));
  }
}

void GenericAstMatchCheck::check(const MatchFinder::MatchResult &Result) {
  auto Map = Result.Nodes.getMap();
  for (const auto& [k, v] : Map) {
    diag(v.getSourceRange().getBegin(), k) << v.getSourceRange();
  }
}

} // namespace clang::tidy::misc
