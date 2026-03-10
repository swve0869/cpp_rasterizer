#include <iostream>
#include <SDL3/SDL.H>
#include <raster.h>



void setPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    // Cast the void pointer to a 32-bit integer pointer
    Uint32* pixels = (Uint32*)surface->pixels;
    //x < 0 || y < 0 ||
    if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
    {
        return;
    }
    // Index = (y * pitch_in_pixels) + x
    // surface->pitch is in BYTES, so we divide by 4 to get pixels
    // pixels[(y * (surface->pitch / 4)) + x] = color;

    // render with x y origin in bottm left
    int index = ((((surface->h) -1) * (surface->pitch / 4)) - (y * (surface->pitch / 4)) + x);
    pixels[ index] = color;
    return;
}

void rasterBox(SDL_Surface* surface, int x, int y, int w, int h, Uint32 color) {
    // Cast the void pointer to a 32-bit integer pointer
    Uint32* pixels = (Uint32*)surface->pixels;
   
    for (int i = x; i < x + w; i++) {
        for (int j = y; j <= y + h; j++) {
            setPixel(surface, i, j, color);
        }
    }
}


point2d leftmost(point2d p1, point2d p2) {
    if (p1.x == p2.x) { return p1; }
    if (p1.x < p2.x)  { return p1; }
    if (p1.x > p2.x)  { return p2; }
}

point2d rightmost(point2d p1, point2d p2) {
    if (p1.x == p2.x) { return p1; }
    if (p1.x > p2.x) { return p1; }
    if (p1.x < p2.x) { return p2; }
}

point2d uppermost(point2d p1, point2d p2) {
    if (p1.y == p2.y) { return p1; }
    if (p1.y < p2.y) { return p2; }
    if (p1.y > p2.y) { return p1; }
}

point2d lowermost(point2d p1, point2d p2) {
    if (p1.y == p2.y) { return p1; }
    if (p1.y > p2.y) { return p2; }
    if (p1.y < p2.y) { return p1; }
}

//TODO raster polygon and line instead of just line
void raster_poly(SDL_Surface* surface, poly2d l, int thickness, Uint32 color) {

    float slope;
    point2d p1 = l.p1;
    point2d p2 = l.p2;


    // check for and raster flat or vertical lines
    if (p1.y == p2.y) { // flat
        p1 = leftmost(l.p1, l.p2);  p2 = rightmost(l.p1, l.p2);
        for (float x = p1.x; x <= p2.x; x += 1) {
            rasterBox(surface, (int)x, (int)p1.y, thickness, thickness, 0);
        }
        return;
    }
    if (p1.x == p2.x) { // vertical
        p1 = lowermost(l.p1, l.p2); p2 = uppermost(l.p1, l.p2);
        for (float y = p1.y; y <= p2.y; y += 1) {
            rasterBox(surface, (int)p1.x, (int)y, thickness, thickness, 0);
        }
        return;
    }
    
    //calculate slope
    slope = (p1.y - p2.y) / (p1.x - p2.x);
    p1 = leftmost(l.p1, l.p2);  p2 = rightmost(l.p1, l.p2);
    
    for (float x = p1.x; x <= p2.x; x += 1) {
        float y = slope * x;// +p1.y;
        //std::cout << (int)x << ":" << (int)y << "   slop: " << slope << std::endl;
        rasterBox(surface, (int)x, (int)y, thickness, thickness, 0);

    }
}


void raster_object(SDL_Surface* surface, object3d* o) {
    for (const auto& poly2d : o->polys2d) {

        raster_poly(surface, poly2d, 5, 0);
    }

}



