#pragma once
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
  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
  EVENT_CLASS_TYPE(MouseButtonPressed)
};
class BLZR_API MouseButtonPressed : public Event {
public:
  MouseButtonPressed(float x, float y) : m_MouseX(x), m_MouseY(y) {}

  inline float getX() const { return m_MouseX; }
  inline float getY() const { return m_MouseY; }

  std::string toString() const override {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << m_MouseX << ", " << m_MouseY;
    return ss.str();
  }

private:
  float m_MouseX, m_MouseY;
};
} // namespace Blazr
