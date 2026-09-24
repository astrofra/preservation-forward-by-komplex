#ifndef FORWARD_SDL_VIEW_H
#define FORWARD_SDL_VIEW_H
#include <string>
#include <SDL.h>
#include "render/rgb_surface.h"

namespace forward_player {
class SdlView {
public:
    SdlView() : window_(NULL), renderer_(NULL), texture_(NULL) {}
    ~SdlView();
    bool open(std::string* error);
    bool present(const forward_offline::RgbSurface& frame, std::string* error);
    SDL_Window* window() const { return window_; }
private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
};
}
#endif
