#include <vector>
#include <ionlang/const/const_name.h>
#include "test_api/bootstrap.h"
#include "test_api/const.h"
#include "pch.h"

using namespace ionlang;

TEST(ParserTest, ParseInt) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::LiteralInteger, "5")
    });

    AstPtrResult<IntegerLiteral> integer = parser.parseIntegerLiteral(nullptr);

    // TODO: Verify integer type?

    EXPECT_TRUE(util::hasValue(integer));

    // TODO: Address?
    // Prevent SEGFAULT when trying to access members of std::nullopt.
    if (!util::hasValue(integer)) {
        return;
    }

    EXPECT_EQ(util::getResultValue(integer)->value, 5);
}

TEST(ParserTest, ParseChar) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::LiteralCharacter, "a")
    });

    AstPtrResult<CharLiteral> character = parser.parseCharLiteral(nullptr);

    EXPECT_TRUE(util::hasValue(character));
    EXPECT_EQ(util::getResultValue(character)->value, 'a');
}

TEST(ParserTest, ParseIdentifier) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::Identifier, test::constant::foo)
    });

    std::optional<std::string> identifier = parser.parseName();

    EXPECT_TRUE(identifier.has_value());
    EXPECT_EQ(*identifier, test::constant::foo);
}

// TODO: Successor is StructType.
TEST(ParserTest, ParseUserDefinedType) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::Identifier, test::constant::foo)
    });

    AstPtrResult<Resolvable<Type>> typeResult = parser.parseType(nullptr);

    EXPECT_TRUE(util::hasValue(typeResult));

    std::shared_ptr<Type> type = **util::getResultValue(typeResult);

    EXPECT_EQ(type->typeKind, TypeKind::Struct);
    EXPECT_EQ(type->typeName, test::constant::foo);
}

TEST(ParserTest, ParseVoidType) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::TypeVoid, const_name::typeVoid)
    });

    AstPtrResult<Resolvable<Type>> typeResult = parser.parseType(nullptr);

    EXPECT_TRUE(util::hasValue(typeResult));

    std::shared_ptr<Type> type = **util::getResultValue(typeResult);

    EXPECT_EQ(type->typeName, const_name::typeVoid);
    EXPECT_EQ(type->typeKind, TypeKind::Void);
}

TEST(ParserTest, ParseInteger32Type) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::TypeInt32, const_name::typeInt32)
    });

    AstPtrResult<Resolvable<Type>> typeResult = parser.parseType(nullptr);

    EXPECT_TRUE(util::hasValue(typeResult));

    std::shared_ptr<Type> type = **util::getResultValue(typeResult);

    EXPECT_EQ(type->typeName, const_name::typeInt32);
    EXPECT_EQ(type->typeKind, TypeKind::Integer);

    // Convert to integer type and inspect.
    // TODO: Is static cast correct here, or should it be a dynamic pointer cast?
    std::shared_ptr<IntegerType> integerType = type->staticCast<IntegerType>();

    EXPECT_EQ(integerType->integerKind, IntegerKind::Int32);
    EXPECT_TRUE(integerType->isSigned);
}

TEST(ParserTest, ParsePointerType) {
    // TODO: Implement.
}

// TODO: Update to 'ParseArgS'
//TEST(ParserTest, ParseArg) {
//    Parser parser = test::bootstrap::parser({
//        Token(TokenKind::Identifier, "type"),
//        Token(TokenKind::Identifier, "test")
//    });
//
//    std::optional<Arg> argResult = parser.parseArg(nullptr);
//
//    EXPECT_TRUE(argResult.has_value());
//
//    Arg arg = *argResult;
//
//    EXPECT_EQ(arg.first->name, "type");
//    EXPECT_EQ(arg.second, "test");
//}

TEST(ParserTest, ParseFunctionBody) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::SymbolBraceL, "{"),
        Token(TokenKind::SymbolBraceR, "}")
    });

    AstPtrResult<Block> functionBodyResult = parser.parseBlock(nullptr);

    EXPECT_TRUE(util::hasValue(functionBodyResult));
    EXPECT_TRUE(util::getResultValue(functionBodyResult)->symbolTable->isEmpty());
}

TEST(ParserTest, ParseEmptyPrototype) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::Identifier, test::constant::foobar),
        Token(TokenKind::SymbolParenthesesL, "("),
        Token(TokenKind::SymbolParenthesesR, ")"),
        Token(TokenKind::SymbolArrow, "->"),
        Token(TokenKind::Identifier, "type")
    });

    AstPtrResult<Prototype> prototypeResult = parser.parsePrototype(nullptr);

    EXPECT_TRUE(util::hasValue(prototypeResult));

    std::shared_ptr<Prototype> prototype = util::getResultValue(prototypeResult);

    // Verify return type.
    EXPECT_STREQ((***prototype->returnType->id).c_str(), "type");

    // Verify prototype.
    EXPECT_STREQ(prototype->name.c_str(), test::constant::foobar.c_str());

    // Verify prototype's arguments.
    EXPECT_TRUE(prototype->argumentList->symbolTable->isEmpty());
    EXPECT_FALSE(prototype->argumentList->isVariable);
}

