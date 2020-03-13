precision mediump float;

attribute vec2 vertex;

// array that contains information on
// sprite
// [0] -> spriteSourceX
// [1] -> spriteSourceY
// [2] -> spriteWidth
// [3] -> spriteHeight
uniform vec4 spriteRect;

// texture width and height
uniform vec2 textureSize;

uniform mat4 model; 
uniform mat4 projection;
uniform bvec2 flipped;

//the varying statement tells the shader pipeline that this variable
//has to be passed on to the next stage (so the fragment shader)
varying vec2 TexCoords;


//the shader entry point is the main method
void main()
{
    gl_Position = projection * model * vec4(vertex, 0.0, 1.0); 

    vec2 spriteSource = vec2(spriteRect[0], spriteRect[1]);
    vec2 spriteSize = vec2(spriteRect[2] - 1.0, spriteRect[3] - 1.0); // -1.0 to fix border problem

    vec2 v = vertex;
    if(flipped.x == true) v.x = 1.0 - v.x;
    if(flipped.y == true) v.y = 1.0 - v.y;
    TexCoords.x = (v.x * spriteSize.x + spriteSource.x) / textureSize.x;
    TexCoords.y = (v.y * spriteSize.y + spriteSource.y) / textureSize.y;
    
}