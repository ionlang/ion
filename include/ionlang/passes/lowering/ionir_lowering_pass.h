#pragma once

#include <ionshared/container/stack.h>
#include <ionir/construct/basic_block.h>
#include <ionlang/misc/ionir_emitted_entities.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class IonIrLoweringPass : public Pass {
    private:
        struct Buffers {
            ionshared::OptPtr<ionir::Module> module = std::nullopt;

            ionshared::OptPtr<ionir::Function> function = std::nullopt;

            ionshared::OptPtr<ionir::BasicBlock> basicBlock = std::nullopt;

            ionshared::OptPtr<ionir::InstBuilder> builder = std::nullopt;
        };

        [[nodiscard]] static ionshared::Ptr<ionir::Type> processTypeQualifiers(
            ionshared::Ptr<ionir::Type> type,
            const ionshared::Ptr<TypeQualifiers> &qualifiers
        );

        ionshared::PtrSymbolTable<ionir::Module> modules;

        Buffers buffers;

        IonIrEmittedEntities symbolTable;

        uint32_t nameCounter;

        ionshared::Ptr<ionir::Module> requireModule();

        ionshared::Ptr<ionir::Function> requireFunction();

        ionshared::Ptr<ionir::BasicBlock> requireBasicBlock();

        ionshared::Ptr<ionir::InstBuilder> requireBuilder();

        /**
         * Set the currently active builder if any. Modifying the builder
         * will also set/update the active IonIR basic block buffer.
         */
        void setBuilder(ionshared::Ptr<ionir::BasicBlock> basicBlock);

        /**
         * Backup current buffers, invoke the callback and restore the
         * stashed buffers.
         */
        void stashBuffers(const std::function<void()>& callback);

        [[nodiscard]] uint32_t getNameCounter() noexcept;

        /**
         * Visit and emit a construct if it has not been already
         * previously visited and emitted, and return the resulting
         * lowered construct by looking it up on the local symbol table.
         * By default, buffers are stashed during visitation of the
         * construct and restored afterwards to avoid being overwritten.
         */
        template<typename T = ionir::Construct>
            requires std::derived_from<T, ionir::Construct>
        ionshared::Ptr<T> safeEarlyVisitOrLookup(
            ionshared::Ptr<Construct> construct,
            bool useDynamicCast = true,
            bool stashBuffers = true
        ) {
            /**
             * NOTE: If specified, buffers must be stashed and restored after
             * visitation, as the construct could be anything, including a block
             * or a function, which, among others, alter buffers when being lowered.
             */
            if (stashBuffers) {
                this->stashBuffers([&, this] {
                    /**
                     * NOTE: Construct is protected from being emitted more
                     * than once during this call.
                     */
                    this->visit(construct);
                });
            }
            else {
                this->visit(construct);
            }

            if (!this->symbolTable.contains(construct)) {
                throw std::runtime_error("Visiting construct did not create an entry in the local symbol table");
            }

            return *this->symbolTable.find<T>(construct, useDynamicCast);
        }

    public:
        explicit IonIrLoweringPass(
            ionshared::Ptr<ionshared::PassContext> context,

            ionshared::PtrSymbolTable<ionir::Module> modules =
                std::make_shared<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>>()
        );

        ~IonIrLoweringPass();

        [[nodiscard]] ionshared::Ptr<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>> getModules() const;

        [[nodiscard]] ionshared::OptPtr<ionir::Module> getModuleBuffer() const;

        bool setModuleBuffer(const std::string &id);

        /**
         * Visit a construct for lowering. Will not visit children,
         * as they are already visited by the delegated visit methods.
         * Must be used as the default visiting method for constructs,
         * as it will prevent constructs from possibly being emitted more
         * than once (for example, when a construct is used earlier than
         * its official lowering point).
         */
        void visit(ionshared::Ptr<Construct> node) override;

        void visitModule(ionshared::Ptr<Module> node) override;

        void visitFunction(ionshared::Ptr<Function> node) override;

        void visitExtern(ionshared::Ptr<Extern> construct) override;

        void visitPrototype(ionshared::Ptr<Prototype> node) override;

        void visitBlock(ionshared::Ptr<Block> construct) override;

        void visitIntegerLiteral(ionshared::Ptr<IntegerLiteral> node) override;

        void visitCharLiteral(ionshared::Ptr<CharLiteral> node) override;

        void visitStringLiteral(ionshared::Ptr<StringLiteral> node) override;

        void visitBooleanLiteral(ionshared::Ptr<BooleanLiteral> node) override;

        void visitGlobal(ionshared::Ptr<Global> construct) override;

        void visitIntegerType(ionshared::Ptr<IntegerType> construct) override;

        void visitBooleanType(ionshared::Ptr<BooleanType> construct) override;

        void visitVoidType(ionshared::Ptr<VoidType> construct) override;

        void visitIfStatement(ionshared::Ptr<IfStatement> construct) override;

        void visitReturnStatement(ionshared::Ptr<ReturnStatement> construct) override;

        void visitAssignmentStatement(ionshared::Ptr<AssignmentStatement> construct) override;

        void visitVariableDecl(ionshared::Ptr<VariableDeclStatement> construct) override;

        void visitCallExpr(ionshared::Ptr<CallExpr> construct) override;

        void visitOperationExpr(ionshared::Ptr<OperationExpr> construct) override;

        void visitStruct(ionshared::Ptr<Struct> construct) override;

        void visitStructDefinition(ionshared::Ptr<StructDefinition> construct) override;

        // TODO: visitRef() if !isResolved() error, else this->visit(ref->getValue()); Is this a good idea? It may be required for example for if statement condition (if it's a ref). Investigate.
    };
}
