#include <ionshared/misc/util.h>
#include <ionlang/const/const.h>
#include <ionlang/const/const_name.h>
#include <ionlang/const/notice.h>
#include <ionlang/syntax/parser.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    AstPtrResult<Value<>> Parser::parseLiteralFork() {
        /**
         * Always use static pointer cast when downcasting to Value<>,
         * otherwise the cast result will be nullptr.
         */
        switch (this->tokenStream.get().getKind()) {
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
        this->beginSourceLocationMapping();

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

        // Skip the value token.
        this->tokenStream.skip();

        // Calculate the value's bit-length and it's corresponding integer kind.
        uint32_t valueBitLength = ionshared::util::calculateBitLength(value);

        std::optional<IntegerKind> valueIntegerKind =
            util::calculateIntegerKindFromBitLength(valueBitLength);

        if (!valueIntegerKind.has_value()) {
            // TODO: Use proper exception.
            throw std::runtime_error("Integer value's type kind could not be determined");
        }
        /**
         * Default literal integers to have a bit-length of 32 bits,
         * if they can fit on it.
         */
        else if (*valueIntegerKind <= IntegerKind::Int32) {
            valueIntegerKind = IntegerKind::Int32;
        }

        ionshared::Ptr<IntegerType> integerType =
            std::make_shared<IntegerType>(*valueIntegerKind);

        ionshared::Ptr<IntegerLiteral> integerLiteral =
            std::make_shared<IntegerLiteral>(integerType, value);

        this->finishSourceLocationMapping(integerLiteral);

        return integerLiteral;
    }

    AstPtrResult<BooleanLiteral> Parser::parseBooleanLiteral() {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralBoolean), BooleanLiteral)

        std::string value = this->tokenStream.get().getValue();

        this->tokenStream.skip();

        bool boolValue;

        if (value == ConstName::booleanTrue) {
            boolValue = true;
        }
        else if (value == ConstName::booleanFalse) {
            boolValue = false;
        }
        else {
            // TODO: Use internal errors.
            throw std::runtime_error("Unexpected token value");
        }

        ionshared::Ptr<BooleanLiteral> booleanLiteral =
            std::make_shared<BooleanLiteral>(boolValue);

        this->finishSourceLocationMapping(booleanLiteral);

        return booleanLiteral;
    }

    AstPtrResult<CharLiteral> Parser::parseCharLiteral() {
        this->beginSourceLocationMapping();

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
        ionshared::Ptr<CharLiteral> charLiteral =
            std::make_shared<CharLiteral>(stringValue[0]);

        this->finishSourceLocationMapping(charLiteral);

        return charLiteral;
    }

    AstPtrResult<StringLiteral> Parser::parseStringLiteral() {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralString), StringLiteral)

        // Extract the value from the string token.
        std::string value = this->tokenStream.get().getValue();

        // Skip over string token.
        this->tokenStream.skip();

        ionshared::Ptr<StringLiteral> stringLiteral =
            std::make_shared<StringLiteral>(value);

        this->finishSourceLocationMapping(stringLiteral);

        return stringLiteral;
    }
}