int main(int argc, char* args[]) {
    bool isRunning = true;
    SDL_Event ev;
    float degree = 0;
    float tx = 0;
    float ty = 0;
    float tz = 0;
    float w =  2000;
    float h =  2000;
    SDL_Rect rect = { 0, 0, w, h};


    Uint32 screen_bckgrnd_color = (Uint32)COLOR_WHITE;



    // 1. Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Create the Window
    SDL_Window* window = SDL_CreateWindow(
        "SDL2 Static Window",          // Title
        w, h,                      // Width, Height
        SDL_WINDOW_RESIZABLE              // Flags
    );

    if (window == NULL) {
        printf("Window Error: %s\n", SDL_GetError());
        return 1;
    }

    // 3. Get the Window Surface (The "Canvas")
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    // 4. Fill the surface with a color (R, G, B)
    // MapRGB converts the 0-255 values into a format the surface understands
   SDL_FillSurfaceRect(screenSurface, &rect , screen_bckgrnd_color);

    // 5. Update the surface to show the change on the window
    SDL_UpdateWindowSurface(window);

    // 6. Simple Hack to keep the window open for 5 seconds
    // Otherwise, it would open and close instantly
    //SDL_Delay(5000);


    // The Main Loop
    while (isRunning) 
    {
        static bool rend = false;
        static float d = 200;

        // 1. Event Handling (The "Input" phase)
        // This inner loop empties the event queue every frame
        while (SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_EVENT_QUIT) {
                isRunning = false; // User clicked the [X]
            }
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE) {
                    isRunning = false; // User pressed Escape
                }
                if (ev.key.key == SDLK_UP) {
                    d += 10;
                    std::cout << "d: " << d << std::endl;
                    rend = false;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
                if (ev.key.key == SDLK_DOWN) {
                    d -= 10;
                    std::cout << "d: " << d << std::endl;
                    rend = false;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }

                if (ev.key.key == SDLK_A) {
                    if (SDL_MUSTLOCK(screenSurface)) {
                        SDL_LockSurface(screenSurface);
                    }
                    static int i = 0;
                    i++;
                    rasterBox(screenSurface, 200, 200, 50, 50, 230);

                    rasterBox(screenSurface, 500+i, 500,20,20, 23);


                    if (SDL_MUSTLOCK(screenSurface)) {
                        SDL_UnlockSurface(screenSurface);
                    }
                }

                if (ev.key.key == SDLK_R) {

                    rend = false;
                    degree = 1;
          
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
                if (ev.key.key == SDLK_H) {

                    rend = false;
                    tx += 100;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
                if (ev.key.key == SDLK_J) {

                    rend = false;
                    tx -= 100;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
                if (ev.key.key == SDLK_B) {

                    rend = false;
                    ty += 100;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
                if (ev.key.key == SDLK_N) {

                    rend = false;
                    ty -= 100;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
                if (ev.key.key == SDLK_U) {

                    rend = false;
                    tz += 100;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
                if (ev.key.key == SDLK_I) {

                    rend = false;
                    tz -= 100;
                    SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

                }
            }
            
        }

        // 2. Processing/Update (The "Logic" phase)
        // This is where you'd move players or calculate physics
        if (SDL_MUSTLOCK(screenSurface)) {
            SDL_LockSurface(screenSurface);
        }

  
        
        if (!rend) {


            //std::vector<point3d> cube_points = {
            //    // Top points (Y = 500)
            //    { 300.0f, 500.0f, 300.0f }, // p0: Top-Front-Left
            //    { 500.0f, 500.0f, 300.0f }, // p1: Top-Front-Right
            //    { 500.0f, 500.0f, 500.0f }, // p2: Top-Back-Right
            //    { 300.0f, 500.0f, 500.0f }, // p3: Top-Back-Left

            //    // Bottom points (Y = 300)
            //    { 300.0f, 300.0f, 300.0f }, // p4: Bottom-Front-Left
            //    { 500.0f, 300.0f, 300.0f }, // p5: Bottom-Front-Right
            //    { 500.0f, 300.0f, 500.0f }, // p6: Bottom-Back-Right
            //    { 300.0f, 300.0f, 500.0f }  // p7: Bottom-Back-Left
            //};


    

            point2d p1 = { 100,100 };  
            point2d p2 = { 200,100 };
            point2d p3 = { 200,200 };
            point2d p4 = { 100,200 };
            poly2d l1= create_poly2d(p1, p2);
            poly2d l2 = create_poly2d(p2, p3);
            poly2d l3 = create_poly2d(p3, p4);
            poly2d l4 = create_poly2d(p4, p1);

            //raster_poly(screenSurface, l2, 10, 100);
            //rasterLine(screenSurface, l2d2, 10, 100);
            /*rasterLine(screenSurface, l1, 10, 100);
            rasterLine(screenSurface, l2, 10, 100);
            rasterLine(screenSurface, l3, 10, 100);
            rasterLine(screenSurface, l4, 10, 100);*/

           //object cube = create_cube_from_points(cube_points);

            point3d p3d = { 0,0,0 };
            object3d cube = create_cube_from_center_point(p3d,100);
            
            //translate_object(&cube, 500, 500, 800);
            //rotate_object(&cube, 45, 0, 0);
            translate_object(&cube,tx,ty,tz);
            project_object(&cube,d);
            translate_object2d(&cube,w/2,h/2);
            //print_object3d_2d(&cube);
            print_object3d(&cube);
            raster_object(screenSurface,&cube);
            
            



            rend = true;
        }

        if (SDL_MUSTLOCK(screenSurface)) {
            SDL_UnlockSurface(screenSurface);
        }


        // 3. Rendering (The "Drawing" phase)
        // Clear the surface and redraw your rects here
       // SDL_FillSurfaceRect(screenSurface, &rect, 200);


        SDL_UpdateWindowSurface(window);
    }


    // 7. Cleanup
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

