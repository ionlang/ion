#pragma once

#include <ionshared/misc/named.h>
#include <ionshared/tracking/scope_anchor.h>
#include <ionlang/tracking/context.h>
#include "construct.h"

namespace ionlang {
    class Pass;

    class Module : public Construct, public ionshared::Named {
    private:
        ionshared::Ptr<Context> context;

    public:
        explicit Module(
            std::string id,
            ionshared::Ptr<Context> context = std::make_shared<Context>()
        );

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        [[nodiscard]] ionshared::Ptr<Context> getContext() const noexcept;

        void setContext(ionshared::Ptr<Context> context) noexcept;
    };
}
