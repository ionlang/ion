#pragma once

#include <ionshared/misc/helpers.h>
#include <ionshared/passes/bare_pass.h>
#include <ionlang/construct/construct.h>
#include <ionlang/construct/module.h>
#include <ionlang/construct/prototype.h>

namespace ionlang {
    class Pass : public ionshared::BarePass<Construct> {
    public:
        virtual void visitModule(ionshared::Ptr<Module> node);

        virtual void visitPrototype(ionshared::Ptr<Prototype> node);
    };
}
