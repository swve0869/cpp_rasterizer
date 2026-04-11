
#include <SDL3/SDL.H>
#include <raster.h>
#include <obj_parser.h>


#include "macros.h"

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
void raster_poly(SDL_Surface* surface, const poly2d p) {

    if (p.line) {
        point2d p1 = p.p1;
        point2d p2 = p.p2;

        float step = p.style.thickness; // slope;//1; //std::abs(p1.x - p2.x) / p.style.thickness;
        float slope = (p2.y - p1.y) / (p2.x - p1.x);

        // TODO change raster to take into account surface/screen dinmensions to calculate near flat
        // and near vertical slopes. currently set to 1500 

        // check for and raster flat or vertical lines
        if (p1.y == p2.y || std::fabs(slope) < float(1.0 / 1500.0)) { // hor. or close to hor. depending on screen width
            p1 = leftmost(p.p1, p.p2);  p2 = rightmost(p.p1, p.p2);
            for (float x = p1.x; x <= p2.x; x += step) {
                rasterBox(surface, (int)x, (int)p1.y, p.style.thickness, p.style.thickness, p.style.color);
            }
            return;
        }
        if (p1.x == p2.x || std::abs(slope) > 1500) { // vertical or close to vertical depending on screen height
            p1 = lowermost(p.p1, p.p2); p2 = uppermost(p.p1, p.p2);
            for (float y = p1.y; y <= p2.y; y += step) {
                rasterBox(surface, (int)p1.x, (int)y, p.style.thickness, p.style.thickness, p.style.color);
            }
            return;
        }

        //calculate slope
        p1 = leftmost(p.p1, p.p2);  p2 = rightmost(p.p1, p.p2);

        //float slope = (p2.y - p1.y) / (p2.x - p1.x);
        float b = p.p1.y - (p.p1.x * slope);
        step = std::abs(p.style.thickness / slope);//1; //std::abs(p1.x - p2.x) / p.style.thickness;
        if (std::abs(slope) < 1) {
            step = std::abs(slope / p.style.thickness);
        }

        //printf("%.10f : %.10f -->  %.10f : %.10f step:%.10f  slope: %.10f/n", p1.x, p1.y, p2.x, p2.y,step,slope);


        for (float x = p1.x; x <= p2.x; x += step) {
            float y = slope * x + b;// +p1.y;
            //std::cout << (int)x << ":" << (int)y << "   slop: " << slope << std::endl;
            rasterBox(surface, (int)x, (int)y, p.style.thickness, p.style.thickness, p.style.color);
        }

    }
    else { // rasterize polyp

        point2d p1 = p.p1; point2d p2 = p.p2; point2d p3 = p.p3;
        
        //find bounding box 
        float Ax = leftmost(p1, leftmost(p2, p3)).x;
        float Ay = lowermost(p1, lowermost(p2, p3)).y;
        float w = rightmost(p1, rightmost(p2, p3)).x - Ax;
        float h = uppermost(p1, uppermost(p2, p3)).y - Ay;
      

        // find determinant orientation with respect to center
        bool detpos = false;
        point2d center = { ((p1.x + p2.x + p3.x) / 3.0f) , (p1.y + p2.y + p3.y) / 3.0f  };
        setPixel(surface, (int)center.x, (int)center.y, p.style.color);
        if (det(p2 - p1, center - p1) > 0) {   
            detpos = true;
        }
        // move through box
        for (float x = Ax; x < Ax+w; x+=1 ) {
            for (float y = Ay; y < Ay+h; y += 1) {
                //rasterBox(surface, (int)x, y, 1, 1, COLOR_MAGENTA);
                
                point2d c = { x,y };
               
                float det12 = det(p2 - p1, c - p1);
                float det23 = det(p3 - p2, c - p2);
                float det31 = det(p1 - p3, c - p3);

                // if point inside render (greater then det or less than det) 
                if (det12 > 0 && det23> 0 && det31 > 0 
                    && detpos || 
                    det12 < 0 && det23 < 0 && det31 < 0
                    && !detpos) {
                    
                    //TODO remove rand color logic

                    
                    setPixel(surface, (int)x, (int)y, p.style.color);
                   


                    //rasterBox(surface, (int)x, (int)y, p.style.thickness, p.style.thickness, p.style.color);

                }
                // Possible OPT (reduces unecessary loops)
                // if point outside maybe snap to closest edge?
                    
            }
        }       
    }
}




void raster_object(SDL_Surface* surface, object3d* o, view v) {
    // move object to to view space // TODO camera translation matrix
    

    // translate the 3d obj to a cam view // TODO Not great copies entire 3d poly vector to 
    cam_translate_object(o, v.x, v.y, v.z);

    //print_object3d(&obj);
    // project object at distance // TODO change this projection to be part of camera translation matrix
    project_object(o, v.dist);
    // take 2d polys and translate to screen surface coordinates
    translate_object2d(o, v.rastercontext.w / 2, v.rastercontext.h / 2);
    print_object3d(o);
    //print_object3d_2d(o);


    for (auto& poly2d : o->polys2d) {

        //rand color for poly logic
        poly2d.style.color = (Uint32)std::rand();
        raster_poly(surface, poly2d);
    }

}



