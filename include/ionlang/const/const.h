#pragma once

#include <string>
#include <map>
#include <optional>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <ionshared/container/map.h>
#include <ionlang/construct/value/integer_literal.h>
#include <ionlang/lexical/token_kind.h>

namespace ionlang {
    typedef llvm::IntegerType* (llvm::IntegerType::*LlvmIntTypeResolver)(llvm::LLVMContext& context);

    enum IntrinsicModuleKind {
        Reflection,

        Array
    };

    class Const {
    private:
        static ionshared::Map<IntegerKind, LlvmIntTypeResolver> integerTypeResolverMap;

    public:
        static const ionshared::Map<std::string, IntrinsicModuleKind> intrinsicModules;

        static const std::string basicBlockInternalPrefix;

        static const std::string basicBlockEntryId;

        static std::optional<LlvmIntTypeResolver> tryGetIntTypeResolver(IntegerKind kind);

        static std::map<TokenKind, int> operatorPrecedence;

        static std::map<ConstructKind, std::string> constructNames;

        static std::map<TokenKind, IntegerKind> tokenKindToIntegerKind;

        static std::optional<std::string> findConstructKindName(ConstructKind constructKind);

        static std::optional<IntegerKind> findIntegerKind(TokenKind tokenKind);
    };
}
