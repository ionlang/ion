#pragma once

#include <ionshared/misc/helpers.h>
#include <ionshared/misc/named.h>
#include <ionlang/construct/pseudo/construct_with_parent.h>
#include "module.h"

namespace ionlang {
    struct Pass;

    struct Attribute : ConstructWithParent<Module, Construct, ConstructKind>, ionshared::Named {
        explicit Attribute(std::string id);

        void accept(Pass& visitor) override;
    };

    typedef std::vector<std::shared_ptr<Attribute>> Attributes;
}
