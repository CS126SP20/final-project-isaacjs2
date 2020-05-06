#include <cinder/Font.h>
#include <cinder/ImageIo.h>
#include <cinder/Path2d.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/app/App.h>

#include <cinder/gl/Texture.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>

#include <array>
#include <string>
#include <vector>

#include "sudoku/utils.h"

namespace sudoku {

void DrawBox(std::pair<ci::vec2, ci::vec2> bounds, const ci::Color& color) {
  ci::gl::color(color);

  ci::Path2d box;
  box.moveTo(bounds.first);
  box.lineTo(ci::vec2(bounds.second.x, bounds.first.y));
  box.lineTo(bounds.second);
  box.lineTo(ci::vec2(bounds.first.x, bounds.second.y));

  box.close();
  ci::gl::draw(box);
}

void DrawLine(float x1, float y1, float x2, float y2, const ci::Color& color) {
  ci::gl::color(color);

  ci::Path2d line;
  line.moveTo(x1, y1);
  line.lineTo(x2, y2);
  line.close();
  ci::gl::draw(line);
}

ci::vec2 GetMiddleOfBox(std::pair<ci::vec2, ci::vec2> box) {
  // Calculate width and height of box
  float x_diff = box.second.x - box.first.x;
  float y_diff = box.second.y - box.first.y;

  return ci::vec2(box.first.x + x_diff / 2, box.first.y + y_diff / 2);
}

bool IsMouseInBox(const ci::vec2& mouse_pos,
                  const std::pair<ci::vec2, ci::vec2>& box_bounds) {
  return mouse_pos.x > box_bounds.first.x
         && mouse_pos.x < box_bounds.second.x
         && mouse_pos.y > box_bounds.first.y
         && mouse_pos.y < box_bounds.second.y;
}

}