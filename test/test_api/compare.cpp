#include <ionir/llvm/llvm_module.h>
#include <ionshared/misc/helpers.h>
#include "compare.h"
#include "util.h"
#include "filesystem.h"

namespace ionlang::test::compare {
    const std::string irPath = "ir";

    bool strings(std::string expected, std::string actual) {
        return util::trim(std::move(expected)) == util::trim(std::move(actual));
    }

    bool ir(std::string output, const std::string &fileName) {
        std::optional<std::string> contents = fs::readTestFile(fs::joinPaths(irPath, fileName + ".ll"));

        // TODO: Consider returning int or enum for better verbosity.
        // File contents could not be retrieved. Fail process.
        if (!contents.has_value()) {
            return false;
        }

        // Trim and compare expected output and actual file content.
        return util::trim(std::move(output)) == util::trim(*contents);
    }

    bool ir(ionshared::Ptr<IonIrCodegenPass> ionIrCodegenPass, std::string fileName) {
        ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = ionIrCodegenPass->getModuleBuffer();

        if (!ionshared::Util::hasValue(ionIrModuleBuffer)) {
            throw std::runtime_error("Module buffer in IonIrCodegenPass is not set");
        }

        return compare::ir(LlvmModule(*ionIrModuleBuffer).getAsString(), fileName);
    }
}
