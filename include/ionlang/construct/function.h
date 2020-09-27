#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/tracking/local_var_descriptor.h>
#include "construct.h"
#include "prototype.h"
#include "module.h"

namespace ionlang {
    class Pass;

    struct Function : ConstructWithParent<Module> {
        ionshared::Ptr<Prototype> prototype;

        ionshared::Ptr<Block> body;

        ionshared::PtrSymbolTable<LocalVariableDescriptor> localVariables;

        Function(
            ionshared::Ptr<Module> parent,
            ionshared::Ptr<Prototype> prototype,
            ionshared::Ptr<Block> body
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
