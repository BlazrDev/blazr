#pragma once
#include "Event.h"
#include <sstream>

namespace Blazr {
class BLZR_API KeyEvent : public Event {

protected:
  int m_KeyCode;

public:
  inline int getKeyCode() const { return m_KeyCode; }
  EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
  KeyEvent(int keyCode) : m_KeyCode(keyCode) {}
};

class BLZR_API KeyPressedEvent : public KeyEvent {
private:
  int m_RepeatCount;

public:
  KeyPressedEvent(int keyCode, int repeatCount)
      : KeyEvent(keyCode), m_RepeatCount(repeatCount) {}
  inline int getRepeatCount() const { return m_RepeatCount; }

  std::string toString() const override {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << m_KeyCode << " is "
       << (m_RepeatCount == 0 ? "pressed" : "repeating");
    return ss.str();
  }

  EVENT_CLASS_TYPE(KeyPressed)
};

class BLZR_API KeyReleasedEvent : public KeyEvent {
public:
  KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

  std::string toString() const override {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << m_KeyCode;
    return ss.str();
  }
  EVENT_CLASS_TYPE(KeyReleased)
};
} // namespace Blazr
