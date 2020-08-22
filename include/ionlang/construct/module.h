#pragma once

#include <ionshared/misc/named.h>
#include <ionshared/tracking/scope_anchor.h>
#include <ionlang/passes/pass.h>
#include "construct.h"

namespace ionlang {
    class Module : public Construct, public ionshared::ScopeAnchor<Construct>, public ionshared::Named {
    public:
        explicit Module(
            std::string id,

            ionshared::PtrSymbolTable<Construct> symbolTable =
                ionshared::Util::makePtrSymbolTable<Construct>()
        );

        void accept(Pass &visitor) override;
    };
}
