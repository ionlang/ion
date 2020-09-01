#include <ionlang/syntax/parser.h>
#include <ionlang/const/const.h>
#include <ionlang/lexical/classifier.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    // TODO: Consider using Ref<> to register pending type reference if user-defined type is parsed?
    ionshared::OptPtr<Type> Parser::parseType() {
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
                IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolStar))

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

        IONIR_PARSER_ASSERT((
            Classifier::isBuiltInType(tokenKind)
                || tokenKind == TokenKind::Identifier
        ))

        ionshared::OptPtr<Type> type;

        if (tokenKind == TokenKind::TypeVoid) {
            type = this->parseVoidType();
        }
        else if (Classifier::isIntegerType(tokenKind)) {
            type = this->parseIntegerType();
        }

        // TODO: Add support for missing types.

        /**
         * Type could not be identified as integer nor void
         * type, attempt to resolve its an internal type kind
         * from the token's value, otherwise default to an
         * user-defined type assumption.
         */
        if (!ionshared::util::hasValue(type)) {
            type = std::make_shared<Type>(tokenValue, util::resolveTypeKind(tokenValue));
            this->tokenStream.skip();
        }

        // Create and return the resulting type construct.
        return type;
    }

    ionshared::OptPtr<VoidType> Parser::parseVoidType() {
        /**
         * Void type does not accept references nor pointer
         * specifiers, so just simply skip over its token.
         */
        this->skipOver(TokenKind::TypeVoid);

        return std::make_shared<VoidType>();
    }

    ionshared::OptPtr<IntegerType> Parser::parseIntegerType() {
        TokenKind currentTokenKind = this->tokenStream.get().getKind();

        if (!Classifier::isIntegerType(currentTokenKind)) {
            return std::nullopt;
        }

        // TODO: Missing support for is signed or not, as well as is pointer.

        std::optional<IntegerKind> integerKind = Const::getIntegerKind(currentTokenKind);

        if (!integerKind.has_value()) {
            return std::nullopt;
        }

        // Skip over the type token.
        this->tokenStream.skip();

        return std::make_shared<IntegerType>(*integerKind);
    }
}
