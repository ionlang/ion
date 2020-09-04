#include <ionshared/misc/util.h>
#include <ionlang/const/const.h>
#include <ionlang/const/const_name.h>
#include <ionlang/const/notice.h>
#include <ionlang/syntax/parser.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    AstPtrResult<Value<>> Parser::parseLiteral() {
        Token token = this->tokenStream.get();

        /**
         * Always use static pointer cast when downcasting to Value<>,
         * otherwise the cast result will be nullptr.
         */
        switch (token.getKind()) {
            case TokenKind::LiteralInteger: {
                AstPtrResult<IntegerLiteral> integerLiteralResult = this->parseIntegerLiteral();

                IONLANG_PARSER_ASSERT(util::hasValue(integerLiteralResult), Value<>)

                return util::getResultValue(integerLiteralResult)->staticCast<Value<>>();
            }

            case TokenKind::LiteralCharacter: {
                AstPtrResult<CharLiteral> charLiteralResult = this->parseCharLiteral();

                IONLANG_PARSER_ASSERT(util::hasValue(charLiteralResult), Value<>)

                return util::getResultValue(charLiteralResult)->staticCast<Value<>>();
            }

            case TokenKind::LiteralBoolean: {
                AstPtrResult<BooleanLiteral> booleanLiteralResult = this->parseBooleanLiteral();

                IONLANG_PARSER_ASSERT(util::hasValue(booleanLiteralResult), Value<>)

                return util::getResultValue(booleanLiteralResult)->staticCast<Value<>>();
            }

            // TODO: Missing literals.

            default: {
                // TODO: Return std::nullopt instead.
                throw ionshared::util::quickError(IONLANG_NOTICE_MISC_UNEXPECTED_TOKEN);
            }
        }
    }

    AstPtrResult<IntegerLiteral> Parser::parseIntegerLiteral() {
        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralInteger), IntegerLiteral)

        /**
         * Abstract the token's value to be used in the
         * string to long integer conversion.
         */
        std::string tokenValue = this->tokenStream.get().getValue();

        // TODO: May stol() throw an error? If so, wrap in try-catch block for safety.
        /**
         * Attempt to convert token's value to a long
         * (int64_t for cross-platform support).
         */
        int64_t value;

        try {
            // TODO: Need to add support for 128-bit length.
            /**
             * May throw an exception if invalid arguments are provided,
             * or of the integer is too large to be held in any integer
             * type native to C++ (maximum is 64-bit length).
             */
            value = std::stoll(tokenValue);
        }
        catch (std::exception &exception) {
            // Value conversion failed.
            // TODO: Use proper exception.
            throw std::runtime_error("Could not convert string to value, integer may be invalid or too large");
        }

        // Calculate the value's bit-length and it's corresponding integer kind.
        uint32_t valueBitLength = ionshared::util::calculateBitLength(value);

        std::optional<IntegerKind> valueIntegerKind =
            util::calculateIntegerKindFromBitLength(valueBitLength);

        if (!valueIntegerKind.has_value()) {
            // TODO: Use proper exception.
            throw std::runtime_error("Integer value's type kind could not be determined");
        }

        // Create a long integer type for the value.
        ionshared::Ptr<IntegerType> type =
            std::make_shared<IntegerType>(*valueIntegerKind);

        // Create the integer instance.
        ionshared::Ptr<IntegerLiteral> integer =
            std::make_shared<IntegerLiteral>(type, value);

        // Skip current token.
        this->tokenStream.tryNext();

        // Finally, return the result.
        return integer;
    }

    AstPtrResult<BooleanLiteral> Parser::parseBooleanLiteral() {
        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralBoolean), BooleanLiteral)

        std::string value = this->tokenStream.get().getValue();

        this->tokenStream.skip();

        if (value == ConstName::booleanTrue) {
            return std::make_shared<BooleanLiteral>(true);
        }
        else if (value == ConstName::booleanFalse) {
            return std::make_shared<BooleanLiteral>(false);
        }

        // TODO: Use internal errors.
        throw std::runtime_error("Unexpected token value");
    }

    AstPtrResult<CharLiteral> Parser::parseCharLiteral() {
        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralCharacter), CharLiteral)

        // Extract the value from the character token.
        std::string stringValue = this->tokenStream.get().getValue();

        // Skip over character token.
        this->tokenStream.skip();

        // Ensure extracted value only contains a single character.
        if (stringValue.length() > 1) {
            // TODO: Use proper exception.
            throw std::runtime_error("Character value length must be at most 1 character");
        }

        // Create the character construct with the first and only character of the captured value.
        return std::make_shared<CharLiteral>(stringValue[0]);
    }

    AstPtrResult<StringLiteral> Parser::parseStringLiteral() {
        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralString), StringLiteral)

        // Extract the value from the string token.
        std::string value = this->tokenStream.get().getValue();

        // Skip over string token.
        this->tokenStream.skip();

        return std::make_shared<StringLiteral>(value);
    }
}
