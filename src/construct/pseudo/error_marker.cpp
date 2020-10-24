#include <ionlang/passes/pass.h>

namespace ionlang {
    ErrorMarker::ErrorMarker() :
        Construct(ConstructKind::ErrorMarker) {
        //
    }

    void ErrorMarker::accept(Pass& visitor) {
        visitor.visitErrorMarker(this->dynamicCast<ErrorMarker>());
    }
}
