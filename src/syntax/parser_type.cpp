#include <ionlang/syntax/parser.h>
#include <ionlang/const/const.h>
#include <ionlang/lexical/classifier.h>

namespace ionlang {
    // TODO: Consider using Ref<> to register pending type reference if user-defined type is parsed?
    AstPtrResult<Resolvable<Type>> Parser::parseType(const std::shared_ptr<Construct>& parent) {
        std::shared_ptr<TypeQualifiers> qualifiers =
            std::make_shared<TypeQualifiers>();

        // TODO: Simplify to support const mut &*type.

        // 1st qualifier: const (constant).
        if (this->is(TokenKind::QualifierConst)) {
            this->tokenStream.skip();
            qualifiers->add(TypeQualifier::Constant);
        }

        // 2nd qualifier: mut (mutable reference or pointer).
        if (this->is(TokenKind::QualifierMutable)) {
            this->tokenStream.skip();

            // Mutable reference.
            if (this->is(TokenKind::SymbolAmpersand)) {
                this->tokenStream.skip();
                qualifiers->add(TypeQualifier::Reference);
            }
            // Otherwise, it must be a pointer.
            else {
                IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolHash))

                qualifiers->add(TypeQualifier::Pointer);
            }
        }
        // 3rd qualifier: reference
        else if (this->is(TokenKind::SymbolAmpersand)) {
            this->tokenStream.skip();
            qualifiers->add(TypeQualifier::Reference);
        }

        // 4th qualifier: pointer.
        if (this->is(TokenKind::SymbolHash)) {
            this->tokenStream.skip();
            qualifiers->add(TypeQualifier::Pointer);
        }

        // TODO: What about **?

        // Retrieve the current token.
        Token token = this->tokenStream.get();

        IONLANG_PARSER_ASSERT((
            Classifier::isBuiltInType(token.kind)
                || token.kind == TokenKind::Identifier
        ))

        AstPtrResult<Resolvable<Type>> type;

        if (token.kind == TokenKind::TypeVoid) {
            type = Resolvable<Type>::make(
                util::getResultValue(this->parseVoidType(parent))
            );
        }
        else if (token.kind == TokenKind::TypeBool) {
            type = Resolvable<Type>::make(
                util::getResultValue(this->parseBooleanType(parent, qualifiers))
            );
        }
        else if (Classifier::isIntegerType(token.kind)) {
            type = Resolvable<Type>::make(
                util::getResultValue(this->parseIntegerType(parent, qualifiers))
            );
        }
        else if (token.kind == TokenKind::Identifier) {
            type = util::getResultValue(
                this->parseStructType(parent, qualifiers)
            )->staticCast<Resolvable<Type>>();
        }

        // TODO: Add support for missing types.

        // TODO: Review. What if type wasn't assigned to? Default to AstPtrResult<Type>?

        // Create and return the resulting type construct.
        return type;
    }

    AstPtrResult<VoidType> Parser::parseVoidType(const std::shared_ptr<Construct>& parent) {
        /**
         * Void type does not accept references nor pointer
         * specifiers, so just simply skip over its token.
         */
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::TypeVoid))

        std::shared_ptr<VoidType> voidType = std::make_shared<VoidType>();

        voidType->parent = parent;

        return voidType;
    }

    AstPtrResult<BooleanType> Parser::parseBooleanType(
        const std::shared_ptr<Construct>& parent,
        const std::shared_ptr<TypeQualifiers>& qualifiers
    ) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::TypeBool))

        std::shared_ptr<BooleanType> booleanType =
            std::make_shared<BooleanType>(qualifiers);

        booleanType->parent = parent;

        return booleanType;
    }

    AstPtrResult<IntegerType> Parser::parseIntegerType(
        const std::shared_ptr<Construct>& parent,
        const std::shared_ptr<TypeQualifiers>& qualifiers
    ) {
        TokenKind currentTokenKind = this->tokenStream.get().kind;

        if (!Classifier::isIntegerType(currentTokenKind)) {
            // TODO: Use proper exception/error.
            throw std::runtime_error("Not an integer type");
        }

        // TODO: Missing support for is signed or not?

        std::optional<IntegerKind> integerKind =
            Const::findIntegerKind(currentTokenKind);

        if (!integerKind.has_value()) {
            // TODO: Use proper exception/error.
            throw std::runtime_error("Integer kind has no value");
        }

        // Skip over the type token.
        this->tokenStream.skip();

        std::shared_ptr<IntegerType> integerType = std::make_shared<IntegerType>(
            *integerKind,

            // TODO: Determine if signed or not.
            true,

            qualifiers
        );

        integerType->parent = parent;

        return integerType;
    }

    AstPtrResult<Resolvable<StructType>> Parser::parseStructType(
        const std::shared_ptr<Construct>& parent,
        const std::shared_ptr<TypeQualifiers>& qualifiers
    ) {
        IONLANG_PARSER_ASSERT(this->expect(TokenKind::Identifier))

        std::string name = this->tokenStream.get().value;

        this->tokenStream.skip();

        PtrResolvable<StructType> structType =
            Resolvable<StructType>::make(
                ResolvableKind::StructType,
                std::make_shared<Identifier>(name),
                parent
            );

        structType->parent = parent;

        return structType;
    }
}
