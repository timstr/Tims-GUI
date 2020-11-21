#pragma once

#include <OFC/Component/Component.hpp>
#include <OFC/Component/PureComponent.hpp>
#include <OFC/Component/FontContext.hpp>
#include <OFC/DOM/TextEntry.hpp>

#include <charconv>

namespace ofc::ui {

    namespace detail {

        class CallbackTextEntry;

    } // namespace detail

    class TextField : public SimpleComponent<dom::TextEntry>, public FontConsumer<TextField> {
    public:
        TextField(Valuelike<String> s);

        TextField& onChange(std::function<void(const String&)> f);

        TextField& onSubmit(std::function<void(const String&)> f);

        TextField& validate(std::function<bool(const String&)> f);

    private:
        Observer<String> m_stringObserver;
        std::function<void(const String&)> m_onChange;
        std::function<void(const String&)> m_onSubmit;
        std::function<bool(const String&)> m_validate;

        std::unique_ptr<dom::TextEntry> createElement() override final;

        void updateString(const String& s);

        void updateFont(const sf::Font*);

        friend detail::CallbackTextEntry;
    };



    template<typename NumberType>
    class NumberTextField : public PureComponent {
    public:
        static_assert(std::is_arithmetic_v<NumberType>);

        NumberTextField(Valuelike<NumberType> v)
            : m_value(std::move(v)) {
        
        }

        NumberTextField& onSubmit(std::function<void(NumberType)> f) {
            m_onSubmit = std::move(f);
            return *this;
        }

    private:
        Valuelike<NumberType> m_value;
        std::function<void(NumberType)> m_onSubmit;

        AnyComponent render() const noexcept override {
            auto numStr = m_value.map([](NumberType v) -> String { return std::to_string(v); });

            auto convert = [](const String& s) -> std::optional<NumberType> {
                const auto str = s.toAnsiString();
                auto v = NumberType{};
                auto begin = str.c_str();
                auto end = begin + str.size();
                const auto [p, ec] = std::from_chars(begin, end, v);
                if (ec == std::errc{} && p == end){
                    return v;
                }
                return std::nullopt;
            };

            return TextField{std::move(numStr)}
                .validate([convert](const String& s){ return convert(s).has_value(); })
                .onSubmit([this, convert](const String& s){
                    auto v = convert(s);
                    assert(v.has_value());
                    m_onSubmit(*v);
                });
        }
    };

    template<typename NumberType>
    NumberTextField(const Value<NumberType>&) -> NumberTextField<NumberType>;

    template<typename NumberType, typename... Args>
    NumberTextField(const DerivedValue<NumberType, Args...>&) -> NumberTextField<NumberType>;

} // namespace ofc::ui