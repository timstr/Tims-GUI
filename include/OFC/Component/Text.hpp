#pragma once

#include <OFC/Component/Component.hpp>
#include <OFC/Component/FontContext.hpp>

#include <OFC/DOM/Text.hpp>

namespace ofc::ui {

    class Text: public SimpleComponent<dom::Text>, public FontConsumer<Text> {
    public:
        Text(Valuelike<String> s);

    private:
        Observer<String> m_stringObserver;

        std::unique_ptr<dom::Text> createElement() override final;

        void updateString(const String& s);

        void updateFont(const sf::Font*);
    };

} // namespace ofc::ui
