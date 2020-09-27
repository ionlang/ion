#pragma once

#include <ionlang/construct/construct.h>

namespace ionlang {
    // TODO: What if 'pass.h' is never included?
    class Pass;

    struct ErrorMarker : Construct {
        ErrorMarker();

        void accept(Pass &visitor) override;
    };
}
