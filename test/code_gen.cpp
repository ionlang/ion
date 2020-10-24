#include <vector>
#include <ionir/passes/lowering/llvm_lowering_pass.h>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include "pch.h"

using namespace ionlang;

TEST(CodeGenTest, VisitExtern) {
    std::shared_ptr<IonIrLoweringPass> ionIrCodegenPass =
        test::bootstrap::ionIrLoweringPass();

    std::shared_ptr<VoidType> returnType = std::make_shared<VoidType>();
    std::shared_ptr<Args> args = std::make_shared<Args>();

    std::shared_ptr<Prototype> prototype = Prototype::make(
        test::constant::foobar,
        args,
        returnType
    );

    // TODO: No parent module.
    std::shared_ptr<Extern> externConstruct = Extern::make(prototype);

    ionIrCodegenPass->visitExtern(externConstruct);

    // TODO: Re-do. getModuleBuffer() was removed.
//    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrCodegenPass->getModuleBuffer();

//    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    std::shared_ptr<ionir::LlvmLoweringPass> ionIrLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
//    ionIrLlvmLoweringPass->visitModule(*ionIrModuleBuffer);

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmLoweringPass, "extern_simple"));
}

TEST(CodeGenTest, VisitIfStatement) {
    std::shared_ptr<IonIrLoweringPass> ionIrLoweringPass =
        test::bootstrap::ionIrLoweringPass();

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
    consequentBlock->parent = ifStatement;
    ifStatement->parent = function->body;

    // Visit the function.
    ionIrLoweringPass->visitFunction(function);

    // TODO: Re-do. getModuleBuffer() was removed.
//    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrLoweringPass->getModuleBuffer();

//    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    std::shared_ptr<ionir::LlvmLoweringPass> ionIrLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
//    ionIrLlvmLoweringPass->visitModule(*ionIrModuleBuffer);

    // TODO: Debugging. It messes up with statements on original and successor/consequent blocks. Fix that, then remove this.
//    ionshared::LlvmModule(*ionIrLlvmLoweringPass->getModuleBuffer()).printIr();

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmLoweringPass, "statement_if"));
}

TEST(CodeGenTest, VisitVariableDecl) {
    std::shared_ptr<IonIrLoweringPass> ionIrLoweringPass =
        test::bootstrap::ionIrLoweringPass();

    std::shared_ptr<VariableDeclStmt> variableDecl = VariableDeclStmt::make(
        Resolvable<Type>::make(std::make_shared<BooleanType>()),
        test::constant::foo,
        std::make_shared<BooleanLiteral>(true)->flatten()
    );

    std::shared_ptr<Function> function = test::bootstrap::emptyFunction({
        variableDecl
    });

    // Set the variable declaration parent.
    variableDecl->parent = function->body;

    // Visit the function.
    ionIrLoweringPass->visitFunction(function);

    // TODO: Re-do. getModuleBuffer() was removed.
//    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrLoweringPass->getModuleBuffer();

//    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    std::shared_ptr<ionir::LlvmLoweringPass> ionIrLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
//    ionIrLlvmLoweringPass->visitModule(*ionIrModuleBuffer);

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmLoweringPass, "variable_decl"));
}
