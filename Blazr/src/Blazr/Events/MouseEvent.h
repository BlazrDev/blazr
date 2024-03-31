#pragma once
#include "../Core/MouseCodes.h"
#include "Event.h"
#include <sstream>

namespace Blazr {
class BLZR_API MouseMovedEvent : public Event {

public:
  MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

  inline float getX() const { return m_MouseX; }
  inline float getY() const { return m_MouseY; }

  std::string toString() const override {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseMoved)
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
private:
  float m_MouseX, m_MouseY;
};

class BLZR_API MouseButtonEvent : public Event {
public:
  inline MouseCode getMouseButton() { return m_Button; }
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

protected:
  MouseButtonEvent(MouseCode code) : m_Button(code) {}
  MouseCode m_Button;
};

class BLZR_API MouseButtonPressedEvent : public MouseButtonEvent {
public:
  MouseButtonPressedEvent(const MouseCode button) : MouseButtonEvent(button) {}

  std::string toString() const override {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << m_Button;
    return ss.str();
  }
  EVENT_CLASS_TYPE(MouseButtonPressed)
};

class BLZR_API MouseButtonReleasedEvent : public MouseButtonEvent {
public:
  MouseButtonReleasedEvent(const MouseCode button) : MouseButtonEvent(button) {}
  std::string toString() const override {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << m_Button;
    return ss.str();
  }
  EVENT_CLASS_TYPE(MouseButtonReleased)
};

class BLZR_API MouseScrolledEvent : public Event {
public:
  MouseScrolledEvent(const float xOffset, const float yOffset)
      : m_XOffset(xOffset), m_YOffset(yOffset) {}

  inline float getXOffset() const { return m_XOffset; }
  inline float getYOffset() const { return m_YOffset; }

  std::string toString() const override {
    std::stringstream ss;
    ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseScrolled)
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
  float m_XOffset, m_YOffset;
};
} // namespace Blazr
