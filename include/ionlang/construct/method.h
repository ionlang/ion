#pragma once

#include <string>
#include <vector>
#include <ionshared/misc/named.h>
#include <ionir/construct/pseudo/args.h>
#include <ionir/construct/pseudo/child_construct.h>
#include <ionir/construct/type/struct.h>
#include "type.h"
#include "function.h"

namespace ionlang {
    struct Pass;

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
            const std::vector<std::shared_ptr<BasicBlock>>& basicBlocks
        ) noexcept;

        const MethodKind methodKind;

        std::shared_ptr<StructType> structType;

        Method(
            MethodKind kind,
            std::shared_ptr<StructType> structType,
            std::shared_ptr<Prototype> prototype,
            std::vector<std::shared_ptr<BasicBlock>> basicBlocks
        ) noexcept;

        void accept(Pass& visitor) override;
    };
}
