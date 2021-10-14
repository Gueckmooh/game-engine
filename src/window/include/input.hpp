#pragma once

#include <boost/optional.hpp>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <logging/logger.hpp>
#include <macros/macros.hpp>
#include <window/keys.hpp>
#include <window/window_system_events.hpp>

namespace window {
namespace input {

// @todo move this in cpp

class Input {
  public:
    using InputKindType = uint8_t;
    enum class InputKind : InputKindType { ControllerInput, KeyboardInput, Unknown };
    using InputUnion = union {
        key::ControllerKey controllerKey;
        key::KeyboardKey keyboardKey;
    };

  private:
    InputKind fKind;
    InputUnion fInput;

  public:
    key::KeyboardKey keyboardKey() {
        if (fKind == InputKind::KeyboardInput) {
            return fInput.keyboardKey;
        } else {
            return key::KeyboardKey::Unknown;
        }
    }

    key::ControllerKey controllerKey() {
        if (fKind == InputKind::ControllerInput) {
            return fInput.controllerKey;
        } else {
            return key::ControllerKey::Unknown;
        }
    }

    Input(key::ControllerKey key) : fKind(InputKind::ControllerInput) {
        fInput.controllerKey = key;
    }
    Input(key::KeyboardKey key) : fKind(InputKind::KeyboardInput) {
        fInput.keyboardKey = key;
    }
    Input() : fKind(InputKind::KeyboardInput) {}

    void operator=(key::ControllerKey key) {
        fKind                = InputKind::ControllerInput;
        fInput.controllerKey = key;
    }
    void operator=(key::KeyboardKey key) {
        fKind              = InputKind::KeyboardInput;
        fInput.keyboardKey = key;
    }

    friend std::ostream& operator<<(std::ostream& os, Input& input) {
        switch (input.fKind) {
        case Input::InputKind::KeyboardInput: os << input.fInput.keyboardKey; break;
        case Input::InputKind::ControllerInput: os << input.fInput.controllerKey; break;
        default: os << "<unknown>"; break;
        }
        return os;
    }
};

class ControllerInput {
  private:
    $enum_to_vars_decls(f, _MControllerKeys);

  public:
    $enum_to_vars_getter(getKey, key, ControllerKey, f, _MControllerKeys);
    $enum_to_vars_setter(setKey, key, ControllerKey, f, _MControllerKeys);

    virtual void readInput() = 0;
};

class KeyboardInput {
  private:
    $enum_to_vars_decls(f, _MKeyboardKeys);

    void handleEvent(WsEvent event) {
        if (event.type == WsEventType::KeyPressed) {
            setKey(event.key.which, true);
        } else if (event.type == WsEventType::KeyReleased) {
            setKey(event.key.which, false);
        }
    }

  public:
    $enum_to_vars_getter(getKey, key, KeyboardKey, f, _MKeyboardKeys);
    $enum_to_vars_setter(setKey, key, KeyboardKey, f, _MKeyboardKeys);

    void registerToEventDispatcher(WsEventDispatcher& eventDispatcher) {
        eventDispatcher.registerHandler(WsEventType::KeyPressed,
                                        [this](WsEvent event) { handleEvent(event); });
        eventDispatcher.registerHandler(WsEventType::KeyReleased,
                                        [this](WsEvent event) { handleEvent(event); });
    }

    // virtual void readInput() = 0;
};

class InputCombination {
    std::vector<Input> fInputVector;
    Input fSimpleInput;
    bool fIsSimpleInput;

  public:
    InputCombination(Input input) : fSimpleInput(input), fIsSimpleInput(true) {}
    InputCombination(std::initializer_list<Input> inputs) : fIsSimpleInput(false) {
        fInputVector.insert(fInputVector.begin(), inputs);
    }

    friend std::ostream& operator<<(std::ostream& os, InputCombination& ic) {
        os << "{ ";
        if (ic.fIsSimpleInput) {
            os << ic.fSimpleInput << " }";
        } else {
            for (auto input : ic.fInputVector) { os << input << ", "; }
            os << " }";
        }
        return os;
    }

