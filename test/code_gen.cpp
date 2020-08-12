#include <vector>
#include <ionlang/passes/codegen/ionir_codegen_pass.h>
#include "test_api/bootstrap.h"
#include "test_api/compare.h"
#include "test_api/const.h"

using namespace ionlang;

TEST(CodeGenTest, VisitExtern) {
    ionshared::Ptr<IonIrCodegenPass> ionIrCodegenPass = test::bootstrap::ionIrCodegenPass();
    ionshared::Ptr<VoidType> returnType = std::make_shared<VoidType>();
    ionshared::Ptr<Args> args = std::make_shared<Args>();
    ionshared::Ptr<Prototype> prototype = std::make_shared<Prototype>(test::constant::foobar, args, returnType, nullptr);
    ionshared::Ptr<Extern> externConstruct = std::make_shared<Extern>(prototype);

    ionIrCodegenPass->visitExtern(externConstruct);

    EXPECT_TRUE(test::compare::ir(ionIrCodegenPass, "extern_simple"));
}
