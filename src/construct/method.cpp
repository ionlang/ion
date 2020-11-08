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
        body->parent = result;

        return result;
    }

    Method::Method(
        MethodKind kind,
        std::shared_ptr<StructType> structType,
        std::shared_ptr<Prototype> prototype,
        std::shared_ptr<Block> body
    ) noexcept :
        // TODO: Construct kind is Function, should be Method.
        Construct(ConstructKind::Method),
        methodKind(kind),
        structType(std::move(structType)),
        prototype(std::move(prototype)),
        body(std::move(body)) {
        //
    }

    void Method::accept(Pass& visitor) {
        visitor.visitMethod(this->dynamicCast<Method>());
    }
}
