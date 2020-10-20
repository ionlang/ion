#pragma once

#include <variant>
#include <ionshared/misc/helpers.h>
#include <ionshared/misc/result.h>
#include <ionlang/construct/pseudo/error_marker.h>

namespace ionlang {
    // TODO: Needs to be fixed.
//    template<typename T = Construct>
//    using AstResult = ionshared::Result<T, std::shared_ptr<ErrorMarker>>;
//
//    template<typename T = Construct>
//    using AstPtrResult = AstResult<std::shared_ptr<T>>;

    template<typename T = Construct>
    using AstResult = std::variant<T, std::shared_ptr<ErrorMarker>>;

    template<typename T = Construct>
    using AstPtrResult = AstResult<std::shared_ptr<T>>;
}
