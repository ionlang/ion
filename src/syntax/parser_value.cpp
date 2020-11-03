#include <ionshared/misc/util.h>
#include <ionlang/const/const.h>
#include <ionlang/const/const_name.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Expression<>> Parser::parseLiteral(const std::shared_ptr<Construct>& parent) {
        // TODO: Should this go here?
        this->beginSourceLocationMapping();

        /**
         * Always use static pointer cast when downcasting to Value<>,
         * otherwise the cast result will be nullptr.
         */
        switch (this->tokenStream.get().kind) {
            case TokenKind::LiteralInteger: {
                AstPtrResult<IntegerLiteral> integerLiteralResult = this->parseIntegerLiteral(parent);

                IONLANG_PARSER_ASSERT(util::hasValue(integerLiteralResult))

                return util::getResultValue(integerLiteralResult)->flatten();
            }

            case TokenKind::LiteralCharacter: {
                AstPtrResult<CharLiteral> charLiteralResult = this->parseCharLiteral(parent);

                IONLANG_PARSER_ASSERT(util::hasValue(charLiteralResult))

                return util::getResultValue(charLiteralResult)->flatten();
            }

            case TokenKind::LiteralString: {
                AstPtrResult<StringLiteral> stringLiteralResult = this->parseStringLiteral(parent);

                IONLANG_PARSER_ASSERT(util::hasValue(stringLiteralResult))

                return util::getResultValue(stringLiteralResult)->flatten();
            }

            case TokenKind::LiteralBoolean: {
                AstPtrResult<BooleanLiteral> booleanLiteralResult = this->parseBooleanLiteral(parent);

                IONLANG_PARSER_ASSERT(util::hasValue(booleanLiteralResult))

                return util::getResultValue(booleanLiteralResult)->flatten();
            }

            // TODO: Missing literal(s)?

            default: {
                this->diagnosticBuilder
                    ->bootstrap(diagnostic::internalUnexpectedToken)
                    ->setSourceLocation(this->makeSourceLocation())
                    ->finish();

                return this->makeErrorMarker();
            }
        }
    }

    AstPtrResult<IntegerLiteral> Parser::parseIntegerLiteral(std::shared_ptr<Construct> parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralInteger))

        /**
         * Abstract the token's value to be used in the
         * string to long integer conversion.
         */
        std::string tokenValue = this->tokenStream.get().value;

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
        catch (std::exception& exception) {
            // Value conversion failed.
            this->diagnosticBuilder
                ->bootstrap(diagnostic::syntaxConversionFailed)
                ->setSourceLocation(this->makeSourceLocation())
                ->finish();

            return this->makeErrorMarker();
        }

        // Skip the value token.
        this->tokenStream.skip();

        // Calculate the value's bit-length and it's corresponding integer kind.
        uint32_t valueBitLength = ionshared::util::calculateBitLength(value);

        std::optional<IntegerKind> valueIntegerKind =
            util::calculateIntegerKindFromBitLength(valueBitLength);

        if (!valueIntegerKind.has_value()) {
            this->diagnosticBuilder
                ->bootstrap(diagnostic::syntaxIntegerValueTypeUnknown)
                ->setSourceLocation(this->makeSourceLocation())
                ->finish();

            return this->makeErrorMarker();
        }
        /**
         * Default literal integers to have a bit-length of 32 bits,
         * if they can fit on it.
         */
        else if (*valueIntegerKind <= IntegerKind::Int32) {
            valueIntegerKind = IntegerKind::Int32;
        }

        std::shared_ptr<IntegerType> integerType =
            std::make_shared<IntegerType>(*valueIntegerKind);

        std::shared_ptr<IntegerLiteral> integerLiteral =
            IntegerLiteral::make(integerType, value);

        integerType->parent = integerLiteral;
        integerLiteral->parent = parent;
        this->finishSourceLocationMapping(integerLiteral);

        return integerLiteral;
    }

    AstPtrResult<BooleanLiteral> Parser::parseBooleanLiteral(std::shared_ptr<Construct> parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralBoolean))

        std::string value = this->tokenStream.get().value;

        this->tokenStream.skip();

        bool boolValue;

        if (value == const_name::booleanTrue) {
            boolValue = true;
        }
        else if (value == const_name::booleanFalse) {
            boolValue = false;
        }
        else {
            this->diagnosticBuilder
                ->bootstrap(diagnostic::internalUnexpectedToken)
                ->setSourceLocation(this->makeSourceLocation())
                ->finish();

            return this->makeErrorMarker();
        }

        std::shared_ptr<BooleanLiteral> booleanLiteral =
            std::make_shared<BooleanLiteral>(boolValue);

        booleanLiteral->parent = parent;
        this->finishSourceLocationMapping(booleanLiteral);

        return booleanLiteral;
    }

    AstPtrResult<CharLiteral> Parser::parseCharLiteral(std::shared_ptr<Construct> parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralCharacter))

        // Extract the value from the character token.
        std::string stringValue = this->tokenStream.get().value;

        // Skip over character token.
        this->tokenStream.skip();

        // Ensure extracted value only contains a single character.
        if (stringValue.length() > 1) {
            this->diagnosticBuilder
                ->bootstrap(diagnostic::syntaxCharLengthInvalid)
                ->setSourceLocation(this->makeSourceLocation())
                ->finish();

            return this->makeErrorMarker();
        }

        // Create the character construct with the first and only character of the captured value.
        std::shared_ptr<CharLiteral> charLiteral =
            std::make_shared<CharLiteral>(stringValue[0]);

        charLiteral->parent = parent;
        this->finishSourceLocationMapping(charLiteral);

        return charLiteral;
    }

    AstPtrResult<StringLiteral> Parser::parseStringLiteral(std::shared_ptr<Construct> parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->is(TokenKind::LiteralString))

        // Extract the value from the string token.
        std::string value = this->tokenStream.get().value;

        // Skip over string token.
        this->tokenStream.skip();

        std::shared_ptr<StringLiteral> stringLiteral =
            std::make_shared<StringLiteral>(value);

        stringLiteral->parent = parent;
        this->finishSourceLocationMapping(stringLiteral);

        return stringLiteral;
    }
}
