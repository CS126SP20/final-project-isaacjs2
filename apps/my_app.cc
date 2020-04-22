// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/Font.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/app/App.h>
#include <cpr/api.h>

#include "../cmake-build-debug/_deps/cpr-src/include/cpr/api.h"
#include "../cmake-build-debug/_deps/cpr-src/include/cpr/auth.h"
#include "../cmake-build-debug/_deps/cpr-src/include/cpr/cprtypes.h"
#include "../cmake-build-debug/_deps/cpr-src/include/cpr/parameters.h"

namespace myapp {

using cinder::app::KeyEvent;

MyApp::MyApp()
    : state_{GameState::kMenu}
    {}

void MyApp::setup() {
  ci::gl::enableDepthWrite();
  ci::gl::enableDepthRead();
}

void MyApp::update() {
//  auto r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
//                   cpr::Authentication{"user", "pass"},
//                   cpr::Parameters{{"anon", "true"}, {"key", "value"}});
//  std::cout << r.text << std::endl;
}

void MyApp::draw() {
  if (state_ == GameState::kMenu) {
    DrawMenu();
  }

  if (state_ == GameState::kPlaying) {
    DrawGrid();
  }
}

template <typename C>
void PrintText(const std::string& text,
               const C& color,
               const cinder::ivec2& size,
               const cinder::vec2& loc) {
  cinder::gl::color(color);
  const char font[] = "Arial";

  auto box = ci::TextBox()
      .alignment(ci::TextBox::CENTER)
      .font(cinder::Font(font, 30))
      .size(size)
      .color(color)
      .backgroundColor(ci::ColorA(0, 0, 0, 0))
      .text(text);

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {loc.x - box_size.x / 2, loc.y - box_size.y / 2};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);
}

void MyApp::DrawMenu() const {
  ci::vec2 center = getWindowCenter();
  ci::vec2 button_size(30, 10);

  // Draw standard game button
  ci::gl::color(ci::Color(1, 0, 0));
  ci::gl::drawSolidRect(ci::Rectf(center.x - 20,
                                     center.y - 40,
                                     center.x + 20,
                                     center.y - 20));
  PrintText("Standard",
           ci::Color(0, 0, 0),
            button_size,
           ci::vec2(center.x - 20, center.y - 40));

  // Draw time attack button
  ci::gl::color(ci::Color(0, 1, 0));
  ci::gl::drawSolidRect(ci::Rectf(center.x - 20,
                                     center.y - 10,
                                     center.x + 20,
                                     center.y + 10));
  PrintText("Time Attack",
            ci::Color(0, 0, 0),
             button_size,
            ci::vec2(center.x - 20, center.y - 10));

  // Draw time trial button
  ci::gl::color(ci::Color(0, 0, 1));
  ci::gl::drawSolidRect(ci::Rectf(center.x - 20,
                                     center.y + 20,
                                     center.x + 20,
                                     center.y + 40));
  PrintText("Time Trial",
            ci::Color(0, 0, 0),
             button_size,
            ci::vec2(center.x - 20, center.y + 20));
}

void MyApp::DrawGrid() const {
  int size = 24;
  int tile_size = 10;

  for (size_t row = 0; row < size; row++) {
    for (size_t col = 0; col < size; col++) {
      ci::gl::color(ci::Color(1, 0, 0));
      ci::gl::drawSolidRect(ci::Rectf(tile_size * row,
                                      tile_size * col,
                                      tile_size * (row + 1),
                                      tile_size * (col + 1)));
    }
  }
}

void MyApp::keyDown(KeyEvent event) { }

}  // namespace myapp
