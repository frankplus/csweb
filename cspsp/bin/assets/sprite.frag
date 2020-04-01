/*******************************************************
 * 
 * Copyright (C) 2019 Francesco Pham (pham.francesco@gmail.com)
 * 
 *******************************************************/
precision mediump float;

varying vec2 TexCoords;
uniform sampler2D image;
uniform vec4 color;

void main()
{
    // read the fragment color from texture
    vec4 texColor = texture2D(image, TexCoords);

    gl_FragColor = texColor * color;
}