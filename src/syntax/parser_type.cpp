#include <ionlang/syntax/parser.h>
#include <ionlang/const/const.h>

namespace ionlang {
    // TODO: Consider using Ref<> to register pending type reference if user-defined type is parsed?
    ionshared::OptPtr<Type> Parser::parseType() {
        // Retrieve the current token.
        Token token = this->stream.get();

        // Abstract the token's properties
        std::string tokenValue = token.getValue();
        TokenKind tokenKind = token.getKind();

        IONIR_PARSER_ASSERT((
            Classifier::isType(tokenKind)
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
        if (!ionshared::Util::hasValue(type)) {
            type = std::make_shared<Type>(tokenValue, ionshared::Util::resolveTypeKind(tokenValue));
            this->stream.skip();
        }

        // If applicable, mark the type as a pointer.
        if (this->is(TokenKind::SymbolStar)) {
            // TODO: CRITICAL: Pointer must be an expression, since what about **?
            /**
             * Only mark the type as a pointer if marked so
             * by the star symbol. Since some types (for example
             * the char type) are pointers by default, using a flag
             * in this case would prevent it from being a pointer
             * unless a star symbol is present.
             */
            type->get()->setIsPointer(true);

            // Skip from the star token.
            this->stream.skip();
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
        TokenKind currentTokenKind = this->stream.get().getKind();

        if (!Classifier::isIntegerType(currentTokenKind)) {
            return std::nullopt;
        }

        // TODO: Missing support for is signed or not, as well as is pointer.

        std::optional<IntegerKind> integerKind = Const::getIntegerKind(currentTokenKind);

        if (!integerKind.has_value()) {
            return std::nullopt;
        }

        // Skip over the type token.
        this->stream.skip();

        return std::make_shared<IntegerType>(*integerKind);
    }
}
