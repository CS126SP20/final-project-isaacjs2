#ifndef FINALPROJECT_UTILS_H
#define FINALPROJECT_UTILS_H

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

namespace sudoku {

// Draw a box from the given top left and bottom right points
void DrawBox(std::pair<ci::vec2, ci::vec2> bounds, const ci::Color& color);

// Draw a line from (x1,y1) to (x2,y2)
void DrawLine(float x1, float y1, float x2, float y2, const ci::Color& color);

ci::vec2 GetMiddleOfBox(std::pair<ci::vec2, ci::vec2> box);

bool IsMouseInBox(const ci::vec2& mouse_pos,
                  const std::pair<ci::vec2, ci::vec2>& box_bounds);
}

#endif  // FINALPROJECT_UTILS_H
