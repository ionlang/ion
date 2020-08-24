#include <ionshared/misc/util.h>
#include <ionlang/const/const.h>

namespace ionlang {
    ionshared::Map<IntegerKind, LlvmIntTypeResolver> Const::integerTypeResolverMap =
        ionshared::Map<IntegerKind, LlvmIntTypeResolver>();

    const std::string Const::basicBlockInternalPrefix = ".";

    const std::string Const::basicBlockEntryId = "entry";

    std::map<TokenKind, int> Const::operatorPrecedence = {
        {TokenKind::OperatorAdd, 20},
        {TokenKind::OperatorSubtract, 20},
        {TokenKind::OperatorMultiply, 40},
        {TokenKind::OperatorDivide, 40},
        {TokenKind::OperatorModulo, 40},
        {TokenKind::OperatorExponent, 80}
    };

    std::map<ConstructKind, std::string> Const::constructNames = {
        {ConstructKind::Type, "Type"},
        {ConstructKind::FunctionBody, "FunctionBody"},
        {ConstructKind::Prototype, "Prototype"},
        {ConstructKind::Function, "Function"},
        {ConstructKind::Extern, "Extern"},
        {ConstructKind::Global, "Global"},
        {ConstructKind::Block, "BasicBlock"},
        {ConstructKind::VariableRef, "VariableReference"},
        {ConstructKind::VariableDecl, "VariableDeclaration"},
        {ConstructKind::StructDecl, "StructDeclaration"},
        {ConstructKind::Module, "Module"},
        {ConstructKind::Ref, "Reference"},
        {ConstructKind::Value, "Value"}
    };

    std::map<TokenKind, IntegerKind> Const::tokenKindToIntegerKind = {
        {TokenKind::TypeInt8, IntegerKind::Int8},
        {TokenKind::TypeInt16, IntegerKind::Int16},
        {TokenKind::TypeInt32, IntegerKind::Int32},
        {TokenKind::TypeInt64, IntegerKind::Int64}

        // TODO: Missing unsigned ones? Or should it be operator-based?
    };

    // TODO: Refactor.
    // Const::Const()
    // {
    //     // Register integer kind -> integer type resolvers.
    //     auto map = this->integerTypeResolverMap.unwrap();

    //     // TODO: Fix insertion error.
    //     // map.insert(IntegerKind::Int1, &llvm::IntegerType::getInt1Ty);
    //     // map.insert(IntegerKind::Int32, &llvm::IntegerType::getInt32Ty);
    //     // map.insert(IntegerKind::Int64, &llvm::IntegerType::getInt64Ty);
    //     // map.insert(IntegerKind::Int128, &llvm::IntegerType::getInt128Ty);
    // }

    std::optional<LlvmIntTypeResolver> Const::tryGetIntTypeResolver(IntegerKind kind) {
        return *Const::integerTypeResolverMap.lookup(kind);
    }

    std::optional<std::string> Const::getConstructKindName(ConstructKind constructKind) {
        if (ionshared::Util::mapContains<ConstructKind, std::string>(Const::constructNames, constructKind)) {
            return Const::constructNames[constructKind];
        }

        return std::nullopt;
    }

    std::optional<IntegerKind> Const::getIntegerKind(TokenKind tokenKind) {
        if (ionshared::Util::mapContains<TokenKind, IntegerKind>(Const::tokenKindToIntegerKind, tokenKind)) {
            return Const::tokenKindToIntegerKind[tokenKind];
        }

        return std::nullopt;
    }
}
