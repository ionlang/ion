#include <ionshared/misc/util.h>
#include <ionlang/const/const.h>
#include <ionlang/const/const_name.h>
#include <ionlang/syntax/parser.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    ionshared::OptPtr<IntegerValue> Parser::parseInt() {
        IONIR_PARSER_EXPECT(TokenKind::LiteralInteger)

        /**
         * Abstract the token's value to be used in the
         * string to long integer conversion.
         */
        std::string tokenValue = this->stream.get().getValue();

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
            return this->makeNotice("Could not convert string to value, integer may be invalid or too large");
        }

        // Calculate the value's bit-length and it's corresponding integer kind.
        uint32_t valueBitLength = ionshared::Util::calculateBitLength(value);
        std::optional<IntegerKind> valueIntegerKind = Util::calculateIntegerKindFromBitLength(valueBitLength);

        if (!valueIntegerKind.has_value()) {
            return this->makeNotice("Integer value's type kind could not be determined");
        }

        // Create a long integer type for the value.
        ionshared::Ptr<IntegerType> type = std::make_shared<IntegerType>(*valueIntegerKind);

        // Create the integer instance.
        ionshared::Ptr<IntegerValue> integer = std::make_shared<IntegerValue>(type, value);

        // Skip current token.
        this->stream.tryNext();

        // Finally, return the result.
        return integer;
    }

    ionshared::OptPtr<CharValue> Parser::parseChar() {
        IONIR_PARSER_EXPECT(TokenKind::LiteralCharacter);

        // Extract the value from the character token.
        std::string stringValue = this->stream.get().getValue();

        // Skip over character token.
        this->stream.skip();

        // Ensure extracted value only contains a single character.
        if (stringValue.length() > 1) {
            return this->makeNotice("Character value length must be at most 1 character");
        }

        // Create the character construct with the first and only character of the captured value.
        return std::make_shared<CharValue>(stringValue[0]);
    }

    ionshared::OptPtr<StringValue> Parser::parseString() {
        IONIR_PARSER_EXPECT(TokenKind::LiteralString);

        // Extract the value from the string token.
        std::string value = this->stream.get().getValue();

        // Skip over string token.
        this->stream.skip();

        return std::make_shared<StringValue>(value);
    }
}
