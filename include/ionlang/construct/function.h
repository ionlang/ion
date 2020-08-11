#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/tracking/local_var_descriptor.h>
#include "construct.h"
#include "prototype.h"
#include "function_body.h"

namespace ionlang {
    class Pass;

    class Function : public Construct {
    private:
        ionshared::Ptr<Prototype> prototype;

        ionshared::Ptr<FunctionBody> body;

        ionshared::PtrSymbolTable<LocalVariableDescriptor> localVariables;

    public:
        Function(ionshared::Ptr<Prototype> prototype, ionshared::Ptr<FunctionBody> body);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        ionshared::Ptr<Prototype> getPrototype() const noexcept;

        void setPrototype(ionshared::Ptr<Prototype> prototype);

        ionshared::Ptr<FunctionBody> getBody() const noexcept;

        // TODO: Should this automatically set the body's parent as well?
        void setBody(ionshared::Ptr<FunctionBody> body) noexcept;

        ionshared::PtrSymbolTable<LocalVariableDescriptor> getLocalVariables() const;

        void setLocalVariables(ionshared::PtrSymbolTable<LocalVariableDescriptor> localVariables);
    };
}
