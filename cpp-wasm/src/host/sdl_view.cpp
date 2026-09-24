#include "host/sdl_view.h"
#include "app/demo_runtime.h"
namespace forward_player {
SdlView::~SdlView() {
    if (texture_) SDL_DestroyTexture(texture_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
}
bool SdlView::open(std::string* error) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    Uint32 flags = 0;
#ifndef __EMSCRIPTEN__
    flags = SDL_WINDOW_RESIZABLE;
#endif
    // Browser CSS scales presentation only. A resizable SDL web window would
    // replace the backing dimensions with the CSS size (zero while hidden).
    window_ = SDL_CreateWindow("Forward / Komplex - 512x256", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, kWidth, kHeight, flags);
    if (window_) renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (window_ && !renderer_) renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    if (renderer_) texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGB888,
                                               SDL_TEXTUREACCESS_STREAMING, kWidth, kHeight);
    if (!texture_) { if (error) *error = SDL_GetError(); return false; }
    SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_NONE);
    SDL_RenderSetLogicalSize(renderer_, kWidth, kHeight);
    return true;
}
bool SdlView::present(const forward_offline::RgbSurface& frame, std::string* error) {
    if (SDL_UpdateTexture(texture_, NULL, frame.pixels().data(), kWidth * sizeof(std::uint32_t)) < 0 ||
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255) < 0 || SDL_RenderClear(renderer_) < 0 ||
        SDL_RenderCopy(renderer_, texture_, NULL, NULL) < 0) {
        if (error) *error = SDL_GetError();
        return false;
    }
    SDL_RenderPresent(renderer_);
    return true;
}
}
