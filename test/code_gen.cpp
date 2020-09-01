#include <vector>
#include <ionir/passes/codegen/llvm_codegen_pass.h>
#include <ionlang/passes/codegen/ionir_codegen_pass.h>
#include "test_api/bootstrap.h"
#include "test_api/compare.h"
#include "test_api/const.h"

using namespace ionlang;

TEST(CodeGenTest, VisitExtern) {
    ionshared::Ptr<IonIrCodegenPass> ionIrCodegenPass = test::bootstrap::ionIrCodegenPass();
    ionshared::Ptr<VoidType> returnType = std::make_shared<VoidType>();
    ionshared::Ptr<Args> args = std::make_shared<Args>();

    ionshared::Ptr<Prototype> prototype =
        std::make_shared<Prototype>(test::constant::foobar, args, returnType, nullptr);

    ionshared::Ptr<Extern> externConstruct = std::make_shared<Extern>(prototype);

    ionIrCodegenPass->visitExtern(externConstruct);

    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrCodegenPass->getModuleBuffer();

    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    ionshared::Ptr<ionir::LlvmCodegenPass> ionIrLlvmCodegenPass =
        std::make_shared<ionir::LlvmCodegenPass>();

    // Visit the resulting IonIR module buffer from our own codegen pass.
    ionIrLlvmCodegenPass->visitModule(*ionIrModuleBuffer);

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmCodegenPass, "extern_simple"));
}

TEST(CodeGenTest, VisitIfStatement) {
    ionshared::Ptr<IonIrCodegenPass> ionIrCodegenPass = test::bootstrap::ionIrCodegenPass();

    // The parent will be filled in below.
    ionshared::Ptr<Block> consequentBlock = std::make_shared<Block>(nullptr);

    ionshared::Ptr<IfStatement> ifStatement = std::make_shared<IfStatement>(IfStatementOpts{
        // The parent will be filled in below.
        nullptr,

        std::make_shared<IntegerLiteral>(std::make_shared<IntegerType>(IntegerKind::Int32), 1),
        consequentBlock
    });

    ionshared::Ptr<Function> function = test::bootstrap::emptyFunction({
        ifStatement
    });

    // Set the block and if statement's parents.
    consequentBlock->setParent(ifStatement);
    ifStatement->setParent(function->getBody());

    // Visit the function.
    ionIrCodegenPass->visitFunction(function);

    ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrCodegenPass->getModuleBuffer();

    EXPECT_TRUE(ionshared::util::hasValue(ionIrModuleBuffer));

    // Now, make the ionir::LlvmCodegenPass.
    ionshared::Ptr<ionir::LlvmCodegenPass> ionIrLlvmCodegenPass =
        std::make_shared<ionir::LlvmCodegenPass>();

    // Visit the resulting IonIR module buffer from our own codegen pass.
    ionIrLlvmCodegenPass->visitModule(*ionIrModuleBuffer);

    // TODO: Debugging.
    ionshared::LlvmModule(*ionIrLlvmCodegenPass->getModuleBuffer()).print();

    // Then proceed to compare, as LLVM entities will have been emitted inside the ionir::LlvmCodegenPass.
    EXPECT_TRUE(test::compare::ir(ionIrLlvmCodegenPass, "statement_if"));
}
