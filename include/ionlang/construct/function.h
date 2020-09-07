#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/tracking/local_var_descriptor.h>
#include "construct.h"
#include "prototype.h"
#include "module.h"

namespace ionlang {
    class Pass;

    class Function : public ChildConstruct<Module> {
    private:
        ionshared::Ptr<Prototype> prototype;

        ionshared::Ptr<Block> body;

        ionshared::PtrSymbolTable<LocalVariableDescriptor> localVariables;

    public:
        Function(
            ionshared::Ptr<Module> parent,
            ionshared::Ptr<Prototype> prototype,
            ionshared::Ptr<Block> body
        );

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        [[nodiscard]] ionshared::Ptr<Prototype> getPrototype() const noexcept;

        void setPrototype(ionshared::Ptr<Prototype> prototype) noexcept;

        [[nodiscard]] ionshared::Ptr<Block> getBody() const noexcept;

        // TODO: Should this automatically set the body's parent as well?
        void setBody(ionshared::Ptr<Block> body) noexcept;

        [[nodiscard]] ionshared::PtrSymbolTable<LocalVariableDescriptor> getLocalVariables() const;

        void setLocalVariables(ionshared::PtrSymbolTable<LocalVariableDescriptor> localVariables);
    };
}
