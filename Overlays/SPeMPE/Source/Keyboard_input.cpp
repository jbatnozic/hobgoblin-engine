
#include <SPeMPE/Utility/Keyboard_input.hpp>

#include <Hobgoblin/Common.hpp>
#include <SFML/Window.hpp>

#include <unordered_map>

namespace jbatnozic {
namespace spempe {

namespace {
struct KeyMapping {
    sf::Keyboard::Key spempeToSfKeyMapping[static_cast<unsigned>(KbKey::KeyCount)];
    KbKey             sfToSpempeKeyMapping[sf::Keyboard::KeyCount];

    std::unordered_map<KbKey, std::string> keyToStringMapping;
    std::unordered_map<std::string, KbKey> stringToKeyMapping;

    KeyMapping() {
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::A)]         = sf::Keyboard::A;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::B)]         = sf::Keyboard::B;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::C)]         = sf::Keyboard::C;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::D)]         = sf::Keyboard::D;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::E)]         = sf::Keyboard::E;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F)]         = sf::Keyboard::F;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::G)]         = sf::Keyboard::G;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::H)]         = sf::Keyboard::H;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::I)]         = sf::Keyboard::I;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::J)]         = sf::Keyboard::J;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::K)]         = sf::Keyboard::K;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::L)]         = sf::Keyboard::L;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::M)]         = sf::Keyboard::M;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::N)]         = sf::Keyboard::N;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::O)]         = sf::Keyboard::O;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::P)]         = sf::Keyboard::P;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Q)]         = sf::Keyboard::Q;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::R)]         = sf::Keyboard::R;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::S)]         = sf::Keyboard::S;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::T)]         = sf::Keyboard::T;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::U)]         = sf::Keyboard::U;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::V)]         = sf::Keyboard::V;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::W)]         = sf::Keyboard::W;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::X)]         = sf::Keyboard::X;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Y)]         = sf::Keyboard::Y;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Z)]         = sf::Keyboard::Z;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num0)]      = sf::Keyboard::Num0;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num1)]      = sf::Keyboard::Num1;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num2)]      = sf::Keyboard::Num2;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num3)]      = sf::Keyboard::Num3;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num4)]      = sf::Keyboard::Num4;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num5)]      = sf::Keyboard::Num5;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num6)]      = sf::Keyboard::Num6;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num7)]      = sf::Keyboard::Num7;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num8)]      = sf::Keyboard::Num8;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Num9)]      = sf::Keyboard::Num9;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Escape)]    = sf::Keyboard::Escape;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::LControl)]  = sf::Keyboard::LControl;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::LShift)]    = sf::Keyboard::LShift;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::LAlt)]      = sf::Keyboard::LAlt;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::LSystem)]   = sf::Keyboard::LSystem;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::RControl)]  = sf::Keyboard::RControl;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::RShift)]    = sf::Keyboard::RShift;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::RAlt)]      = sf::Keyboard::RAlt;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::RSystem)]   = sf::Keyboard::RSystem;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Menu)]      = sf::Keyboard::Menu;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::LBracket)]  = sf::Keyboard::LBracket;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::RBracket)]  = sf::Keyboard::RBracket;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Semicolon)] = sf::Keyboard::Semicolon;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Comma)]     = sf::Keyboard::Comma;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Period)]    = sf::Keyboard::Period;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Quote)]     = sf::Keyboard::Quote;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Slash)]     = sf::Keyboard::Slash;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Backslash)] = sf::Keyboard::Backslash;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Tilde)]     = sf::Keyboard::Tilde;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Equal)]     = sf::Keyboard::Equal;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Hyphen)]    = sf::Keyboard::Hyphen;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Space)]     = sf::Keyboard::Space;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Enter)]     = sf::Keyboard::Enter;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Backspace)] = sf::Keyboard::Backspace;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Tab)]       = sf::Keyboard::Tab;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::PageUp)]    = sf::Keyboard::PageUp;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::PageDown)]  = sf::Keyboard::PageDown;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::End)]       = sf::Keyboard::End;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Home)]      = sf::Keyboard::Home;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Insert)]    = sf::Keyboard::Insert;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Delete)]    = sf::Keyboard::Delete;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Add)]       = sf::Keyboard::Add;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Subtract)]  = sf::Keyboard::Subtract;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Multiply)]  = sf::Keyboard::Multiply;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Divide)]    = sf::Keyboard::Divide;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Left)]      = sf::Keyboard::Left;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Right)]     = sf::Keyboard::Right;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Up)]        = sf::Keyboard::Up;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Down)]      = sf::Keyboard::Down;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad0)]   = sf::Keyboard::Numpad0;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad1)]   = sf::Keyboard::Numpad1;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad2)]   = sf::Keyboard::Numpad2;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad3)]   = sf::Keyboard::Numpad3;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad4)]   = sf::Keyboard::Numpad4;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad5)]   = sf::Keyboard::Numpad5;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad6)]   = sf::Keyboard::Numpad6;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad7)]   = sf::Keyboard::Numpad7;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad8)]   = sf::Keyboard::Numpad8;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Numpad9)]   = sf::Keyboard::Numpad9;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F1)]        = sf::Keyboard::F1;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F2)]        = sf::Keyboard::F2;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F3)]        = sf::Keyboard::F3;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F4)]        = sf::Keyboard::F4;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F5)]        = sf::Keyboard::F5;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F6)]        = sf::Keyboard::F6;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F7)]        = sf::Keyboard::F7;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F8)]        = sf::Keyboard::F8;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F9)]        = sf::Keyboard::F9;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F10)]       = sf::Keyboard::F10;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F11)]       = sf::Keyboard::F11;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F12)]       = sf::Keyboard::F12;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F13)]       = sf::Keyboard::F13;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F14)]       = sf::Keyboard::F14;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::F15)]       = sf::Keyboard::F15;
        spempeToSfKeyMapping[static_cast<unsigned>(KbKey::Pause)]     = sf::Keyboard::Pause;

        sfToSpempeKeyMapping[sf::Keyboard::A]         = KbKey::A;
        sfToSpempeKeyMapping[sf::Keyboard::B]         = KbKey::B;
        sfToSpempeKeyMapping[sf::Keyboard::C]         = KbKey::C;
        sfToSpempeKeyMapping[sf::Keyboard::D]         = KbKey::D;
        sfToSpempeKeyMapping[sf::Keyboard::E]         = KbKey::E;
        sfToSpempeKeyMapping[sf::Keyboard::F]         = KbKey::F;
        sfToSpempeKeyMapping[sf::Keyboard::G]         = KbKey::G;
        sfToSpempeKeyMapping[sf::Keyboard::H]         = KbKey::H;
        sfToSpempeKeyMapping[sf::Keyboard::I]         = KbKey::I;
        sfToSpempeKeyMapping[sf::Keyboard::J]         = KbKey::J;
        sfToSpempeKeyMapping[sf::Keyboard::K]         = KbKey::K;
        sfToSpempeKeyMapping[sf::Keyboard::L]         = KbKey::L;
        sfToSpempeKeyMapping[sf::Keyboard::M]         = KbKey::M;
        sfToSpempeKeyMapping[sf::Keyboard::N]         = KbKey::N;
        sfToSpempeKeyMapping[sf::Keyboard::O]         = KbKey::O;
        sfToSpempeKeyMapping[sf::Keyboard::P]         = KbKey::P;
        sfToSpempeKeyMapping[sf::Keyboard::Q]         = KbKey::Q;
        sfToSpempeKeyMapping[sf::Keyboard::R]         = KbKey::R;
        sfToSpempeKeyMapping[sf::Keyboard::S]         = KbKey::S;
        sfToSpempeKeyMapping[sf::Keyboard::T]         = KbKey::T;
        sfToSpempeKeyMapping[sf::Keyboard::U]         = KbKey::U;
        sfToSpempeKeyMapping[sf::Keyboard::V]         = KbKey::V;
        sfToSpempeKeyMapping[sf::Keyboard::W]         = KbKey::W;
        sfToSpempeKeyMapping[sf::Keyboard::X]         = KbKey::X;
        sfToSpempeKeyMapping[sf::Keyboard::Y]         = KbKey::Y;
        sfToSpempeKeyMapping[sf::Keyboard::Z]         = KbKey::Z;
        sfToSpempeKeyMapping[sf::Keyboard::Num0]      = KbKey::Num0;
        sfToSpempeKeyMapping[sf::Keyboard::Num1]      = KbKey::Num1;
        sfToSpempeKeyMapping[sf::Keyboard::Num2]      = KbKey::Num2;
        sfToSpempeKeyMapping[sf::Keyboard::Num3]      = KbKey::Num3;
        sfToSpempeKeyMapping[sf::Keyboard::Num4]      = KbKey::Num4;
        sfToSpempeKeyMapping[sf::Keyboard::Num5]      = KbKey::Num5;
        sfToSpempeKeyMapping[sf::Keyboard::Num6]      = KbKey::Num6;
        sfToSpempeKeyMapping[sf::Keyboard::Num7]      = KbKey::Num7;
        sfToSpempeKeyMapping[sf::Keyboard::Num8]      = KbKey::Num8;
        sfToSpempeKeyMapping[sf::Keyboard::Num9]      = KbKey::Num9;
        sfToSpempeKeyMapping[sf::Keyboard::Escape]    = KbKey::Escape;
        sfToSpempeKeyMapping[sf::Keyboard::LControl]  = KbKey::LControl;
        sfToSpempeKeyMapping[sf::Keyboard::LShift]    = KbKey::LShift;
        sfToSpempeKeyMapping[sf::Keyboard::LAlt]      = KbKey::LAlt;
        sfToSpempeKeyMapping[sf::Keyboard::LSystem]   = KbKey::LSystem;
        sfToSpempeKeyMapping[sf::Keyboard::RControl]  = KbKey::RControl;
        sfToSpempeKeyMapping[sf::Keyboard::RShift]    = KbKey::RShift;
        sfToSpempeKeyMapping[sf::Keyboard::RAlt]      = KbKey::RAlt;
        sfToSpempeKeyMapping[sf::Keyboard::RSystem]   = KbKey::RSystem;
        sfToSpempeKeyMapping[sf::Keyboard::Menu]      = KbKey::Menu;
        sfToSpempeKeyMapping[sf::Keyboard::LBracket]  = KbKey::LBracket;
        sfToSpempeKeyMapping[sf::Keyboard::RBracket]  = KbKey::RBracket;
        sfToSpempeKeyMapping[sf::Keyboard::Semicolon] = KbKey::Semicolon;
        sfToSpempeKeyMapping[sf::Keyboard::Comma]     = KbKey::Comma;
        sfToSpempeKeyMapping[sf::Keyboard::Period]    = KbKey::Period;
        sfToSpempeKeyMapping[sf::Keyboard::Quote]     = KbKey::Quote;
        sfToSpempeKeyMapping[sf::Keyboard::Slash]     = KbKey::Slash;
        sfToSpempeKeyMapping[sf::Keyboard::Backslash] = KbKey::Backslash;
        sfToSpempeKeyMapping[sf::Keyboard::Tilde]     = KbKey::Tilde;
        sfToSpempeKeyMapping[sf::Keyboard::Equal]     = KbKey::Equal;
        sfToSpempeKeyMapping[sf::Keyboard::Hyphen]    = KbKey::Hyphen;
        sfToSpempeKeyMapping[sf::Keyboard::Space]     = KbKey::Space;
        sfToSpempeKeyMapping[sf::Keyboard::Enter]     = KbKey::Enter;
        sfToSpempeKeyMapping[sf::Keyboard::Backspace] = KbKey::Backspace;
        sfToSpempeKeyMapping[sf::Keyboard::Tab]       = KbKey::Tab;
        sfToSpempeKeyMapping[sf::Keyboard::PageUp]    = KbKey::PageUp;
        sfToSpempeKeyMapping[sf::Keyboard::PageDown]  = KbKey::PageDown;
        sfToSpempeKeyMapping[sf::Keyboard::End]       = KbKey::End;
        sfToSpempeKeyMapping[sf::Keyboard::Home]      = KbKey::Home;
        sfToSpempeKeyMapping[sf::Keyboard::Insert]    = KbKey::Insert;
        sfToSpempeKeyMapping[sf::Keyboard::Delete]    = KbKey::Delete;
        sfToSpempeKeyMapping[sf::Keyboard::Add]       = KbKey::Add;
        sfToSpempeKeyMapping[sf::Keyboard::Subtract]  = KbKey::Subtract;
        sfToSpempeKeyMapping[sf::Keyboard::Multiply]  = KbKey::Multiply;
        sfToSpempeKeyMapping[sf::Keyboard::Divide]    = KbKey::Divide;
        sfToSpempeKeyMapping[sf::Keyboard::Left]      = KbKey::Left;
        sfToSpempeKeyMapping[sf::Keyboard::Right]     = KbKey::Right;
        sfToSpempeKeyMapping[sf::Keyboard::Up]        = KbKey::Up;
        sfToSpempeKeyMapping[sf::Keyboard::Down]      = KbKey::Down;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad0]   = KbKey::Numpad0;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad1]   = KbKey::Numpad1;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad2]   = KbKey::Numpad2;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad3]   = KbKey::Numpad3;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad4]   = KbKey::Numpad4;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad5]   = KbKey::Numpad5;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad6]   = KbKey::Numpad6;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad7]   = KbKey::Numpad7;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad8]   = KbKey::Numpad8;
        sfToSpempeKeyMapping[sf::Keyboard::Numpad9]   = KbKey::Numpad9;
        sfToSpempeKeyMapping[sf::Keyboard::F1]        = KbKey::F1;
        sfToSpempeKeyMapping[sf::Keyboard::F2]        = KbKey::F2;
        sfToSpempeKeyMapping[sf::Keyboard::F3]        = KbKey::F3;
        sfToSpempeKeyMapping[sf::Keyboard::F4]        = KbKey::F4;
        sfToSpempeKeyMapping[sf::Keyboard::F5]        = KbKey::F5;
        sfToSpempeKeyMapping[sf::Keyboard::F6]        = KbKey::F6;
        sfToSpempeKeyMapping[sf::Keyboard::F7]        = KbKey::F7;
        sfToSpempeKeyMapping[sf::Keyboard::F8]        = KbKey::F8;
        sfToSpempeKeyMapping[sf::Keyboard::F9]        = KbKey::F9;
        sfToSpempeKeyMapping[sf::Keyboard::F10]       = KbKey::F10;
        sfToSpempeKeyMapping[sf::Keyboard::F11]       = KbKey::F11;
        sfToSpempeKeyMapping[sf::Keyboard::F12]       = KbKey::F12;
        sfToSpempeKeyMapping[sf::Keyboard::F13]       = KbKey::F13;
        sfToSpempeKeyMapping[sf::Keyboard::F14]       = KbKey::F14;
        sfToSpempeKeyMapping[sf::Keyboard::F15]       = KbKey::F15;
        sfToSpempeKeyMapping[sf::Keyboard::Pause]     = KbKey::Pause;

        keyToStringMapping[KbKey::A]         = "A";
        keyToStringMapping[KbKey::B]         = "B";
        keyToStringMapping[KbKey::C]         = "C";
        keyToStringMapping[KbKey::D]         = "D";
        keyToStringMapping[KbKey::E]         = "E";
        keyToStringMapping[KbKey::F]         = "F";
        keyToStringMapping[KbKey::G]         = "G";
        keyToStringMapping[KbKey::H]         = "H";
        keyToStringMapping[KbKey::I]         = "I";
        keyToStringMapping[KbKey::J]         = "J";
        keyToStringMapping[KbKey::K]         = "K";
        keyToStringMapping[KbKey::L]         = "L";
        keyToStringMapping[KbKey::M]         = "M";
        keyToStringMapping[KbKey::N]         = "N";
        keyToStringMapping[KbKey::O]         = "O";
        keyToStringMapping[KbKey::P]         = "P";
        keyToStringMapping[KbKey::Q]         = "Q";
        keyToStringMapping[KbKey::R]         = "R";
        keyToStringMapping[KbKey::S]         = "S";
        keyToStringMapping[KbKey::T]         = "T";
        keyToStringMapping[KbKey::U]         = "U";
        keyToStringMapping[KbKey::V]         = "V";
        keyToStringMapping[KbKey::W]         = "W";
        keyToStringMapping[KbKey::X]         = "X";
        keyToStringMapping[KbKey::Y]         = "Y";
        keyToStringMapping[KbKey::Z]         = "Z";
        keyToStringMapping[KbKey::Num0]      = "Num0";
        keyToStringMapping[KbKey::Num1]      = "Num1";
        keyToStringMapping[KbKey::Num2]      = "Num2";
        keyToStringMapping[KbKey::Num3]      = "Num3";
        keyToStringMapping[KbKey::Num4]      = "Num4";
        keyToStringMapping[KbKey::Num5]      = "Num5";
        keyToStringMapping[KbKey::Num6]      = "Num6";
        keyToStringMapping[KbKey::Num7]      = "Num7";
        keyToStringMapping[KbKey::Num8]      = "Num8";
        keyToStringMapping[KbKey::Num9]      = "Num9";
        keyToStringMapping[KbKey::Escape]    = "Escape";
        keyToStringMapping[KbKey::LControl]  = "LControl";
        keyToStringMapping[KbKey::LShift]    = "LShift";
        keyToStringMapping[KbKey::LAlt]      = "LAlt";
        keyToStringMapping[KbKey::LSystem]   = "LSystem";
        keyToStringMapping[KbKey::RControl]  = "RControl";
        keyToStringMapping[KbKey::RShift]    = "RShift";
        keyToStringMapping[KbKey::RAlt]      = "RAlt";
        keyToStringMapping[KbKey::RSystem]   = "RSystem";
        keyToStringMapping[KbKey::Menu]      = "Menu";
        keyToStringMapping[KbKey::LBracket]  = "LBracket";
        keyToStringMapping[KbKey::RBracket]  = "RBracket";
        keyToStringMapping[KbKey::Semicolon] = "Semicolon";
        keyToStringMapping[KbKey::Comma]     = "Comma";
        keyToStringMapping[KbKey::Period]    = "Period";
        keyToStringMapping[KbKey::Quote]     = "Quote";
        keyToStringMapping[KbKey::Slash]     = "Slash";
        keyToStringMapping[KbKey::Backslash] = "Backslash";
        keyToStringMapping[KbKey::Tilde]     = "Tilde";
        keyToStringMapping[KbKey::Equal]     = "Equal";
        keyToStringMapping[KbKey::Hyphen]    = "Hyphen";
        keyToStringMapping[KbKey::Space]     = "Space";
        keyToStringMapping[KbKey::Enter]     = "Enter";
        keyToStringMapping[KbKey::Backspace] = "Backspace";
        keyToStringMapping[KbKey::Tab]       = "Tab";
        keyToStringMapping[KbKey::PageUp]    = "PageUp";
        keyToStringMapping[KbKey::PageDown]  = "PageDown";
        keyToStringMapping[KbKey::End]       = "End";
        keyToStringMapping[KbKey::Home]      = "Home";
        keyToStringMapping[KbKey::Insert]    = "Insert";
        keyToStringMapping[KbKey::Delete]    = "Delete";
        keyToStringMapping[KbKey::Add]       = "Add";
        keyToStringMapping[KbKey::Subtract]  = "Subtract";
        keyToStringMapping[KbKey::Multiply]  = "Multiply";
        keyToStringMapping[KbKey::Divide]    = "Divide";
        keyToStringMapping[KbKey::Left]      = "Left";
        keyToStringMapping[KbKey::Right]     = "Right";
        keyToStringMapping[KbKey::Up]        = "Up";
        keyToStringMapping[KbKey::Down]      = "Down";
        keyToStringMapping[KbKey::Numpad0]   = "Numpad0";
        keyToStringMapping[KbKey::Numpad1]   = "Numpad1";
        keyToStringMapping[KbKey::Numpad2]   = "Numpad2";
        keyToStringMapping[KbKey::Numpad3]   = "Numpad3";
        keyToStringMapping[KbKey::Numpad4]   = "Numpad4";
        keyToStringMapping[KbKey::Numpad5]   = "Numpad5";
        keyToStringMapping[KbKey::Numpad6]   = "Numpad6";
        keyToStringMapping[KbKey::Numpad7]   = "Numpad7";
        keyToStringMapping[KbKey::Numpad8]   = "Numpad8";
        keyToStringMapping[KbKey::Numpad9]   = "Numpad9";
        keyToStringMapping[KbKey::F1]        = "F1";
        keyToStringMapping[KbKey::F2]        = "F2";
        keyToStringMapping[KbKey::F3]        = "F3";
        keyToStringMapping[KbKey::F4]        = "F4";
        keyToStringMapping[KbKey::F5]        = "F5";
        keyToStringMapping[KbKey::F6]        = "F6";
        keyToStringMapping[KbKey::F7]        = "F7";
        keyToStringMapping[KbKey::F8]        = "F8";
        keyToStringMapping[KbKey::F9]        = "F9";
        keyToStringMapping[KbKey::F10]       = "F10";
        keyToStringMapping[KbKey::F11]       = "F11";
        keyToStringMapping[KbKey::F12]       = "F12";
        keyToStringMapping[KbKey::F13]       = "F13";
        keyToStringMapping[KbKey::F14]       = "F14";
        keyToStringMapping[KbKey::F15]       = "F15";
        keyToStringMapping[KbKey::Pause]     = "Pause";

        for (const auto& pair : keyToStringMapping) {
            stringToKeyMapping[pair.second] = pair.first;
        }
    }
};

