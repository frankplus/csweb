//incoming values from the vertex shader stage.
//if the vertices of a primitive have different values, they are interpolated!

// setting float precision
precision mediump float;

varying vec2 TexCoords;
uniform sampler2D image;

void main()
{
    // read the fragment color from texture
    gl_FragColor = texture2D(image, TexCoords);
}