#include <iostream>
#include <SDL3/SDL.H>
#include <raster.h>



void setPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    // Cast the void pointer to a 32-bit integer pointer
    Uint32* pixels = (Uint32*)surface->pixels;
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

    if (l.line) {
        point2d p1 = l.p1;
        point2d p2 = l.p2;

        float step = thickness; // slope;//1; //std::abs(p1.x - p2.x) / thickness;
        float slope = (p2.y - p1.y) / (p2.x - p1.x);

        // TODO change raster to take into account surface/screen dinmensions to calculate near flat
        // and near vertical slopes. currently set to 1500 

        // check for and raster flat or vertical lines
        if (p1.y == p2.y || std::fabs(slope) < float(1.0 / 1500.0)) { // hor. or close to hor. depending on screen width
            p1 = leftmost(l.p1, l.p2);  p2 = rightmost(l.p1, l.p2);
            for (float x = p1.x; x <= p2.x; x += step) {
                rasterBox(surface, (int)x, (int)p1.y, thickness, thickness, 0);
            }
            return;
        }
        if (p1.x == p2.x || std::abs(slope) > 1500) { // vertical or close to vertical depending on screen height
            p1 = lowermost(l.p1, l.p2); p2 = uppermost(l.p1, l.p2);
            for (float y = p1.y; y <= p2.y; y += step) {
                rasterBox(surface, (int)p1.x, (int)y, thickness, thickness, 0);
            }
            return;
        }

        //calculate slope
        p1 = leftmost(l.p1, l.p2);  p2 = rightmost(l.p1, l.p2);

        //float slope = (p2.y - p1.y) / (p2.x - p1.x);
        float b = l.p1.y - (l.p1.x * slope);
        step = std::abs(thickness / slope);//1; //std::abs(p1.x - p2.x) / thickness;
        if (std::abs(slope) < 1) {
            step = std::abs(slope / thickness);
        }

        //printf("%.10f : %.10f -->  %.10f : %.10f step:%.10f  slope: %.10f\n", p1.x, p1.y, p2.x, p2.y,step,slope);


        for (float x = p1.x; x <= p2.x; x += step) {
            float y = slope * x + b;// +p1.y;
            //std::cout << (int)x << ":" << (int)y << "   slop: " << slope << std::endl;
            rasterBox(surface, (int)x, (int)y, thickness, thickness, 0);
        }

    }
    else { // rasterize poly
        rasterBox(surface, (int)l.p1.x, (int)l.p1.y, thickness, thickness, 0);
        rasterBox(surface, (int)l.p2.x, (int)l.p2.y, thickness, thickness, 0);
        rasterBox(surface, (int)l.p3.x, (int)l.p3.y, thickness, thickness, 0);

           
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
    float xdegree = 0;
    float ydegree = 0;
    float zdegree = 0;


    float tx = 0;
    float ty = 0;
    float tz = 200;
    float w =  1500;
    float h =  1500;
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

            SDL_Delay(16);

            if (ev.type == SDL_EVENT_QUIT) {
                isRunning = false; // User clicked the [X]
            }

            //int* keyboard_state = (int*)SDL_GetKeyboardState(NULL);


            if (ev.type == SDL_EVENT_KEY_DOWN) {
                if (ev.key.key == SDLK_ESCAPE) {
                    isRunning = false; // User pressed Escape
                }
                

                if (ev.key.key == SDLK_UP) {
                    d += 10;
                    std::cout << "d: " << d << std::endl;
                    rend = false;


                }
                if (ev.key.key == SDLK_DOWN) {
                    d -= 10;
                    std::cout << "d: " << d << std::endl;
                    rend = false;


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
                 
                    if (ev.key.mod == SDL_KMOD_LSHIFT)
                    {
                        xdegree -= 10;
                    }
                    else {
                        xdegree += 10;
                    }
                    rend = false;
                }
                if (ev.key.key == SDLK_H) {

                    rend = false;
                    tx += 100;


                }
                if (ev.key.key == SDLK_J) {

                    rend = false;
                    tx -= 100;


                }
                if (ev.key.key == SDLK_B) {

                    rend = false;
                    ty += 100;


                }
                if (ev.key.key == SDLK_N) {

                    rend = false;
                    ty -= 100;


                }
                if (ev.key.key == SDLK_U) {

                    rend = false;
                    tz += 100;


                }
                if (ev.key.key == SDLK_I) {

                    rend = false;
                    tz -= 100;


                }

                SDL_FillSurfaceRect(screenSurface, &rect, screen_bckgrnd_color);

            }
            
        }

        // 2. Processing/Update (The "Logic" phase)
        // This is where you'd move players or calculate physics
        if (SDL_MUSTLOCK(screenSurface)) {
            SDL_LockSurface(screenSurface);
        }

  
        
        if (!rend) {


    /*        point2d p1 = { 100,100 };  
            point2d p2 = { 200,100 };
            point2d p3 = { 200,200 };
            point2d p4 = { 100,200 };
            poly2d l1= create_poly2d(p1, p2);
            poly2d l2 = create_poly2d(p2, p3);
            poly2d l3 = create_poly2d(p3, p4);
            poly2d l4 = create_poly2d(p4, p1);*/


            //3 point poly test
            point3d p1 = { 0,0,0 };  point3d p2 = { 40,40,40 }; point3d p3 = { 0,40,0 };


            poly3d po = create_poly3d(p1, p2, p3);
            std::vector<poly3d> v = { po };
            object3d obj = object3d(v);

            translate_object(&obj, tx, ty, tz);
            //print_object3d(&obj);
            project_object(&obj, d);
            translate_object2d(&obj, w / 2, h / 2);
            print_object3d(&obj);
            print_object3d_2d(&obj);
            raster_object(screenSurface, &obj);


            
                
           //CUBE STUFF 
            point3d p3d = { 0,0,0 };
            object3d cube = create_cube_from_center_point(p3d, 100);
            //translate_object(&cube, 500, 500, 800);
            //project_object(&cube, d);
            //print_object3d(&cube);
            //print_object3d_2d(&cube);
            //printf("rotate: ");
            rotate_object(&cube, xdegree, xdegree, 0);
            //printf("translate3d: ");
            translate_object(&cube, tx, ty, tz);
            //printf("project: ");
            project_object(&cube, d);
            //printf("translate2d: ");
            translate_object2d(&cube, w / 2, h / 2);
            //printf("raster: ");

            //raster_object(screenSurface,&cube);
            //printf("end :");
            





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

