# cpp_rasterizer

First goal of my rasterizer was to render a cube and rotate it. The process is pretty generic but I really enjoyed working through it from scratch. The video below is a funny issue I had with rasterizing sloped lines. 
https://github.com/user-attachments/assets/f44b26d6-879a-48bc-b191-190861651d50


Here is the progress on the 2dpoly rasterizer. The fill function can take any order of three points and will find the orientation of the centroid vecor to polyp edge vector determinant. Then it fills the bounding box only if the cursr is within the polyps edges.
https://github.com/user-attachments/assets/465d2cb2-ea40-4ca4-929a-972aa704004d

