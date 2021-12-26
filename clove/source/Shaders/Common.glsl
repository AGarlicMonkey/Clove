struct Light {
    vec3 direction;
    int shadowIndex;

	vec3 position;
	float constant;

	vec3 ambient;
	float linearV;

	vec3 diffuse;
	float quadratic;

	vec3 specular;
	float radius;

    int type;
    float padding_1;
    float padding_2;
    float padding_3;
};