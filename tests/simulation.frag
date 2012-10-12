
#define MAX_NUMBER_ITEMS 8
#define BACKGROUND_COLOR vec4(0, 0, 0.2, 1)
#define ZERO_COLOR vec4(0, 0, 0.5, 1)
#define POSITIVE_COLOR vec4(0, 1, 0, 1)
#define NEGATIVE_COLOR vec4(1, 0, 0, 1)
#define LINE_COLOR vec4(1, 1, 1, 1)
#define RANGE 0.05


#ifdef GL_ES
precision highp float;
uniform highp vec3 u_data[MAX_NUMBER_ITEMS];
uniform highp int u_number;
#else
uniform vec3 u_data[MAX_NUMBER_ITEMS];
uniform int u_number;
#endif

bool isClose(float value, float center, float radius)
{
    return (value > (center-radius) && value < (center+radius));
}

void main()
{    
    // We reset the potential, zero.
    float potential = 0.0;
    
    // here, we iterate the electric charges stored in u_data where
    // u_data.xy is the position vector.
    // u_data.z is the charge value in coulombs.
    for(int i = 0; i < MAX_NUMBER_ITEMS; ++i) {
        if(i < u_number)
            potential += u_data[i].z / distance(gl_FragCoord.xy, u_data[i].xy);
    }
    
    
    // now, in order to render the equipotentials,
    // we get the integer part and we compare it with the original potential.
    // if the integer part is close to the real value, we render with a different color
    // in this case white    
    if(isClose(potential, 0.0, RANGE))
        gl_FragColor = ZERO_COLOR;
    else if(isClose(potential, ceil(potential), RANGE))
        gl_FragColor = LINE_COLOR;
    
    else
    {
        potential = clamp(potential / 10.0, -1.0, 1.0);
        
        // If the potencial is positive we render in green
        if(potential > 0.0)
            gl_FragColor = mix(BACKGROUND_COLOR, POSITIVE_COLOR, potential);

        // If the potencial is negative we render in green
        else
            gl_FragColor = mix(BACKGROUND_COLOR, NEGATIVE_COLOR, -potential);
    }
}
