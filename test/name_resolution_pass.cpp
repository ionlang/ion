#include <ionlang/passes/semantic/name_resolution_pass.h>
#include <ionlang/type_system/type_factory.h>
#include <ionlang/misc/statement_builder.h>
#include "pch.h"

using namespace ionlang;

TEST(NameResolutionPassTest, Run) {
    std::shared_ptr<PassManager> passManager = std::make_shared<PassManager>();

    passManager->registerPass(std::make_shared<NameResolutionPass>(
        std::make_shared<ionshared::PassContext>()
    ));

    // Bootstrap the initial AST.
    Ast ast{
        test::bootstrap::emptyFunction()
    };

    // Locate the function and retrieve it's entry block.
    ionshared::OptPtr<Function> function = ast.front()->dynamicCast<Function>();
    std::shared_ptr<Block> functionBody = function->get()->body;

    // Create an statement builder instance and the branch instruction's condition.
    std::shared_ptr<StatementBuilder> statementBuilder =
        std::make_shared<StatementBuilder>(functionBody);

    std::string id = test::constant::foo;

    // Create the branch instruction pointing to the entry block on both true and false.
    statementBuilder->createVariableDecl(
        type_factory::typeInteger32(),
        id,

        std::make_shared<IntegerLiteral>(
            type_factory::typeInteger32(),
            1
        )->flattenExpression()
    );

    auto assignmentStmt = AssignmentStmt::make(
        Resolvable<VariableDeclStmt>::make(
            ResolvableKind::NearestVariableOrArgument,
            std::make_shared<Identifier>(id),
            functionBody
        ),

        IntegerLiteral::make(
            std::make_shared<IntegerType>(IntegerKind::Int32),
            1
        )->flattenExpression()
    );

    statementBuilder->appendStatement(assignmentStmt);

    // TODO: CRITICAL: Recently solved the problem which was that it was using the section's own symbol table instead of the function's to find the section (Dummy mistake). Verify that this is actually how it should be.

    passManager->run(ast);

    // TODO: Add more tests.

    EXPECT_TRUE(assignmentStmt->variableDeclStmtRef->isResolved());
//    EXPECT_EQ(assignmentStatement->getValue(), functionBody);
}

// TODO: Implement.
//TEST(NameresolutionPassTest, ResolveCallExprCallee) {
//    std::shared_ptr<PassManager> passManager = std::make_shared<PassManager>();
//
//    passManager->registerPass(std::make_shared<NameResolutionPass>(
//        std::make_shared<ionshared::PassContext>()
//    ));
//
//    // Bootstrap the initial AST.
//    Ast ast = {
//        test::bootstrap::emptyFunction()
//    };
//
//    // Locate the function and retrieve it's entry block.
//    ionshared::OptPtr<Function> function = ast->front()->dynamicCast<Function>();
//    std::shared_ptr<Block> functionBody = function->get()->getBody();
//
//    // Create an statement builder instance and the branch instruction's condition.
//    std::shared_ptr<StatementBuilder> statementBuilder = std::make_shared<StatementBuilder>(functionBody);
//
//    std::string id = test::constant::foo;
//
//    // TODO: CRITICAL: Recently solved the problem which was that it was using the section's own symbol table instead of the function's to find the section (Dummy mistake). Verify that this is actually how it should be.
//
//    passManager->run(ast);
//
//    // TODO: Add more tests.
//
//    // TODO
////    EXPECT_TRUE(assignmentStatement->getVariableDeclStatement()->isResolved());
//    //    EXPECT_EQ(assignmentStatement->getValue(), functionBody);
//}
