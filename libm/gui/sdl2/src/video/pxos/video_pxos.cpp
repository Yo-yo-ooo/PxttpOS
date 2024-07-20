#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"

#include "SDL_hints.h"

#include <libm/wmStuff/wmStuff.h>
#include "../../SDL_internal.h"
#include "../../../cstring/string.h"
#ifdef SDL_VIDEO_DRIVER_PXOS
#define SFP(x) SDL_PXOS_##x

#define PXOSVID_DRIVER_NAME       "PxttpOS Video Drive(SDL2)"

Window* win;

static void SFP(DestroyDevice)(SDL_VideoDevice *device)
{
    OS_free(device);
}

static int SFP(CreateSDLWindow)(_THIS, SDL_Window *window){
    win = requestWindow();
    if (win == NULL)
        return 0;
    OS_free(win->Title);
    win->Title = window->title;
    win->Dimensions.width = window->w;
    win->Dimensions.height = window->h;
    win->Resizeable = false;
    window->id = win->ID;
    setWindow(win);
}

static void SFP(SetWindowTitle)(_THIS, SDL_Window *window){
    //Window* win = requestWindow();
    if(window->id == win->ID){
        win->Title = window->title;
        setWindow(win);
    }else{return;}
}

static void SFP(SetWindowPosition)(_THIS,SDL_Window *window){
    if(window->id == win->ID){
        win->Dimensions.x = window->x;
        win->Dimensions.y = window->y;
        setWindow(win);
    }else{return;}
}

static void SFP(SetWindowSize)(_THIS,SDL_Window *window){
    if(window->id == win->ID){
        win->Dimensions.width = window->w;
        win->Dimensions.height = window->h;
        setWindow(win);
    }else{return;}
}

static void SFP(ShowWindow)(_THIS, SDL_Window * window){
    if(window->id == win->ID){
        win->Hidden = false;
        setWindow(win);
    }else{return;}
}
static void SFP(HideWindow)(_THIS, SDL_Window * window){
    if(window->id == win->ID){
        win->Hidden = true;
        setWindow(win);
    }else{return;}
}

static void SFP(DestroyWindow)(_THIS, SDL_Window * window){
    if(window->id == win->ID){
        if(window->is_destroying == SDL_FALSE)
            window->is_destroying = SDL_TRUE;
        deleteWindow(win->ID);
    }
}

/* Initialization/Query device */
static SDL_VideoDevice *PXOS_CreateDevice(void){

    SDL_VideoDevice *device;

    device = (SDL_VideoDevice *)OS_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        //SDL_OutOfMemory();
        return 0;
    }

    device->is_dummy = SDL_FALSE;
    device->name = PXOSVID_DRIVER_NAME;
    device->CreateSDLWindow = SDL_PXOS_CreateSDLWindow;
    device->SetWindowTitle = SDL_PXOS_SetWindowTitle;
    device->SetWindowPosition = SDL_PXOS_SetWindowPosition;
    device->SetWindowSize = SDL_PXOS_SetWindowSize;
    device->ShowWindow = SDL_PXOS_ShowWindow;
    device->HideWindow = SDL_PXOS_HideWindow;
    device->DestroyWindow = SDL_PXOS_DestroyWindow;
    device->free = SDL_PXOS_DestroyDevice;

    return device;
}

VideoBootStrap PXOS_bootstrap = {
    PXOSVID_DRIVER_NAME, "SDL dummy video driver",
    PXOS_CreateDevice,
    NULL /* no ShowMessageBox implementation */
};

#endif /* SDL_VIDEO_DRIVER_PXOS */