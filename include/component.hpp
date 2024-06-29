#include <memory>
#include "entity.hpp"

namespace ShiftFlamework {

template <typename ComponentStoreClass, typename ComponentClass>
concept ComponentStore = requires (ComponentStoreClass store, ComponentClass component){
    {store.initialize()} -> std::same_as<void>;
    {store.create()} -> std::same_as<std::shared_ptr<ComponentClass>> ;
    {store.get()} -> std::same_as<std::shared_ptr<ComponentClass>> ;
    {store.remove()} -> std::same_as<void>;

    {component.get_entity()} -> std::same_as<std::shared_ptr<Entity>>;
    {component.entity_id} -> std::same_as<EntityID>;
    {ComponentClass::get_store()} -> std::same_as<std::shared_ptr<ComponentStoreClass>>;
};

}