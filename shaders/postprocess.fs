#version 450 core

out vec4 FragColor;

in vec2 tex_coord;
uniform sampler2D frame_color;

uniform vec2 uv_offset;
uniform vec2 uv_scale;
uniform vec2 screen_res;

struct Cross {
	float width;
	float height;
	vec4 color;
	bool enabled;
};
uniform Cross cross = Cross(0.001, 0.01, vec4(0.5,0.5,0.5,1), true);

// following NVIDIA's whitepaper on FXAA
float FxaaLuma(vec3 rgb) { return rgb.y * (0.587/0.299) + rgb.x; }

float rgb2luma(vec3 rgb){
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

#define FXAA_EDGE_THRESHOLD 1.0/8
#define FXAA_EDGE_THRESHOLD_MIN 1.0/16
#define FXAA_SUBPIX_TRIM 1.0/4
#define FXAA_SUBPIX_CAP 3.0/4
#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))
#define FXAA_SEARCH_STEPS 24

uniform bool fxaa_enabled;

vec4 fxaa_filter(vec2 uv) {
	// local contrast check
	vec4 color_center = texture(frame_color, uv);
	vec3 color_n = textureOffset(frame_color, uv, ivec2(0,-1)).rgb;
	vec3 color_s = textureOffset(frame_color, uv, ivec2(0,1)).rgb;
	vec3 color_w = textureOffset(frame_color, uv, ivec2(-1,0)).rgb;
	vec3 color_e = textureOffset(frame_color, uv, ivec2(1,0)).rgb;

	float luma_center = FxaaLuma(color_center.rgb);
	float luma_n = FxaaLuma(color_n.rgb);
	float luma_s = FxaaLuma(color_s.rgb);
	float luma_w = FxaaLuma(color_w.rgb);
	float luma_e = FxaaLuma(color_e.rgb);

	float luma_min = min(luma_center, min(luma_n, min(luma_s, min(luma_w, luma_e))));
	float luma_max = max(luma_center, max(luma_n, max(luma_s, max(luma_w, luma_e))));
	float contrast = luma_max - luma_min;

	if (contrast < max(FXAA_EDGE_THRESHOLD_MIN, luma_max * FXAA_EDGE_THRESHOLD)) {
		return color_center;
	}


	vec3 rgb_avg = color_center.rgb + color_n + color_s + color_w + color_e;
	vec3 color_nw = textureOffset(frame_color, uv, ivec2(-1,-1)).rgb;
	vec3 color_ne = textureOffset(frame_color, uv, ivec2(1,-1)).rgb;
	vec3 color_sw = textureOffset(frame_color, uv, ivec2(-1,1)).rgb;
	vec3 color_se = textureOffset(frame_color, uv, ivec2(1,1)).rgb;
	rgb_avg += color_nw + color_ne + color_sw + color_se;
	rgb_avg = rgb_avg * (1.0/9);

	
	// vertical or horizontal edge detection with sobel filter
	float luma_nw = FxaaLuma(color_nw);
	float luma_ne = FxaaLuma(color_ne);
	float luma_sw = FxaaLuma(color_sw);
	float luma_se = FxaaLuma(color_se);

	float edge_vertical =
		abs(0.25 * luma_nw - 0.5 * luma_n + 0.25 * luma_ne) +
		abs(0.25 * luma_w - 0.5 * luma_center + 0.25 * luma_e) +
		abs(0.25 * luma_sw - 0.5 * luma_s + 0.25 * luma_se);

	float edge_horizontal =
		abs(0.25 * luma_nw - 0.5 * luma_w + 0.25 * luma_sw) +
		abs(0.25 * luma_n - 0.5 * luma_center + 0.25 * luma_s) +
		abs(0.25 * luma_ne - 0.5 * luma_e + 0.25 * luma_se);

	bool is_horizontal = edge_horizontal >= edge_vertical;

	// end of edge search
	vec2 inv_screen_size = 1.0/textureSize(frame_color, 0);
	float step_length = is_horizontal ? inv_screen_size.y : inv_screen_size.x; 
	vec2 step = is_horizontal ? vec2(inv_screen_size.x, 0) : vec2(0, inv_screen_size.y); 
	
	float luma_1 = is_horizontal ? luma_s : luma_w;
	float luma_2 = is_horizontal ? luma_n : luma_e;

	float grad_1 = luma_1 - luma_center;
	float grad_2 = luma_2 - luma_center;

	bool is_1_steeper = abs(grad_1) >= abs(grad_2);

	float local_gradient = 0.25*max(abs(grad_1), abs(grad_2));
	float luma_local_avg;
	
	if (is_1_steeper) {
		step_length = -step_length;
		luma_local_avg = 0.5 * (luma_1 + luma_center);
	} else {
		luma_local_avg = 0.5 * (luma_2 + luma_center);
	}
	
	vec2 uv_p = uv + step;
	vec2 uv_m = uv - step;
	bool done_p = false;
	bool done_m = false;
	float luma_p = 0;
	float luma_m = 0;

	if(is_horizontal){
		uv_p.y += step_length * 0.5;
		uv_m.y += step_length * 0.5;
	} else {
		uv_p.x += step_length * 0.5;
		uv_m.x += step_length * 0.5;
	}

	for(uint i = 0; i < FXAA_SEARCH_STEPS; i++) {
		if (!done_p) {
			luma_p = FxaaLuma(texture(frame_color, uv_p).rgb);
		}
		if (!done_m) {
			luma_m = FxaaLuma(texture(frame_color, uv_m).rgb);
		}

		done_p = done_p || (abs(luma_p - luma_local_avg) >= local_gradient);
		done_m = done_m || (abs(luma_m - luma_local_avg) >= local_gradient);

		if (done_p && done_m) break;

		if (!done_p) {
			uv_p += step;
		}
		if (!done_m) {
			uv_m -= step;
		}
	}

	float distance1 = is_horizontal ? (uv_p.x - uv.x) : (uv_p.y - uv.y);
	float distance2 = is_horizontal ? (uv.x - uv_m.x) : (uv.y - uv_m.x);

	bool side_p_closer = distance1 < distance2;
	float distance_final = min(distance1, distance2);
	float edge_length = distance1 + distance2;
	float pixel_offset = - distance_final / edge_length + 0.5;

	bool is_luma_center_smaller = luma_center < luma_local_avg;
	bool correct_variation = ((side_p_closer ? luma_p : luma_m) < luma_local_avg) != is_luma_center_smaller;
	float final_offset = correct_variation ? pixel_offset : 0.0;


	// subpixel aliasing check
	// only average von neuman neighbourhood (nn)	
	float luma_nn = (luma_n + luma_s + luma_w + luma_e) * 0.25;
	float subpixel_contrast = abs(luma_nn - luma_center);
	float blend_nn = max(0.0, (subpixel_contrast / contrast) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
	blend_nn = min(blend_nn, FXAA_SUBPIX_CAP);

	final_offset = max(final_offset, blend_nn);


	// how much move towards edge
	vec2 final_uv = uv;
	if (is_horizontal) {
		final_uv.y += final_offset * step_length;
	} else {
		final_uv.x += final_offset * step_length;
	}

	return texture(frame_color, final_uv);
}

vec4 add_cross(vec4 color) {
	vec2 diff_from_center = tex_coord - vec2(0.5, 0.5);
	
	bool in_horizontal = (abs(diff_from_center.x) <= cross.height) &&  (abs(diff_from_center.y) <= cross.width * screen_res.x / screen_res.y);
	bool in_vertical = (abs(diff_from_center.x) <= cross.width) &&  (abs(diff_from_center.y) <= cross.height * screen_res.x / screen_res.y);

	if (in_horizontal || in_vertical) {
		return cross.color;
	} else {
		return color;
	}
}

void main()
{
	vec2 uv = tex_coord / uv_scale - uv_offset;
	bool inside_01 = (0 <= uv. x && uv.x <= 1) && (0 <= uv.y && uv.y <= 1);
	vec4 color = fxaa_enabled && inside_01 ? fxaa_filter(uv) : (inside_01 ? texture(frame_color, uv) : vec4(0));

	FragColor = cross.enabled ? add_cross(color) : color;
}