#pragma once

#include <OFC/Component/Component.hpp>

namespace ofc::ui {

    template<typename T, typename DerivedContextProvider>
    class ContextProvider : public ForwardingComponent {
    public:
        ContextProvider(Value<T> pv)
            : m_value(std::move(pv)) {

        }

        DerivedContextProvider& with(AnyComponent c) {
            m_component = std::move(c);
            return *static_cast<DerivedContextProvider*>(this);
        }

        using ValueType = T;

    private:
        Value<T> m_value;
        AnyComponent m_component;

        void* findContextProvider(const std::type_info& ti) noexcept override final {
            if (ti == typeid(DerivedContextProvider)) {
                Value<T>* valuePtr = &m_value;
                return static_cast<void*>(valuePtr);
            }
            if (auto p = parent()) {
                return p->findContextProvider(ti);
            }
            return nullptr;
        }

        void onMount(const dom::Element* beforeElement) override final {
            m_component.tryMount(this, beforeElement);
        }

        void onUnmount() override final {
            m_component.tryUnmount();
        }

        std::vector<const Component*> getPossibleChildren() const noexcept override final {
            return { m_component.get() };
        }
    };

    template<typename Derived, typename ContextProviderType>
    class ContextConsumer {
    public:
        using ValueType = typename ContextProviderType::ValueType;

        ContextConsumer(void (Derived::* onUpdate)(DiffArgType<ValueType>))
            : m_observer(static_cast<Derived*>(this), onUpdate)
            , m_init(false) {

        }

    protected:
        Observer<ValueType>& getObserver() {
            if (!m_init) {
                if (m_observer.getValue().hasValue()) {
                    m_init = true;
                    return m_observer;
                } else if (auto pv = static_cast<Derived*>(this)->findContext<ContextProviderType>()){
                    m_observer.assign(*pv);
                    m_init = true;
                    return m_observer;
                } else {
                    throw std::runtime_error("Attempted to use context which could not be found in component tree");
                }
            }
            return m_observer;
        }

    private:
        Observer<ValueType> m_observer;
        bool m_init;
    };

} // namespace ofc::ui
