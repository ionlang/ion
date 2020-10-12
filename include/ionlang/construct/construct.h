#pragma once

#include <ionshared/tracking/symbol_table.h>
#include <ionshared/construct/base_construct.h>
#include <ionshared/diagnostics/source_location.h>

namespace ionlang {
    enum class ConstructKind {
        Type,

        FunctionBody,

        Prototype,

        Function,

        Extern,

        Global,

        Block,

        VariableRef,

        VariableDecl,

        StructDecl,

        Module,

        Resolvable,

        Expression,

        Statement,

        ErrorMarker,

        Attribute,

        Struct
    };

    class Construct;

    class Pass;

    typedef ionshared::Ast<Construct> Ast;

    struct Construct : ionshared::BaseConstruct<Construct, ConstructKind> {
        template<class T>
        static Ast convertChildren(std::vector<ionshared::Ptr<T>> vector) {
            // TODO: Ensure T is child of AstNode.
            Ast children = {};

            for (const auto &item : vector) {
                children.push_back(item);
            }

            return children;
        }

        template<class T>
        static Ast convertChildren(ionshared::PtrSymbolTable<T> symbolTable) {
            Ast children = {};
            auto symbolTableEntries = symbolTable->unwrap();

            for (const auto &[id, construct] : symbolTableEntries) {
                children.push_back(construct);
            }

            return children;
        }

        template<typename TFirst, typename TSecond>
        static Ast mergeChildren(TFirst first, TSecond second) {
            Ast children = {};

            for (const auto item : first) {
                children.push_back(item);
            }

            for (const auto item : second) {
                children.push_back(item);
            }

            return children;
        }

        explicit Construct(
            ConstructKind kind,
            std::optional<ionshared::SourceLocation> sourceLocation = std::nullopt,
            ionshared::OptPtr<Construct> parent = std::nullopt
        );

        virtual void accept(Pass &visitor) = 0;

        [[nodiscard]] virtual Ast getChildNodes();

        /**
         * Verify the members and properties of the node, and it's children.
         * Without an implementation by the derived class, this will return
         * true if all the child nodes are successfully verified. If there
         * are no child nodes, the result will be true by default.
         */
        [[nodiscard]] virtual bool verify();

        [[nodiscard]] std::optional<std::string> findConstructName();
    };
}
