#pragma once

#include <GUI/Component/Component.hpp>

namespace ui {

    class List : public ForwardingComponent {
    public:
        template<typename... ComponentTypes>
        List(ComponentTypes&&... components) {
            static_assert((... && std::is_convertible_v<ComponentTypes, AnyComponent>), "All arguments to List must be convertible to AnyComponent");
            int dummy[sizeof...(ComponentTypes)] = {
                (m_components.emplace_back(std::forward<ComponentTypes>(components)), 0)...
            };
        }

    private:
        std::vector<AnyComponent> m_components;

        void onMount(const dom::Element* beforeSibling) override final;

        void onUnmount() override final;

        std::vector<const Component*> getChildren() const noexcept override final;
    };

} // namespace ui 
