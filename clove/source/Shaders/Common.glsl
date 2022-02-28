struct Light {
    vec3 direction;
    int shadowIndex;

	vec3 position;
	float radius;

	vec3 ambient;
    int type;

	vec3 diffuse;
	float padding_1;

	vec3 specular;
	float padding_2;
};