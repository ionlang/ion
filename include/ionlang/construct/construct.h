#pragma once

#include <ionshared/construct/bare_construct.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    enum class ConstructKind {
        Type,

        FunctionBody,

        Prototype,

        Function,

        Extern,

        Global,

        BasicBlock,

        VariableRef,

        VariableDecl,

        StructDecl,

        Module,

        Ref,

        Value
    };

    class Construct;

    typedef ionshared::Ast<Construct> Ast;

    class Construct : public ionshared::BareConstruct<Construct, ConstructKind> {
    public:
        explicit Construct(ConstructKind kind);

        virtual void accept(Pass &visitor) = 0;
    };
}