const KeyMapping MAPPING{};

sf::Keyboard::Key ToSfKey(KbKey aKey) {
    if (aKey == KbKey::Unknown) {
        return sf::Keyboard::Unknown;
    }
    if (aKey >= KbKey::A && aKey < KbKey::KeyCount) {
        return MAPPING.spempeToSfKeyMapping[static_cast<unsigned>(aKey)];
    }
    throw hobgoblin::TracedLogicError{"KbKey value out of range!"};
}

KbKey ToSPeMPEKey(sf::Keyboard::Key aKey) {
    if (aKey == sf::Keyboard::Unknown) {
        return KbKey::Unknown;
    }
    if (aKey >= sf::Keyboard::A && aKey < sf::Keyboard::KeyCount) {
        return MAPPING.sfToSpempeKeyMapping[aKey];
    }
    throw hobgoblin::TracedLogicError{"sf::Keyboard::Key value out of range!"};
}
} // namespace

std::string KbKeyToString(KbKey aKey) {
    if (aKey >= KbKey::A && aKey < KbKey::KeyCount) {
        return MAPPING.keyToStringMapping.at(aKey);
    }
    return "Unknown";
}

KbKey StringToKbKey(const std::string& aString) {
    const auto iter = MAPPING.stringToKeyMapping.find(aString);
    if (iter != MAPPING.stringToKeyMapping.end()) {
        return iter->second;
    }
    return KbKey::Unknown;
}

