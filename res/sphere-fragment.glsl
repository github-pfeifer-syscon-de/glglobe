
in vec2 UV;
in vec3 NORMAL;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

in vec3 LightDirection_tangentspace;
in vec3 EyeDirection_tangentspace;

out vec3 color;

uniform sampler2D dayTexture;
uniform sampler2D nightTexture;
uniform sampler2D NormalTextureSampler;
uniform sampler2D SpecularTextureSampler;
uniform sampler2D weatherSampler;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;
uniform vec3 LightPosition_worldspace;  // point-light
uniform float specLightPower;   // ~300 this needs to get scale to the light distance
uniform float diffuseLightPower;    // ~1000 this contributes to normal-map "height"
uniform float ambient;
uniform float twilight;         // let user tune twilight
uniform int debug;
uniform float specular_power;
uniform float weather_alpha;

void main() {
    vec3 LightColor = vec3(1.0,1.0,1.0);
    // doing shade calculation here leads to nicer modeling of twilight
    float shade = dot(normalize(LightPosition_worldspace), (NORMAL));   // left out normalize, as our values are already normalized

    float fact = 1.0/(2.0*twilight);
    float day;
    if (shade < -twilight) {    // complete night
       day = 0.0;
    }
    else if (shade > twilight) {    // complete day
       day = 1.0;
    }
    else {
        day = ((shade+twilight) * fact);
    }
    float night = (1.0 - day);
    vec4 valWeather = texture(weatherSampler, UV);
    vec4 valNight = texture(nightTexture, UV);
    vec4 valDay = texture(dayTexture, UV);
    vec3 MaterialDiffuseColor;
    if ((debug & 1) != 0) {
	MaterialDiffuseColor = vec3(0.6,0.6,0.6);
    }
    else {
	MaterialDiffuseColor = (night * valNight + day * valDay).rgb;
    }
    MaterialDiffuseColor = mix(MaterialDiffuseColor, valWeather.rgb, min(valWeather.a * weather_alpha, 1.0));
    vec3 MaterialAmbientColor = MaterialDiffuseColor * ambient;   // from 0.1
    vec3 MaterialSpecularColor = texture( SpecularTextureSampler, UV ).rgb  * 0.3;

    // Local normal, in tangent space. normal map is in TGA (not in DDS) for better quality
    vec3 TextureNormal = texture(NormalTextureSampler, UV).rgb;
    vec3 TextureNormal_tangentspace = normalize(TextureNormal*2.0 - 1.0);

    // Distance to the light
    float distance = length( LightPosition_worldspace - Position_worldspace );

    // Normal of the computed fragment, in camera space
    vec3 n = TextureNormal_tangentspace;
    // Direction of the light (from the fragment to the light)
    vec3 l = normalize(LightDirection_tangentspace);
    // Cosine of the angle between the normal and the light direction,
    // clamped above 0
    //  - light is at the vertical of the triangle -> 1
    //  - light is perpendicular to the triangle -> 0
    //  - light is behind the triangle -> 0
    float cosTheta = clamp(dot(n, l), 0.0, 1.0);

    // Eye vector (towards the camera)
    vec3 E = normalize(EyeDirection_tangentspace);
    // Direction in which the triangle reflects the light
    vec3 R = reflect(-l,n);
    // Cosine of the angle between the Eye vector and the Reflect vector,
    // clamped to 0
    //  - Looking into the reflection -> 1
    //  - Looking elsewhere -> < 1
    float cosAlpha = clamp(dot(E, R), 0.0, 1.0);

    color =
	    // Ambient : simulates indirect lighting
	    MaterialAmbientColor +
	    // Diffuse : "color" of the object
	    MaterialDiffuseColor * LightColor * diffuseLightPower * cosTheta / (distance*distance) +
	    // Specular : reflective highlight, like a mirror
	    MaterialSpecularColor * LightColor * specLightPower * pow(cosAlpha, specular_power) / (distance*distance);

}
