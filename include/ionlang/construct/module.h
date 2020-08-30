#pragma once

#include <ionshared/misc/named.h>
#include <ionshared/tracking/scope_anchor.h>
#include "construct.h"

namespace ionlang {
    class Pass;

    class Module : public Construct, public ionshared::ScopeAnchor<Construct>, public ionshared::Named {
    public:
        explicit Module(
            std::string id,

            ionshared::PtrSymbolTable<Construct> symbolTable =
                ionshared::util::makePtrSymbolTable<Construct>()
        );

        void accept(Pass &visitor) override;
    };
}