///////////////////////////////////////////////////////////////////////////
// KBINPUT                                                               //
///////////////////////////////////////////////////////////////////////////

KbInput::KbInput(const KbInputTracker& aTracker)
    : _tracker{aTracker}
{
}

bool KbInput::checkPressed(KbKey aKey, Mode aMode) const {
    if (aKey < KbKey::A || aKey >= KbKey::KeyCount) {
        throw hobgoblin::TracedLogicError{"Must pass a valid KbKey value (except 'Unknown')!"};
    }

    switch (aMode) {
    case Mode::Default:
        return _tracker._controlBlocks[static_cast<std::size_t>(aKey)].isPressed();

    case Mode::Edge:
        return _tracker._controlBlocks[static_cast<std::size_t>(aKey)].isPressedEdge();

    case Mode::Repeated:
        return _tracker._controlBlocks[static_cast<std::size_t>(aKey)].isPressedRepeated();

    case Mode::Direct:
        return sf::Keyboard::isKeyPressed(ToSfKey(aKey));

    default:
        throw hobgoblin::TracedLogicError{"Invalid mode passed!"};
    }
}

bool KbInput::checkReleased(KbKey aKey, Mode aMode) const {
    if (aKey < KbKey::A || aKey >= KbKey::KeyCount) {
        throw hobgoblin::TracedLogicError{"Must pass a valid KbKey value (except 'Unknown')!"};
    }

    switch (aMode) {
    case Mode::Default:
        return !(_tracker._controlBlocks[static_cast<std::size_t>(aKey)].isPressed());

    case Mode::Edge:
        return _tracker._controlBlocks[static_cast<std::size_t>(aKey)].isReleasedEdge();

    case Mode::Repeated:
        return _tracker._controlBlocks[static_cast<std::size_t>(aKey)].isReleasedEdge();

    case Mode::Direct:
        return !sf::Keyboard::isKeyPressed(ToSfKey(aKey));

    default:
        throw hobgoblin::TracedLogicError{"Invalid mode passed!"};
    }
}

