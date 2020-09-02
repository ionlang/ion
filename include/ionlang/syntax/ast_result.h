#pragma once

#include <variant>
#include <ionshared/misc/helpers.h>
#include <ionshared/misc/result.h>
#include <ionlang/construct/pseudo/error_marker.h>

namespace ionlang {
    template<typename T = Construct>
    using AstResult = ionshared::Result<T, ionshared::Ptr<ErrorMarker>>;

    template<typename T = Construct>
    using AstPtrResult = AstResult<ionshared::Ptr<T>>;
}