int main(int argc, char* args[]) {
    bool isRunning = true;
    SDL_Event ev;
    float xdegree = 0;
    float ydegree = 0;
    float zdegree = 0;
    
    const float deg = 10; //standard degree for rotate functions


    float tx = 0;
    float ty = 0;
    float tz = 200;
    float w =  800;
    float h =  800;
    rastercontext rcontext = { w,h };
    
   

    SDL_Rect rect = { 0, 0, w, h};


    Uint32 screen_bckgrnd_color = (Uint32)COLOR_WHITE;


    // TEAPOT
    object3d* tpot = new object3d;
    if (!parse_obj_file("C:/Users/swami.velamala/source/repos/sdl/sdl/files/teapot.obj", tpot));
    scale_object(tpot, 50);

    // CUBE
    point3d p3d = { 0,0,0 };
    object3d cube = create_cube_from_center_point(p3d, 100);

    // simple polyp
    point3d p1 = { 0,0,0 };  point3d p2 = { 40,40,40 }; point3d p3 = { 0,40,0 };
    poly3d po = create_poly3d(p1, p2, p3);
    std::vector<poly3d> v = { po };
    object3d* polyp = new object3d(v);


    object3d* arr[] = { polyp,&cube,tpot };
    int ind = 2;
    object3d* curr_obj = arr[ind];



    // dodecahderon
    //parse_obj_file("C:/Users/swami.velamala/source/repos/sdl/sdl/files/test.obj", &tpot);

    // 1. Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Error: %s/n", SDL_GetError());
        return 1;
    }

    // 2. Create the Window
    SDL_Window* window = SDL_CreateWindow(
        "SDL2 Static Window",          // Title
        w, h,                      // Width, Height
        SDL_WINDOW_RESIZABLE              // Flags
    );

    if (window == NULL) {
        printf("Window Error: %s/n", SDL_GetError());
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
        view cur_view = { tx,ty,tz,d,rcontext };

        curr_obj = arr[ind];
        

        // 1. Event Handling (The "Input" phase)
        // This inner loop empties the event queue every frame
        while (SDL_PollEvent(&ev) != 0) {

            //qSDL_Delay(16);

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
                        auto start = std::chrono::high_resolution_clock::now();
                        rotate_object(curr_obj, deg, 0, 0);
                        auto end = std::chrono::high_resolution_clock::now();
                        auto t = end - start;
                        
                        std::chrono::duration<double, std::milli> duration_ms = end - start;
                        auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


                        printf("elapsed time :%f\n", duration_ms);
                        //std::cout << "Sum: " << sum << "\n";
                        //std::cout << "Elapsed time: " << duration_ms.count() << " ms\n";
                        //std::cout << "Elapsed time: " << duration_us.count() << " us\n";*/

                    }
                    else {
                        rotate_object(curr_obj, -deg, 0, 0);
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
                if (ev.key.key == SDLK_Q) {

                    printf("ind: %d \n", ind);
                    if (ind < 2) {
                        ind += 1;
                    }
                    else {
                        ind = 0;
                    }
                       


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
           /* point3d p1 = { 0,0,0 };  point3d p2 = { 40,40,40 }; point3d p3 = { 0,40,0 };


            poly3d po = create_poly3d(p1, p2, p3);
            std::vector<poly3d> v = { po };
            object3d obj = object3d(v);

            rotate_object(&obj, 0, xdegree, 0);*/
            //raster_object(screenSurface, &obj, cur_view);

            // axis stuff TODO fix it all
            //// xaxis
            //point3d p1x_axis = { -1000000.0f, 0.0f, 0.00000f };
            //point3d p2x_axis = {  1000000.0f, 0.0f, 0.00000f };

            //po = create_poly3d(p1x_axis, p2x_axis);
            //v.clear(); v.push_back(po);
            //object3d xaxis = object3d(v);
            //rotate_object(&xaxis, xdegree, xdegree, 0);
            //translate_object(&xaxis, tx, ty, tz);
            //project_object(&xaxis, d);
            //translate_object2d(&xaxis, w / 2, h / 2);
            //raster_object(screenSurface, &xaxis);

            //// yaxis
            //point3d p1y_axis = { 0.00000f, -1000000.0f, 0.00000f };
            //point3d p2y_axis = { 0.00000f, 1000000.0f, 0.00000f };

            //po = create_poly3d(p1y_axis, p2y_axis);
            //v.clear(); v.push_back(po);
            //object3d yaxis = object3d(v);
            //rotate_object(&yaxis, xdegree, xdegree, 0);
            //translate_object(&yaxis, tx, ty, tz);
            //project_object(&yaxis, d);
            //translate_object2d(&yaxis, w / 2, h / 2);
            //raster_object(screenSurface, &yaxis);

            //// zaxis
            //point3d p1z_axis = { 0.00000f, 0.00000f, -1000000.0f };
            //point3d p2z_axis = { 0.00000f, 0.00000f, 1000000.0f };

            //po = create_poly3d(p1z_axis, p2z_axis);
            //v.clear(); v.push_back(po);
            //object3d zaxis = object3d(v);
            //rotate_object(&zaxis, xdegree, xdegree, 0);
            //translate_object(&zaxis, tx, ty, tz);
            //project_object(&zaxis, d);
            //translate_object2d(&zaxis, w / 2, h / 2);
            //raster_object(screenSurface, &zaxis);}


            
                
            //CUBE STUFF 
           // point3d p3d = { 0,0,0 };
            //object3d cube = create_cube_from_center_point(p3d, 100);
            //translate_object(&cube, 500, 500, 800);
            //project_object(&cube, d);
            //print_object3d(&cube);
            //print_object3d_2d(&cube);
            //printf("rotate: ");
            //rotate_object(&cube, xdegree, 0, 0);
            //printf("translate3d: ");
            //translate_object(&cube, tx, ty, tz);
            //printf("project: ");
            //project_object(&cube, d);
            //printf("translate2d: ");
            //translate_object2d(&cube, w / 2, h / 2);
            //printf("raster: ");

            //raster_object(screenSurface,&cube, cur_view);
            //printf("end :");
            


            // teapot
            
            //rotate_object(&tpot, xdegree, xdegree, 0);
            raster_object(screenSurface,curr_obj, cur_view);


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

