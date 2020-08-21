#pragma once

#include <ionshared/error_handling/notice.h>
#include <ionlang/construct/pseudo/error_marker.h>
#include <ionlang/syntax/ast_result.h>

namespace ionlang {
    class NoticeSentinel {
    private:
        ionshared::Ptr<ionshared::NoticeStack> noticeStack;

    public:
        explicit NoticeSentinel(
            ionshared::Ptr<ionshared::NoticeStack> noticeStack =
                std::make_shared<ionshared::Stack<ionshared::Notice>>()
        );

        [[nodiscard]] ionshared::Ptr<ionshared::NoticeStack> getNoticeStack() const noexcept;

        template<typename T = Construct>
        AstPtrResult<T> makeError(std::string message) {
            // TODO: Review.
            return std::make_shared<ErrorMarker>(message);
        }
    };
}