    // @todo do it for keyboard
    bool isActive(ControllerInput& cInput) {
        if (fIsSimpleInput) {
            auto key = fSimpleInput.controllerKey();
            return cInput.getKey(key);
        }
        for (auto& input : fInputVector) {
            auto key = input.controllerKey();
            if (key != key::ControllerKey::Unknown) {
                if (!cInput.getKey(key)) { return false; }
            } else {
                return false;
            }
        }
        return true;
    }

    bool isActive(KeyboardInput& kInput) {
        if (fIsSimpleInput) {
            auto key = fSimpleInput.keyboardKey();
            return kInput.getKey(key);
        }
        for (auto& input : fInputVector) {
            auto key = input.keyboardKey();
            if (key != key::KeyboardKey::Unknown) {
                if (!kInput.getKey(key)) { return false; }
            } else {
                return false;
            }
        }
        return true;
    }
};

class InputManager {
  private:
    std::multimap<std::string_view, InputCombination> fInputMap;
    std::set<std::string_view> fRecordFilter;

    std::set<std::string_view> fActiveInput;
    std::set<std::string_view> fUpInput;
    std::set<std::string_view> fDownInput;

    boost::optional<ControllerInput&> fMaybeControllerInput;
    boost::optional<KeyboardInput&> fMaybeKeyboardInput;

    bool fRecording        = false;
    bool fReplaying        = false;
    unsigned fReplayCursor = 0;
    std::vector<std::set<std::string_view>> fRecord;

    void setActive(std::string_view key, bool value) {
        if (value) {
            if (fActiveInput.find(key) == fActiveInput.end()) {
                fUpInput.insert(key);
                fActiveInput.insert(key);
            }
        } else {
            if (fActiveInput.find(key) != fActiveInput.end()) {
                fDownInput.insert(key);
                fActiveInput.erase(key);
            }
        }
    }

  public:
    InputManager(ControllerInput& controllerInput, KeyboardInput& keyboardInput)
        : fMaybeControllerInput(controllerInput), fMaybeKeyboardInput(keyboardInput) {}
    InputManager(KeyboardInput& keyboardInput) : fMaybeKeyboardInput(keyboardInput) {}

    void addMapping(std::string_view key, InputCombination inputs) {
        fInputMap.emplace(key, inputs);
    }

    void dontRecord(std::string_view key) { fRecordFilter.insert(key); }

    bool isActive(std::string_view key) {
        if (!fReplaying || (fRecordFilter.find(key) != fRecordFilter.end())) {
            return fActiveInput.find(key) != fActiveInput.end();
        } else {
            auto cursor = fReplayCursor - 1;
            if (cursor < fRecord.size())
                return fRecord.at(cursor).find(key) != fRecord.at(cursor).end();
            else
                return false;
        }
    }

    bool isUp(std::string_view key) {
        // @todo record that
        return fUpInput.find(key) != fUpInput.end();
    }

    bool isDown(std::string_view key) {
        // @todo record that
        return fDownInput.find(key) != fDownInput.end();
    }

    void readInput() {
        if (fMaybeControllerInput) fMaybeControllerInput->readInput();
    }

    void computeInput() {
        fUpInput.clear();
        fDownInput.clear();
        for (auto pair : fInputMap) {
            if ((fMaybeControllerInput && pair.second.isActive(*fMaybeControllerInput))
                || (fMaybeKeyboardInput && pair.second.isActive(*fMaybeKeyboardInput))) {
                setActive(pair.first, true);
            } else {
                setActive(pair.first, false);
            }
        }
        if (fRecording) fRecord.push_back(fActiveInput);
        if (fReplaying) fReplayCursor++;
    }

    // ----- For recording
    void startRecord() {
        logging::logger.info("window.input.InputManager") << "start record" << std::endl;
        fRecord.clear();
        fRecording = true;
    }

    void stopRecord() {
        logging::logger.info("window.input.InputManager") << "stop record" << std::endl;
        fRecording = false;
    }

    void startReplay() {
        if (!fReplaying)
            logging::logger.info("window.input.InputManager")
                << "start replay" << std::endl;
        fReplayCursor = 0;
        fReplaying    = true;
    }

    void stopReplay() {
        logging::logger.info("window.input.InputManager") << "stop replay" << std::endl;
        fReplaying = false;
    }
};

#undef _MControllerKeys
#undef _MKeyboardKeys
#undef _MMouseKeys
#undef _MWheelKeys

}   // namespace input
}   // namespace window
