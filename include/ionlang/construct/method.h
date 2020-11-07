#pragma once

#include <string>
#include <vector>
#include <ionshared/misc/named.h>
#include <ionlang/construct/block.h>
#include "function.h"

namespace ionlang {
    struct Pass;

    struct StructType;

    enum MethodKind {
        Normal,

        Constructor,

        Destructor,

        OperatorOverload
    };

    struct Method : Function {
        static std::shared_ptr<Method> make(
            MethodKind kind,
            const std::shared_ptr<StructType>& structType,
            const std::shared_ptr<Prototype>& prototype,
            const std::shared_ptr<Block>& body
        ) noexcept;

        const MethodKind methodKind;

        std::shared_ptr<StructType> structType;

        Method(
            MethodKind kind,
            std::shared_ptr<StructType> structType,
            const std::shared_ptr<Prototype>& prototype,
            std::shared_ptr<Block> body
        ) noexcept;

        void accept(Pass& visitor) override;
    };
}
