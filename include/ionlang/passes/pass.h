#pragma once

#include <ionshared/misc/helpers.h>
#include <ionshared/passes/bare_pass.h>
#include <ionlang/construct/pseudo/ref.h>
#include <ionlang/construct/construct.h>
#include <ionlang/construct/module.h>
#include <ionlang/construct/prototype.h>
#include <ionlang/construct/extern.h>
#include <ionlang/construct/basic_block.h>
#include <ionlang/construct/statement.h>
#include <ionlang/construct/value.h>
#include <ionlang/construct/type.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass : public ionshared::BarePass<Construct> {
    public:
        virtual void visit(ionshared::Ptr<Construct> node);

        virtual void visitChildren(ionshared::Ptr<Construct> node);

        virtual void visitModule(ionshared::Ptr<Module> node);

        virtual void visitPrototype(ionshared::Ptr<Prototype> node);

        virtual void visitExtern(ionshared::Ptr<Extern> node);

        virtual void visitBasicBlock(ionshared::Ptr<BasicBlock> node);

        virtual void visitStatement(ionshared::Ptr<Statement> node);

        virtual void visitValue(ionshared::Ptr<Value<>> node);

        virtual void visitType(ionshared::Ptr<Type> node);

        virtual void visitRef(PtrRef<> node);

        virtual void visitFunction(ionshared::Ptr<Function> node);
    };
}
