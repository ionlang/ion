#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Method> Method::make(
        MethodKind kind,
        const std::shared_ptr<StructType>& structType,
        const std::shared_ptr<Prototype>& prototype,
        const std::shared_ptr<Block>& body
    ) noexcept {
        std::shared_ptr<Method> result =
            std::make_shared<Method>(kind, prototype, body);

        prototype->parent = result;

        for (const auto& basicBlock : body) {
            basicBlock->parent = result;
        }

        return result;
    }

    Method::Method(
        MethodKind kind,
        std::shared_ptr<StructType> structType,
        const std::shared_ptr<Prototype>& prototype,
        std::shared_ptr<Block> body
    ) noexcept :
        // TODO: Construct kind is Function, should be Method.
        Function(std::move(prototype), std::move(body)),
        methodKind(kind),
        structType(std::move(structType)) {
        //
    }

    void Method::accept(Pass& visitor) {
        visitor.visitMethod(this->dynamicCast<Method>());
    }
}
