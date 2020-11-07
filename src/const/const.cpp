#include <ionshared/misc/util.h>
#include <ionlang/const/const.h>

namespace ionlang {
    ionshared::Map<IntegerKind, LlvmIntTypeResolver> Const::integerTypeResolverMap =
        ionshared::Map<IntegerKind, LlvmIntTypeResolver>();

    const ionshared::Map<std::string, IntrinsicModuleKind> Const::intrinsicModules({
        {"reflection", IntrinsicModuleKind::Reflection},
        {"array", IntrinsicModuleKind::Array}
    });

    const std::string Const::basicBlockInternalPrefix = ".";

    const std::string Const::basicBlockEntryId = "entry";

    std::map<TokenKind, int> Const::operatorPrecedence = {
        {TokenKind::OperatorAddition, 20},
        {TokenKind::OperatorSubtraction, 20},
        {TokenKind::OperatorMultiplication, 40},
        {TokenKind::OperatorDivision, 40},
        {TokenKind::OperatorModulo, 40}
    };

    std::map<ConstructKind, std::string> Const::constructNames = {
        {ConstructKind::Type, "type"},
        {ConstructKind::Prototype, "prototype"},
        {ConstructKind::Function, "function"},
        {ConstructKind::Extern, "extern"},
        {ConstructKind::Global, "global"},
        {ConstructKind::Block, "basic block"},
        {ConstructKind::VariableRef, "variable reference"},
        {ConstructKind::StructDefinition, "struct definition"},
        {ConstructKind::Module, "module"},
        {ConstructKind::Resolvable, "resolvable"},
        {ConstructKind::Expression, "expression"},
        {ConstructKind::Statement, "statement"},
        {ConstructKind::ErrorMarker, "error marker"},
        {ConstructKind::Attribute, "attribute"},
        {ConstructKind::Identifier, "identifier"},
        {ConstructKind::Import, "import"}
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

    std::optional<std::string> Const::findConstructKindName(ConstructKind constructKind) {
        if (ionshared::util::mapContains<ConstructKind, std::string>(Const::constructNames, constructKind)) {
            return Const::constructNames[constructKind];
        }

        return std::nullopt;
    }

    std::optional<IntegerKind> Const::findIntegerKind(TokenKind tokenKind) {
        if (ionshared::util::mapContains<TokenKind, IntegerKind>(Const::tokenKindToIntegerKind, tokenKind)) {
            return Const::tokenKindToIntegerKind[tokenKind];
        }

        return std::nullopt;
    }
}
