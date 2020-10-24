#pragma once

#include <ionshared/tracking/symbol_table.h>
#include <ionshared/construct/base_construct.h>
#include <ionshared/diagnostics/source_location.h>

namespace ionlang {
    enum struct ConstructKind {
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

    struct Construct;

    struct Pass;

    typedef ionshared::Ast<Construct> Ast;

    struct Construct : ionshared::BaseConstruct<Construct, ConstructKind> {
        template<class T>
        static Ast convertChildren(std::vector<std::shared_ptr<T>> vector) {
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

        /**
         * Create a shared pointer to a construct, while setting its
         * parent. Should be preferred instead of creating the shared
         * pointer and then setting its parent manually for consistency
         * and error-preventing measures.
         */
        template<typename TConstruct, typename ...TArgs>
        requires std::derived_from<TConstruct, Construct>
        static std::shared_ptr<TConstruct> makeChild(
            const std::shared_ptr<Construct>& parent,
            TArgs&&... args
        ) {
            std::shared_ptr<TConstruct> construct =
                std::make_shared<TConstruct>(std::forward<TArgs>(args)...);

            construct->parent = parent;

            // TODO: Consider having a virtual default-empty 'acceptChild(<ConstructTypeHere> child)' which inserts it into a corresponding list (if any)

            return construct;
        }

        explicit Construct(
            ConstructKind kind,
            std::optional<ionshared::SourceLocation> sourceLocation = std::nullopt,
            ionshared::OptPtr<Construct> parent = std::nullopt
        );

        virtual void accept(Pass& visitor) = 0;

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
