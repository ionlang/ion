#include <ionlang/misc/ionir_emitted_entities.h>

namespace ionlang {
    IonIrEmittedEntities::IonIrEmittedEntities() :
        entities() {
        //
    }

    void IonIrEmittedEntities::set(
        const std::shared_ptr<Construct> &key,
        const std::shared_ptr<ionir::Construct> &value
    ) {
        this->entities.set(key, value, true);
    }

    bool IonIrEmittedEntities::contains(std::shared_ptr<Construct> key) const {
        return this->entities.contains(std::move(key));
    }
}
