#pragma once

#include <ionshared/misc/named.h>
#include <ionshared/tracking/scoped.h>
#include <ionshared/tracking/context.h>
#include "construct.h"

namespace ionlang {
    struct Pass;

//    typedef ionshared::Scope<Construct> Scope;

    typedef ionshared::Context<Construct> Context;

    struct Module : Construct, ionshared::Named {
        std::shared_ptr<Context> context;

        explicit Module(
            std::string id,
            std::shared_ptr<Context> context = std::make_shared<Context>()
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