TEST(ParserTest, ParseEmptyFunction) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::KeywordFunction, "fn"),
        Token(TokenKind::Identifier, test::constant::foobar),
        Token(TokenKind::SymbolParenthesesL, "("),
        Token(TokenKind::SymbolParenthesesR, ")"),
        Token(TokenKind::SymbolArrow, "->"),
        Token(TokenKind::Identifier, "type"),
        Token(TokenKind::SymbolBraceL, "{"),
        Token(TokenKind::SymbolBraceR, "}")
    });

    AstPtrResult<Function> functionResult = parser.parseFunction(nullptr);

    ASSERT_TRUE(util::hasValue(functionResult));

    std::shared_ptr<Function> function = util::getResultValue(functionResult);

    EXPECT_TRUE(function->verify());
    EXPECT_TRUE(function->body->symbolTable->isEmpty());
}

TEST(ParserTest, ParseFunction) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::KeywordFunction, "fn"),
        Token(TokenKind::Identifier, test::constant::foobar),
        Token(TokenKind::SymbolParenthesesL, "("),
        Token(TokenKind::SymbolParenthesesR, ")"),
        Token(TokenKind::SymbolArrow, "->"),
        Token(TokenKind::TypeVoid, "void"),
        Token(TokenKind::SymbolBraceL, "{"),
        Token(TokenKind::SymbolBraceR, "}")
    });

    AstPtrResult<Function> functionResult = parser.parseFunction(nullptr);

    ASSERT_TRUE(util::hasValue(functionResult));

    std::shared_ptr<Function> function = util::getResultValue(functionResult);

    /**
     * Function should be able to be verified successfully,
     * since it's body contains valid requirements.
     */
    EXPECT_TRUE(function->verify());

    // TODO: Verify prototype? Or just body block?
}

TEST(ParserTest, ParseExtern) {
    Parser parser = test::bootstrap::parser({
        Token(TokenKind::KeywordExtern, "extern"),
        Token(TokenKind::Identifier, test::constant::foobar),
        Token(TokenKind::SymbolParenthesesL, "("),
        Token(TokenKind::SymbolParenthesesR, ")"),
        Token(TokenKind::SymbolArrow, "->"),
        Token(TokenKind::Identifier, "type"),
        Token(TokenKind::SymbolSemiColon, ";")
    });

    AstPtrResult<Extern> externResult = parser.parseExtern(nullptr);

    ASSERT_TRUE(util::hasValue(externResult));

    std::shared_ptr<Prototype> prototype = util::getResultValue(externResult)->prototype;

    EXPECT_EQ(prototype->name, test::constant::foobar);
    EXPECT_TRUE(prototype->argumentList->symbolTable->isEmpty());
    EXPECT_FALSE(prototype->argumentList->isVariable);
}

TEST(ParserTest, ParseBinaryOperationExpr) {
    std::string expectedValueString = "1";
    uint32_t expectedValue = 1;

    Parser parser = test::bootstrap::parser({
        Token(TokenKind::LiteralInteger, expectedValueString),
        Token(TokenKind::OperatorAddition, "+"),
        Token(TokenKind::LiteralInteger, expectedValueString)
    });

    AstPtrResult<Expression<>> operationExprResult =
        parser.parseExpression(nullptr);

    ASSERT_TRUE(util::hasValue(operationExprResult));

    EXPECT_EQ(
        util::getResultValue(operationExprResult)->expressionKind,
        ExpressionKind::Operation
    );

    std::shared_ptr<OperationExpr> binaryOperationExpr =
        util::getResultValue(operationExprResult)->dynamicCast<OperationExpr>();

    EXPECT_EQ(binaryOperationExpr->operation, IntrinsicOperatorKind::Addition);
    EXPECT_EQ(binaryOperationExpr->leftSideValue->expressionKind, ExpressionKind::IntegerLiteral);
    ASSERT_TRUE(ionshared::util::hasValue(binaryOperationExpr->rightSideValue));

    std::shared_ptr<Expression<>> rightSide = *binaryOperationExpr->rightSideValue;

    EXPECT_EQ(rightSide->expressionKind, ExpressionKind::IntegerLiteral);

    std::shared_ptr<IntegerLiteral> leftSideIntegerLiteral =
        binaryOperationExpr->leftSideValue->dynamicCast<IntegerLiteral>();

    std::shared_ptr<IntegerLiteral> rightSideIntegerLiteral =
        rightSide->dynamicCast<IntegerLiteral>();

    // TODO: Consider checking value types as well?

    EXPECT_EQ(leftSideIntegerLiteral->value, expectedValue);
    EXPECT_EQ(rightSideIntegerLiteral->value, expectedValue);
}
