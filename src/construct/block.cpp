#include <utility>
#include <ionlang/passes/pass.h>
#include <ionlang/misc/statement_builder.h>

namespace ionlang {
    std::shared_ptr<Block> Block::make(
        const std::vector<std::shared_ptr<Statement>>& statements,
        const ionshared::PtrSymbolTable<VariableDeclStmt>& symbolTable
    ) noexcept {
        std::shared_ptr<Block> result =
            std::make_shared<Block>(statements, symbolTable);

        for (const auto& statement : statements) {
            statement->parent = result;
        }

        return result;
    }

    Block::Block(
        std::vector<std::shared_ptr<Statement>> statements,
        const ionshared::PtrSymbolTable<VariableDeclStmt>& symbolTable
    ) :
        ConstructWithParent<Construct>(ConstructKind::Block),
        ionshared::Scoped<VariableDeclStmt>(symbolTable),
        statements(std::move(statements)) {
        //
    }

    void Block::accept(Pass& visitor) {
        // TODO: Cast fails.
//        visitor.visitScopeAnchor(this->dynamicCast<ionshared::Scoped<Construct>>());
        visitor.visitBlock(this->dynamicCast<Block>());
    }

    Ast Block::getChildNodes() {
        return Construct::convertChildren(this->statements);
    }

    void Block::appendStatement(const std::shared_ptr<Statement>& statement) {
        this->statements.push_back(statement);

        /**
         * Variable declaration statements should be registered on
         * the local symbol table.
         */
        if (statement->statementKind == StatementKind::VariableDeclaration) {
            std::shared_ptr<VariableDeclStmt> variableDecl =
                statement->dynamicCast<VariableDeclStmt>();

            this->symbolTable->set(variableDecl->name, variableDecl);
        }

        // TODO: What about other named statements? Currently there might be none -- but in the future this might be an edge case, it's really daunting to write checks for each named construct (also recall there's Identifier, so we can't just std::dynamic_pointer_cast<ionshared::Named>).
    }

    bool Block::relocateStatement(size_t orderIndex, const std::shared_ptr<Block>& target) {
        /**
         * The size of the local statements vector is less than
         * the provided index.
         */
        if (!ionshared::util::indexExistsInVector(orderIndex, this->statements)) {
            return false;
        }
        // Prevent the target from being this same instance.
        else if (target.get() == this) {
            return false;
        }

        // TODO: Relocate symbol table entries, but making a map Statement -> Vector<Symbol Table Key>. Remember statement is already registered on the local symbol table on the appendStatement() call.

        /**
         * NOTE: The statement is registered on foreign block's symbol
         * table during this call.
         */
        target->appendStatement(this->statements[orderIndex]);

        this->statements.erase(this->statements.begin() + orderIndex);

        return true;
    }

    size_t Block::relocateStatements(
        const std::shared_ptr<Block> &target,
        size_t from,
        std::optional<size_t> to
    ) {
        bool areStatementsEmpty = this->statements.empty();

        if (to.has_value()) {
            if (*to > from) {
                throw std::out_of_range("To cannot be after from");
            }
            else if (!areStatementsEmpty && *to > this->statements.size() - 1) {
                throw std::out_of_range("Provided order is outsize of bounds");
            }
        }

        if (!areStatementsEmpty && from > this->statements.size() - 1) {
            throw std::out_of_range("Provided order is outsize of bounds");
        }

        size_t statementsRelocated = 0;

        auto toIterator = to.has_value()
            ? this->statements.begin() + *to
            : this->statements.end();

        for (auto i = this->statements.begin() + from; i != toIterator; i++) {
            if (this->relocateStatement(i - this->statements.begin(), target)) {
                statementsRelocated++;
            }
        }

        return statementsRelocated;
    }

    std::shared_ptr<Block> Block::slice(size_t from, std::optional<size_t> to) {
        std::shared_ptr<Block> newBlock =
            Construct::makeChild<Block>(this->forceGetUnboxedParent());

        /**
         * NOTE: Index boundary checks are performed when relocation
         * occurs, therefore there is no need to re-write them here.
         */
        this->relocateStatements(newBlock, from, to);

        return newBlock;
    }

    std::optional<size_t> Block::locate(std::shared_ptr<Statement> construct) const {
        return ionshared::util::locateInVector<std::shared_ptr<Statement>>(
            this->statements,
            std::move(construct)
        );
    }

    std::shared_ptr<StatementBuilder> Block::createBuilder() {
        return std::make_shared<StatementBuilder>(this->dynamicCast<Block>());
    }

    std::vector<std::shared_ptr<Statement>> Block::findTerminals() const {
        std::vector<std::shared_ptr<Statement>> terminalStatements = {};

        for (const auto &statement : this->statements) {
            if (statement->isTerminal()) {
                terminalStatements.push_back(statement);
            }
        }

        return terminalStatements;
    }

    ionshared::OptPtr<Statement> Block::findFirstStatement() noexcept {
        if (!this->statements.empty()) {
            return this->statements.front();
        }

        return std::nullopt;
    }

    ionshared::OptPtr<Statement> Block::findLastStatement() noexcept {
        if (!this->statements.empty()) {
            return this->statements.back();
        }

        return std::nullopt;
    }

    ionshared::OptPtr<Function> Block::findParentFunction() {
        ionshared::OptPtr<Function> parentFunction = std::nullopt;
        std::queue<std::shared_ptr<ConstructWithParent<Block>>> childrenOfBlockQueue = {};

        childrenOfBlockQueue.push(this->staticCast<ConstructWithParent<Block>>());

        while (!childrenOfBlockQueue.empty()) {
            std::shared_ptr<ConstructWithParent<Block>> childOfBlock =
                childrenOfBlockQueue.front();

            childrenOfBlockQueue.pop();

            std::shared_ptr<Construct> parent = *childOfBlock->parent;

            if (parent->constructKind == ConstructKind::Function) {
                parentFunction = parent->dynamicCast<Function>();

                break;
            }
            else if (dynamic_cast<ConstructWithParent<Block> *>(parent.get())) {
                childrenOfBlockQueue.push(parent->dynamicCast<ConstructWithParent<Block>>());
            }
        }

        return parentFunction;
    }
}
