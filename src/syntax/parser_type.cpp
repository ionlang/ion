#include <ionlang/syntax/parser.h>
#include <ionlang/const/const.h>
#include <ionlang/lexical/classifier.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    // TODO: Consider using Ref<> to register pending type reference if user-defined type is parsed?
    AstPtrResult<Type> Parser::parseType() {
        ionshared::Ptr<TypeQualifiers> qualifiers = std::make_shared<TypeQualifiers>();

        // TODO: Simplify to support const mut &*type.

        // 1st qualifier: const (constant).
        if (this->is(TokenKind::QualifierConst)) {
            this->tokenStream.skip();
            qualifiers->insert(TypeQualifier::Constant);
        }

        // 2nd qualifier: mut (mutable reference or pointer).
        if (this->is(TokenKind::QualifierMutable)) {
            this->tokenStream.skip();

            // Mutable reference.
            if (this->is(TokenKind::SymbolAmpersand)) {
                this->tokenStream.skip();
                qualifiers->insert(TypeQualifier::Reference);
            }
            // Otherwise, it must be a pointer.
            else {
                IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolStar), Type)

                qualifiers->insert(TypeQualifier::Pointer);
            }
        }
        // 3rd qualifier: reference
        else if (this->is(TokenKind::SymbolAmpersand)) {
            this->tokenStream.skip();
            qualifiers->insert(TypeQualifier::Reference);
        }

        // 4th qualifier: pointer.
        if (this->is(TokenKind::SymbolStar)) {
            this->tokenStream.skip();
            qualifiers->insert(TypeQualifier::Pointer);
        }

        // TODO: What about **?

        // Retrieve the current token.
        Token token = this->tokenStream.get();

        // Abstract the token's properties
        std::string tokenValue = token.getValue();
        TokenKind tokenKind = token.getKind();

        IONLANG_PARSER_ASSERT((
            Classifier::isBuiltInType(tokenKind)
                || tokenKind == TokenKind::Identifier
        ), Type)

        AstPtrResult<Type> type;

        if (tokenKind == TokenKind::TypeVoid) {
            type = util::getResultValue(this->parseVoidType());
        }
        else if (tokenKind == TokenKind::TypeBool) {
            type = util::getResultValue(this->parseBooleanType(qualifiers));
        }
        if (Classifier::isIntegerType(tokenKind)) {
            type = util::getResultValue(this->parseIntegerType(qualifiers));
        }

        // TODO: Add support for missing types.

        /**
         * Type could not be identified as integer nor void
         * type, attempt to resolve its an internal type kind
         * from the token's value, otherwise default to an
         * user-defined type assumption.
         */
        if (!util::hasValue(type)) {
            type = std::make_shared<Type>(tokenValue, util::resolveTypeKind(tokenValue));
            this->tokenStream.skip();
        }

        // Create and return the resulting type construct.
        return type;
    }

    AstPtrResult<VoidType> Parser::parseVoidType() {
        /**
         * Void type does not accept references nor pointer
         * specifiers, so just simply skip over its token.
         */
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::TypeVoid), VoidType)

        return std::make_shared<VoidType>();
    }

    AstPtrResult<BooleanType> Parser::parseBooleanType(const ionshared::Ptr<TypeQualifiers> &qualifiers) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::TypeBool), BooleanType)

        return std::make_shared<BooleanType>(qualifiers);
    }

    AstPtrResult<IntegerType> Parser::parseIntegerType(const ionshared::Ptr<TypeQualifiers> &qualifiers) {
        TokenKind currentTokenKind = this->tokenStream.get().getKind();

        if (!Classifier::isIntegerType(currentTokenKind)) {
            // TODO: Use proper exception/error.
            throw std::runtime_error("Not an integer type");
        }

        // TODO: Missing support for is signed or not?

        std::optional<IntegerKind> integerKind = Const::getIntegerKind(currentTokenKind);

        if (!integerKind.has_value()) {
            // TODO: Use proper exception/error.
            throw std::runtime_error("Integer kind has no value");
        }

        // Skip over the type token.
        this->tokenStream.skip();

        return std::make_shared<IntegerType>(*integerKind, false, qualifiers);
    }
}
