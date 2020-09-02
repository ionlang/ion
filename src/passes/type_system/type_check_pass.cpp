#include <ionlang/const/notice.h>
#include <ionlang/type_system/type_util.h>
#include <ionlang/passes/type_system/type_check_pass.h>

namespace ionlang {
    void TypeCheckPass::visitFunction(ionshared::Ptr<Function> node) {
        ionshared::OptPtr<Block> functionBody = node->getBody();

        if (!ionshared::util::hasValue(functionBody)) {
            throw std::runtime_error("Entry basic block for function body is not set");
        }

        std::vector<ionshared::Ptr<Statement>> statements = functionBody->get()->getStatements();

        // TODO: CRITICAL! There may be more than a single terminal statement on blocks.
        ionshared::OptPtr<Statement> terminalStatement = functionBody->get()->findTerminalStatement();

        // All basic blocks must contain at least a terminal instruction.
        if (statements.empty() || !ionshared::util::hasValue(terminalStatement)) {
            throw std::runtime_error("Section must contain at least a terminal instruction");
        }
    }

    void TypeCheckPass::visitReturnStatement(ionshared::Ptr<ReturnStatement> node) {
        // TODO: Check function (parent) is actually ConstructKind::Function since blocks can have different parents.
        ionshared::Ptr<Function> function = node->getParent()->getParent()->getParent();
        ionshared::Ptr<Type> functionReturnType = function->getPrototype()->getReturnType();
        ionshared::OptPtr<Construct> returnStatementValue = node->getValue();
        bool returnStatementValueSet = ionshared::util::hasValue(returnStatementValue);

        /**
         * Functions whose prototype's return type is non-void must provide
         * a value to the return instruction.
         */
        if (functionReturnType->getTypeKind() != TypeKind::Void && !returnStatementValueSet) {
            throw std::runtime_error(
                "Function whose prototype's return type is not void must return a corresponding value"
            );
        }
        else if (returnStatementValueSet) {
            switch (returnStatementValue->get()->getConstructKind()) {
                /**
                 * Return instruction's value is a construct derived from Value<>.
                 * Compare its type to the function's return type.
                 */
                case ConstructKind::Value: {
                    /**
                     * At this point, the function returns a non-void value. Abstract its
                     * return value's type construct.
                     */
                    ionshared::Ptr<Type> returnInstValueType =
                        returnStatementValue->get()->staticCast<Value<>>()->getType();

                    /**
                     * Function's return type and the return instruction's value's type
                     * do not match. Issue a type error. Even if it's returning compatible
                     * types (such as different bit length integer values) no implicit conversions
                     * are made by the compiler (everything on the IR language must be explicit).
                     */
                    if (!type_util::isSameType(returnInstValueType, functionReturnType)) {
                        throw ionshared::util::quickError(
                            IONIR_NOTICE_FUNCTION_RETURN_TYPE_MISMATCH,
                            function->getPrototype()->getId()
                        );
                    }

                    break;
                }

                /**
                 * Return instruction's value is set, but it's a reference. The reference
                 * must be resolved then compared.
                 */
                case ConstructKind::Ref: {
                    // TODO
                    throw std::runtime_error("Not implemented");

                    break;
                }

                default: {
                    throw std::runtime_error("Unrecognized return instruction value");
                }
            }
        }
    }

    void TypeCheckPass::visitVariableDecl(ionshared::Ptr<VariableDecl> node) {
        std::string targetId = node->getTarget()->getId();

        if (!node->getTarget()->isResolved()) {
            throw ionshared::util::quickError(
                IONIR_NOTICE_INST_STORE_UNRESOLVED_REF,
                targetId
            );
        }

        TypeKind storeInstValueTypeKind = node->getValue()->getType()->getTypeKind();
        ionshared::Ptr<AllocaInst> targetValue = *node->getTarget()->getValue();

        // Attempting to store a value with a different type than what was allocated.
        if (storeInstValueTypeKind != targetValue->getType()->getTypeKind()) {
            throw ionshared::util::quickError(
                IONIR_NOTICE_INST_STORE_TYPE_MISMATCH,
                targetId
            );
        }
    }
}
