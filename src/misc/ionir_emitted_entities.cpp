#include <ionlang/misc/ionir_emitted_entities.h>

namespace ionlang {
    IonIrEmittedEntities::IonIrEmittedEntities() :
        entities() {
        //
    }

    void IonIrEmittedEntities::set(
        const ionshared::Ptr<Construct> &key,
        const ionshared::Ptr<ionir::Construct> &value
    ) {
        this->entities.set(key, value, true);
    }

    bool IonIrEmittedEntities::contains(ionshared::Ptr<Construct> key) const {
        return this->entities.contains(std::move(key));
    }
}
