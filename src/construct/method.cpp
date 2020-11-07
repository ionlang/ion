#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Method> Method::make(
        MethodKind kind,
        const std::shared_ptr<StructType>& structType,
        const std::shared_ptr<Prototype>& prototype,
        const std::vector<std::shared_ptr<BasicBlock>>& basicBlocks
    ) noexcept {
        std::shared_ptr<Method> result =
            std::make_shared<Method>(kind, prototype, basicBlocks);

        prototype->parent = result;

        for (const auto& basicBlock : basicBlocks) {
            basicBlock->parent = result;
        }

        return result;
    }

    Method::Method(
        MethodKind kind,
        std::shared_ptr<StructType> structType,
        std::shared_ptr<Prototype> prototype,
        std::vector<std::shared_ptr<BasicBlock>> basicBlocks
    ) noexcept :
        // TODO: Construct kind is Function, should be Method.
        Function(std::move(prototype), std::move(basicBlocks)),
        methodKind(kind),
        structType(std::move(structType)) {
        //
    }

    void Method::accept(Pass& visitor) {
        visitor.visitMethod(this->dynamicCast<Method>());
    }
}
