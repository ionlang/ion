#include <vector>
#include <ionir/passes/lowering/llvm_lowering_pass.h>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include "pch.h"

using namespace ionlang;

TEST(CodeGenTest, VisitExtern) {
    ionshared::Ptr<IonIrLoweringPass> ionIrCodegenPass = test::bootstrap::ionIrLoweringPass();
    ionshared::Ptr<VoidType> returnType = std::make_shared<VoidType>();
    ionshared::Ptr<Args> args = std::make_shared<Args>();

    ionshared::Ptr<Prototype> prototype =
        std::make_shared<Prototype>(test::constant::foobar, args, returnType, nullptr);

    // TODO: No parent module.
    ionshared::Ptr<Extern> externConstruct = std::make_shared<Extern>(nullptr, prototype);

    ionIrCodegenPass->visitExtern(externConstruct);

    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrCodegenPass->getModuleBuffer();

    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    ionshared::Ptr<ionir::LlvmLoweringPass> ionIrLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
    ionIrLlvmLoweringPass->visitModule(*ionIrModuleBuffer);

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmLoweringPass, "extern_simple"));
}

TEST(CodeGenTest, VisitIfStatement) {
    ionshared::Ptr<IonIrLoweringPass> ionIrLoweringPass = test::bootstrap::ionIrLoweringPass();

    // The parent will be filled in below.
    ionshared::Ptr<Block> consequentBlock = std::make_shared<Block>(nullptr);

    ionshared::Ptr<IfStatement> ifStatement = std::make_shared<IfStatement>(IfStatementOpts{
        // The parent will be filled in below.
        nullptr,

        std::make_shared<BooleanLiteral>(true),
        consequentBlock
    });

    ionshared::Ptr<Function> function = test::bootstrap::emptyFunction({
        ifStatement
    });

    // Set the block and if statement's parents.
    consequentBlock->parent = ifStatement;
    ifStatement->parent = function->body;

    // Visit the function.
    ionIrLoweringPass->visitFunction(function);

    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrLoweringPass->getModuleBuffer();

    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    ionshared::Ptr<ionir::LlvmLoweringPass> ionIrLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
    ionIrLlvmLoweringPass->visitModule(*ionIrModuleBuffer);

    // TODO: Debugging. It messes up with statements on original and successor/consequent blocks. Fix that, then remove this.
    ionshared::LlvmModule(*ionIrLlvmLoweringPass->getModuleBuffer()).printIr();

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmLoweringPass, "statement_if"));
}

TEST(CodeGenTest, VisitVariableDecl) {
    ionshared::Ptr<IonIrLoweringPass> ionIrLoweringPass = test::bootstrap::ionIrLoweringPass();

    ionshared::Ptr<VariableDeclStatement> variableDecl = std::make_shared<VariableDeclStatement>(VariableDeclStatementOpts{
        // The parent will be filled in below.
        nullptr,

        std::make_shared<BooleanType>(),
        test::constant::foo,
        std::make_shared<BooleanLiteral>(true)
    });

    ionshared::Ptr<Function> function = test::bootstrap::emptyFunction({
        variableDecl
    });

    // Set the variable declaration parent.
    variableDecl->parent = function->body;

    // Visit the function.
    ionIrLoweringPass->visitFunction(function);

    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrLoweringPass->getModuleBuffer();

    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    ionshared::Ptr<ionir::LlvmLoweringPass> ionIrLlvmLoweringPass =
        std::make_shared<ionir::LlvmLoweringPass>(
            std::make_shared<ionshared::PassContext>()
        );

    // Visit the resulting IonIR module buffer from our own codegen pass.
    ionIrLlvmLoweringPass->visitModule(*ionIrModuleBuffer);

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmLoweringPass, "variable_decl"));
}
