#pragma once

#include <ionlang/construct/pseudo/child_construct.h>
#include "prototype.h"
#include "module.h"

namespace ionlang {
    class Pass;

    class Extern : public ChildConstruct<Module> {
    private:
        ionshared::Ptr<Prototype> prototype;

    public:
        Extern(ionshared::Ptr<Module> parent, ionshared::Ptr<Prototype> prototype);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        [[nodiscard]] ionshared::Ptr<Prototype> getPrototype() const noexcept;

        void setPrototype(ionshared::Ptr<Prototype> prototype) noexcept;
    };
}