const sf::String& KbInput::getTypedText() const {
    return _tracker._typedText;
}

///////////////////////////////////////////////////////////////////////////
// KBINPUT MUTATOR                                                       //
///////////////////////////////////////////////////////////////////////////

void KbInputMutator::setTypedTextMaxLength(hobgoblin::PZInteger aMaxLength) {
    _tracker._typedTextMaxLength = aMaxLength;
    _tracker._limitTypedTextLength();
}

void KbInputMutator::setTypedTextString(sf::String aString) {
    _tracker._typedText = std::move(aString);
    _tracker._limitTypedTextLength();
}

KbInputMutator::KbInputMutator(KbInputTracker& aTracker)
    : _tracker{aTracker}
{
}

///////////////////////////////////////////////////////////////////////////
// CONTROL BLOCK                                                         //
///////////////////////////////////////////////////////////////////////////

/*
Control block _status member is structured like this:

Bit 0: ABS bit for current frame (ABS = 'Absolute', that is, absolute YES or NO for 'is it pressed?')
Bit 1: EDGE bit for current frame (is 1 if the key was pressed/released in this frame)
Bit 2: ABS bit from one frame ago
Bit 3: EDGE bit from one frame ago
...

The whole thing is shifted 2 bits to the left every frame, preserving the ABS bit but clearing the EDGE bit.
*/

