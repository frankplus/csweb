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

//the varying statement tells the shader pipeline that this variable
//has to be passed on to the next stage (so the fragment shader)
varying vec2 TexCoords;


//the shader entry point is the main method
void main()
{
    TexCoords = vertex.zw;
    gl_Position = model * vec4(vertex.xy, 0.0, 1.0); 
    
    // (texCoordX  * spriteWidth / textureWidth) + texSourceX
    TexCoords.x = (vertex.z * (spriteRect[2]/textureSize[0])) + spriteRect[0]/textureSize[0];
    // inverting v component
    TexCoords.y = ((1.0 - vertex.w) * (spriteRect[3]/textureSize[1])) + spriteRect[1]/textureSize[1];
    
}