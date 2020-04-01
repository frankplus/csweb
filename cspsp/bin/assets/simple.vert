/*******************************************************
 * 
 * Copyright (C) 2019 Francesco Pham (pham.francesco@gmail.com)
 * 
 *******************************************************/
attribute vec2 vertex;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex, 0.0, 1.0);     
}