#define BIT_0 0x01
#define BIT_1 0x02
#define BIT_2 0x04

void KbInputTracker::KeyControlBlock::advance() {
    const decltype(_status) lowestBit = _status & BIT_0;
    _status = ((_status << 2) & ~(BIT_0 | BIT_1)) | lowestBit;
}

void KbInputTracker::KeyControlBlock::recordPress() {
    _status |= BIT_0; // Set bit 0 (ABS bit)
    _status |= BIT_1; // Set bit 1 (EDGE bit)
}

void KbInputTracker::KeyControlBlock::recordRelease() {
    _status &= ~BIT_0; // Clear bit 0 (ABS bit)
    _status |= BIT_1;  // Set bit 1 (EDGE bit)
}

bool KbInputTracker::KeyControlBlock::isPressed() const {
    return ((_status & BIT_0) != 0);
}

bool KbInputTracker::KeyControlBlock::isPressedEdge() const {
    // Is it pressed now but wasn't in the previous step?
    return (isPressed() && ((_status & BIT_2) == 0));
}

bool KbInputTracker::KeyControlBlock::isPressedRepeated() const {
    return (isPressed() && ((_status & BIT_1) != 0));
}

bool KbInputTracker::KeyControlBlock::isReleasedEdge() const {
    // Is it released now but wasn't in the previous step?
    return (!isPressed() && ((_status & BIT_2) != 0));
}

