#pragma once

#include <ionshared/misc/named.h>
#include <ionlang/construct/pseudo/args.h>
#include <ionlang/construct/pseudo/child_construct.h>
#include "type.h"

namespace ionlang {
    class Pass;

    /**
     * Prototype's parent is either a function or extern construct.
     */
    class Prototype : public ChildConstruct<>, public ionshared::Named {
    private:
        ionshared::Ptr<Args> args;

        ionshared::Ptr<Type> returnType;

    public:
        explicit Prototype(
            std::string id,
            ionshared::Ptr<Args> args,
            ionshared::Ptr<Type> returnType,
            ionshared::Ptr<Construct> parent
        );

        void accept(Pass &visitor) override;

        ionshared::Ptr<Args> getArgs() const noexcept;

        void setArgs(ionshared::Ptr<Args> args) noexcept;

        ionshared::Ptr<Type> getReturnType() const noexcept;

        void setReturnType(ionshared::Ptr<Type> returnType) noexcept;
    };
}
