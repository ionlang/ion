#include <ionlang/syntax/parser.h>
#include <ionlang/const/const.h>
#include <ionlang/lexical/classifier.h>

namespace ionlang {
    // TODO: Consider using Ref<> to register pending type reference if user-defined type is parsed?
    AstPtrResult<Type> Parser::parseType() {
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

        // Abstract the token's properties
        std::string tokenValue = token.value;
        TokenKind tokenKind = token.kind;

        IONLANG_PARSER_ASSERT((
            Classifier::isBuiltInType(tokenKind)
                || tokenKind == TokenKind::Identifier
        ))

        AstPtrResult<Type> type;

        if (tokenKind == TokenKind::TypeVoid) {
            type = util::getResultValue(this->parseVoidType());
        }
        else if (tokenKind == TokenKind::TypeBool) {
            type = util::getResultValue(this->parseBooleanType(qualifiers));
        }
        else if (Classifier::isIntegerType(tokenKind)) {
            type = util::getResultValue(this->parseIntegerType(qualifiers));
        }
        else if (tokenKind == TokenKind::Identifier) {
            type = util::getResultValue(this->parseUserDefinedType(qualifiers));
        }

        // TODO: Add support for missing types.

        // Create and return the resulting type construct.
        return type;
    }

    AstPtrResult<VoidType> Parser::parseVoidType() {
        /**
         * Void type does not accept references nor pointer
         * specifiers, so just simply skip over its token.
         */
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::TypeVoid))

        return std::make_shared<VoidType>();
    }

    AstPtrResult<BooleanType> Parser::parseBooleanType(
        const std::shared_ptr<TypeQualifiers>& qualifiers
    ) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::TypeBool))

        return std::make_shared<BooleanType>(qualifiers);
    }

    AstPtrResult<IntegerType> Parser::parseIntegerType(
        const std::shared_ptr<TypeQualifiers>& qualifiers
    ) {
        TokenKind currentTokenKind = this->tokenStream.get().kind;

        if (!Classifier::isIntegerType(currentTokenKind)) {
            // TODO: Use proper exception/error.
            throw std::runtime_error("Not an integer type");
        }

        // TODO: Missing support for is signed or not?

        std::optional<IntegerKind> integerKind =
            Const::getIntegerKind(currentTokenKind);

        if (!integerKind.has_value()) {
            // TODO: Use proper exception/error.
            throw std::runtime_error("Integer kind has no value");
        }

        // Skip over the type token.
        this->tokenStream.skip();

        return std::make_shared<IntegerType>(
            *integerKind,

            // TODO: Determine if signed or not.
            true,

            qualifiers
        );
    }

    AstPtrResult<UserDefinedType> Parser::parseUserDefinedType(
        const std::shared_ptr<TypeQualifiers>& qualifiers
    ) {
        IONLANG_PARSER_ASSERT(this->expect(TokenKind::Identifier))

        std::string name = this->tokenStream.get().value;

        this->tokenStream.skip();

        return std::make_shared<UserDefinedType>(
            name,
            qualifiers
        );
    }
}
