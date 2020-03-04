attribute vec4 vertex;

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
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); 
    
    // (texCoordX  * spriteWidth / textureWidth) + texSourceX
    vec2 v = vec2(vertex.z, vertex.w);
    if(flipped.x == true) v.x = 1.0 - v.x;
    if(flipped.y == true) v.y = 1.0 - v.y;
    TexCoords.x = (v.x * (spriteRect[2]/textureSize[0])) + spriteRect[0]/textureSize[0];
    TexCoords.y = (v.y * (spriteRect[3]/textureSize[1])) + spriteRect[1]/textureSize[1];
    
}