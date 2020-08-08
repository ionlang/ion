#pragma once

#include <ionshared/misc/named.h>
#include <ionlang/passes/pass.h>
#include "construct.h"

namespace ionlang {
    class Module : public Construct, public ionshared::Named {
    public:
        explicit Module(std::string id);

        void accept(Pass &visitor) override;
    };
}