#undef BIT_2
#undef BIT_1
#undef BIT_0

///////////////////////////////////////////////////////////////////////////
// INPUT TRACKER                                                         //
///////////////////////////////////////////////////////////////////////////

KbInputTracker::KbInputTracker() {
    _controlBlocks.resize(static_cast<std::size_t>(KbKey::KeyCount));
}

KbInput KbInputTracker::getInput() const {
    return KbInput{*this};
}

KbInputMutator KbInputTracker::getMutator() {
    return KbInputMutator{*this};
}

void KbInputTracker::prepForEvents() {
    for (auto& controlBlock : _controlBlocks) {
        controlBlock.advance();
    }
}

void KbInputTracker::keyEventOccurred(const sf::Event& aEvent) {
    auto key = KbKey::Unknown;

    switch (aEvent.type) {
        case sf::Event::KeyPressed:
            key = ToSPeMPEKey(aEvent.key.code);
            if (key != KbKey::Unknown) {
                _controlBlocks[static_cast<std::size_t>(key)].recordPress();
            }
            break;

        case sf::Event::KeyReleased:
            key = ToSPeMPEKey(aEvent.key.code);
            if (key != KbKey::Unknown) {
                _controlBlocks[static_cast<std::size_t>(key)].recordRelease();
            }
            break;

        default:
            throw hobgoblin::TracedLogicError{"Invalid event passed!"};
    }
}

void KbInputTracker::textEventOccurred(const sf::Event& aEvent) {
    if (aEvent.type != sf::Event::TextEntered) {
        throw hobgoblin::TracedLogicError{"Invalid event passed!"};
    }

    const sf::Uint32 character = aEvent.text.unicode;
    // In a backspace is typed, erase the last typed character
    if (character == '\b') {
        const auto size = _typedText.getSize();
        if (size > 0) {
            _typedText.erase(size - 1);
        }
    }
    // Otherwise, append the new character to the string
    else {
        _typedText += character;
        _limitTypedTextLength();
    }
}

void KbInputTracker::_limitTypedTextLength() {
    const auto size = _typedText.getSize();
    if (size > _typedTextMaxLength) {
        _typedText.erase(_typedTextMaxLength, size - _typedTextMaxLength);
    }
}

} // namespace spempe
} // namespace jbatnozic
