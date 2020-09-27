#pragma once

#include <ionshared/misc/helpers.h>
#include <ionshared/misc/named.h>
#include <ionlang/construct/pseudo/child_construct.h>

namespace ionlang {
    class Pass;

    struct Attribute : ConstructWithParent<>, ionshared::Named {
        Attribute(ionshared::Ptr<Construct> parent, std::string id);

        void accept(Pass &visitor) override;
    };

    typedef std::vector<ionshared::Ptr<Attribute>> Attributes;
}
