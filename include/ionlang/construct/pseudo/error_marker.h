#pragma once

#include <ionlang/construct/construct.h>

namespace ionlang {
    // TODO: What if 'pass.h' is never included?
    class Pass;

    class ErrorMarker : public Construct {
    public:
        ErrorMarker();

        void accept(Pass &visitor) override;
    };
}
