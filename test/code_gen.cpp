#include <vector>
#include <ionir/passes/lowering/llvm_lowering_pass.h>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include "pch.h"

using namespace ionlang;

TEST(CodeGenTest, VisitExtern) {
    std::shared_ptr<IonIrLoweringPass> irCodegenPass =
        test::bootstrap::irLoweringPass();

    std::shared_ptr<Prototype> prototype = Prototype::make(
        test::constant::foobar,
        ArgumentList::make(),
        Resolvable<Type>::make(std::make_shared<VoidType>())
    );

    // TODO: No parent module.
    std::shared_ptr<Extern> externConstruct = Extern::make(prototype);

    irCodegenPass->visitExtern(externConstruct);

    // TODO: Re-do. getModuleBuffer() was removed.
//    ionshared::OptPtr<ionir::Module> irModuleBuffer = irCodegenPass->getModuleBuffer();

//    EXPECT_TRUE(ionshared::util::hasValue(irModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    std::shared_ptr<ionir::LlvmLoweringPass> irLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
//    irLlvmLoweringPass->visitModule(*irModuleBuffer);

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(irLlvmLoweringPass, "extern_simple"));
}

TEST(CodeGenTest, VisitIfStatement) {
    std::shared_ptr<IonIrLoweringPass> irLoweringPass =
        test::bootstrap::irLoweringPass();

    // The parent will be filled in below.
    std::shared_ptr<Block> consequentBlock = Block::make();

    std::shared_ptr<IfStmt> ifStatement = IfStmt::make(
        std::make_shared<BooleanLiteral>(true),
        consequentBlock
    );

    std::shared_ptr<Function> function = test::bootstrap::emptyFunction({
        ifStatement
    });

    // Set the block and if statement's parents.
    consequentBlock->setParent(ifStatement);
    ifStatement->setParent(function->body);

    // Visit the function.
    irLoweringPass->visitFunction(function);

    // TODO: Re-do. getModuleBuffer() was removed.
//    ionshared::OptPtr<ionir::Module> irModuleBuffer = irLoweringPass->getModuleBuffer();

//    EXPECT_TRUE(ionshared::util::hasValue(irModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    std::shared_ptr<ionir::LlvmLoweringPass> irLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
//    irLlvmLoweringPass->visitModule(*irModuleBuffer);

    // TODO: Debugging. It messes up with statements on original and successor/consequent blocks. Fix that, then remove this.
//    ionshared::LlvmModule(*irLlvmLoweringPass->getModuleBuffer()).printIr();

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(irLlvmLoweringPass, "statement_if"));
}

TEST(CodeGenTest, VisitVariableDecl) {
    std::shared_ptr<IonIrLoweringPass> irLoweringPass =
        test::bootstrap::irLoweringPass();

    std::shared_ptr<VariableDeclStmt> variableDecl = VariableDeclStmt::make(
        Resolvable<Type>::make(std::make_shared<BooleanType>()),
        test::constant::foo,
        std::make_shared<BooleanLiteral>(true)->flattenExpression()
    );

    std::shared_ptr<Function> function = test::bootstrap::emptyFunction({
        variableDecl
    });

    // Set the variable declaration parent.
    variableDecl->setParent(function->body);

    // Visit the function.
    irLoweringPass->visitFunction(function);

    // TODO: Re-do. getModuleBuffer() was removed.
//    ionshared::OptPtr<ionir::Module> irModuleBuffer = irLoweringPass->getModuleBuffer();

//    EXPECT_TRUE(ionshared::util::hasValue(irModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    std::shared_ptr<ionir::LlvmLoweringPass> irLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
//    irLlvmLoweringPass->visitModule(*irModuleBuffer);

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(irLlvmLoweringPass, "variable_decl"));
}
