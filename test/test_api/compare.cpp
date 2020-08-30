#include <ionshared/misc/helpers.h>
#include <ionshared/llvm/llvm_module.h>
#include <ionir/passes/codegen/llvm_codegen_pass.h>
#include "compare.h"
#include "filesystem.h"

namespace ionlang::test::compare {
    const std::string outputIrPath = "output";

    bool strings(std::string expected, std::string actual) {
        return util::trim(std::move(expected)) == util::trim(std::move(actual));
    }

    bool ir(std::string output, const std::string &fileName) {
        std::optional<std::string> contents = fs::readTestFile(fs::joinPaths(outputIrPath, fileName + ".ll"));

        // TODO: Consider returning int or enum for better verbosity.
        // File contents could not be retrieved. Fail process.
        if (!contents.has_value()) {
            return false;
        }

        // Trim and compare expected output and actual file content.
        return util::trim(std::move(output)) == util::trim(*contents);
    }

    bool ir(const ionshared::Ptr<ionir::LlvmCodegenPass> &llvmCodegenPass, const std::string &fileName) {
        std::optional<llvm::Module *> llvmModuleBuffer = llvmCodegenPass->getModuleBuffer();

        if (!ionshared::util::hasValue(llvmModuleBuffer)) {
            throw std::runtime_error("Module buffer in LlvmCodegenPass is not set");
        }

        // TODO: IonIR cannot yet convert constructs to strings (code representation). This forces comparison against resulting LLVM IR instead of IonIR code.

        return compare::ir(ionshared::LlvmModule(*llvmModuleBuffer).getAsString(), fileName);
    }
}
