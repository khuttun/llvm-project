//===--- MapSubscriptOperatorLookupCheck.cpp - clang-tidy -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MapSubscriptOperatorLookupCheck.h"
#include "../utils/OptionsUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bugprone {

MapSubscriptOperatorLookupCheck::MapSubscriptOperatorLookupCheck(
    llvm::StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      MapTypes(Options.get("MapTypes", "::std::map;::std::unordered_map")) {}

bool MapSubscriptOperatorLookupCheck::isLanguageVersionSupported(
    const LangOptions &LangOpts) const {
  return LangOpts.CPlusPlus;
}

void MapSubscriptOperatorLookupCheck::storeOptions(
    ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "MapTypes", MapTypes);
}

void MapSubscriptOperatorLookupCheck::registerMatchers(MatchFinder *Finder) {
  auto MapTypeVec = utils::options::parseStringList(MapTypes);
  Finder->addMatcher(
      cxxOperatorCallExpr(
          callee(cxxMethodDecl(hasName("operator[]"),
                               ofClass(hasAnyName(std::vector<StringRef>(
                                   MapTypeVec.begin(), MapTypeVec.end()))))
                     .bind("operator")),
          anyOf(
              // the return value is used in const context
              hasParent(implicitCastExpr(
                  anyOf(hasImplicitDestinationType(isConstQualified()),
                        // for scalar types const usage of the return value is
                        // seen as lvalue -> rvalue cast
                        hasCastKind(CK_LValueToRValue)))),
              // address of the return value is taken, but the resulting pointer
              // is used as pointer-to-const
              hasParent(unaryOperator(
                  hasOperatorName("&"),
                  hasParent(implicitCastExpr(hasImplicitDestinationType(
                      pointsTo(isConstQualified()))))))))
          .bind("call"),
      this);
}

void MapSubscriptOperatorLookupCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Call = Result.Nodes.getNodeAs<CallExpr>("call");
  const auto *Operator = Result.Nodes.getNodeAs<CXXMethodDecl>("operator");
  diag(Call->getBeginLoc(), "do not use operator[] for %0 lookup")
      << Operator->getParent()->getName();
}

} // namespace bugprone
} // namespace tidy
} // namespace clang
