#include "include.cpp"
int mode = 0;
int num_modes = 4;

real interpolate(real t, real a, real b) {
    if (mode == 0) {
        // linear interpolation (lerp)
        return LERP(t, a, b); // a + (t) * (b - a)
    } else if (mode == 1) {
        // smoothstep
        return LERP(3 * pow(t, 2) - 2 * pow(t, 3), a, b);
    } else if (mode == 2) {
        // smootherstep
        return LERP(6 * pow(t, 5) - 15 * pow(t, 4) + 10 * pow(t, 3), a, b);
    } else {
        // cosine
        return LINEAR_REMAP(cos(PI * t), 1.0, -1.0, a, b);
    }
}

vec3 cubic_LERP(real t, vec3 a, vec3 b, vec3 c, vec3 d) {
    return LERP(t, LERP(t, LERP(t, a, b), LERP(t, b, c)), LERP(t, LERP(t, b, c), LERP(t, c, d)));
}

vec3 vector_interpolate(real t, vec3 a, vec3 b) {
    real new_x = interpolate(t, a[0], b[0]);
    real new_y = interpolate(t, a[1], b[1]);
    real new_z = interpolate(t, a[2], b[2]);
    return V3(new_x, new_y, new_z);
}

void draw_world(mat4 PV, real ground, real lim_far) {
    vec3 periwinkle = V3(81.0, 143.0, 224.0) / 255.0;
    vec3 pale = V3(240.0, 237.0, 163.0) / 255.0;
    vec3 pale_blue = V3(131.0, 179.0, 235.0) / 255.0;
    vec3 deep_blue = V3(5.0, 102.0, 255.0) / 255.0;
    vec3 orange = V3(209.0, 118.0, 0.0) / 255.0;
    vec3 dirt = V3(141, 158, 84) / 255.0;
    vec3 darker_dirt = V3(158, 89, 36) / 255.0;

    vec3 sky_colors[4] = {pale_blue, deep_blue, pale, orange};
    vec3 earth_colors[4] = { dirt, darker_dirt, darker_dirt, dirt};

    { // little patch of grass
        double r = 100;
        vec3 verts[] = { { -30, 0, -30 }, { -30, 0,  30 }, {  30, 0,  30 }, {  30, 0, -30 } };
        soup_draw(PV * M4_Translation(0.0, ground, 0.0), SOUP_QUADS, 4, verts, earth_colors, V3(104.0 / 255.0, 0.0, 25.0 / 255.0));

        vec3 v_lines[60] = {};
        for (int i = 0; i < 30; ++i) {
            real time = (real) i / 29;
            v_lines[i * 2] = V3(30, 0, LERP(time, -30, 30));
            v_lines[i * 2 + 1] = V3(-30, 0, LERP(time, -30, 30));
        }
        //soup_draw(PV * M4_Translation(0.0, ground, 0.0), SOUP_LINES, 60, v_lines, NULL, V3(104.0 / 255.0, 0.0, 25.0 / 255.0));
    }

    { // end of the world
        //double r = 100;
        vec3 verts[] = { { -30, 0, -12 }, { -30, 0,  12 }, {  30, 0,  12 }, {  30, 0, -12 } };
        soup_draw(PV * (M4_Translation(0.0, ground, lim_far) * M4_RotationAboutXAxis(RAD(90))), SOUP_QUADS, 4, verts, sky_colors, periwinkle);
    }
    // { // outer box
    //     vec3 vertex_colors[] = { // FORNOW
    //         .5 * monokai.black,  .5 * monokai.black,  .5 * monokai.blue,   .5 * monokai.purple,
    //         .5 * monokai.orange, .5 * monokai.yellow, .5 * monokai.black,  .5 * monokai.black,
    //         .5 * monokai.black,  .5 * monokai.black,  .5 * monokai.black,  .5 * monokai.black,
    //         .5 * monokai.brown,  .5 * monokai.gray,   .5 * monokai.purple, .5 * monokai.orange,
    //         .5 * monokai.black,  .5 * monokai.purple, .5 * monokai.orange, .5 * monokai.black,
    //         .5 * monokai.black,  .5 * monokai.yellow, .5 * monokai.blue,   .5 * monokai.black,
    //     };
    //     Soup3D outer_box = library.soups.box;
    //     outer_box.vertex_colors = vertex_colors;
    //     outer_box.primitive = SOUP_QUADS;
    //     outer_box.draw(PV * M4_Translation(0, 1980.0, 0) * M4_Scaling(2000));
    // }
}

void breathe(real *time, real *_time_sign, real speed, real exhale) {
    *time += *_time_sign * speed;
    if (*time > exhale) {
        *_time_sign *= -1;
        *time = exhale * 2 - *time;
    } else if (*time < 0.0) {
        *_time_sign *= -1;
        *time = -*time;
    }
}

IndexedTriangleMesh3D ant_mesh() {
    IndexedTriangleMesh3D ant = {};
    
    IndexedTriangleMesh3D head = library.meshes.sphere;
    IndexedTriangleMesh3D torso = library.meshes.sphere;
    IndexedTriangleMesh3D tail = library.meshes.sphere;
    IndexedTriangleMesh3D leg = library.meshes.cylinder;
    return ant;
}

void ants() {
    mode = 2;
    
    Camera3D camera = { 30.0, RAD(0), 0, RAD(-20), 0, 0};
    real ground = -5.0;
    real play_time = 0.0;
    real time_for_more = 30.0;
    real speed_time = 0.0;
    real time_to_speed_up = 25.0;

    // jellyfish --------------------------------------------------
    int num = 10;
    int num_double = num * 2;
    int layers = 10;
    int all = layers * num_double;
    real size = 10.0;
    
    real peak = 0.0;
    real base = -3.5;

    real radius_outer = 2.8;
    real radius_inner = 1.3;
    real angle = 2 * PI / num;

    vec3 orig_head = V3(0.0, peak, 0.0);
    vec3 orig_base = V3(0.0, ground, 0.0);
    vec3 head = orig_head;
    vec3 test_head = head;
    vec3 grounded = orig_base;
    vec3 orig_grounded_base[10];
    vec3 grounded_base[10];
    for (int i = 0; i < num; ++i) {
        orig_grounded_base[i] = V3(cos(angle * i) * 0.8, ground, sin(angle * i) * 0.8);
        grounded_base[i] = orig_grounded_base[i];
    }
    real max_rotate = RAD(40);
    vec3 *orig = (vec3 *) calloc(all, sizeof(vec3));
    vec3 *drawn = (vec3 *) calloc(all, sizeof(vec3));

    vec2 *times = (vec2 *) calloc(layers, sizeof(vec2));
    real *rotates = (real *) calloc(layers, sizeof(real));
    vec2 *times_rotate = (vec2 *) calloc(layers, sizeof(vec2));
    vec2 *radii = (vec2 *) calloc(layers, sizeof(vec2));
    real *heights = (real *) calloc(layers, sizeof(real));
    mat4 *scale = (mat4 *) calloc(layers, sizeof(mat4));

    for (int j = 0; j < layers; ++j) {
        real time = (real) j / layers;
        times[j][0] = 1 - time;
        times[j][1] = 1.0;

        times_rotate[j][0] = 1 - time * 2.0;
        times_rotate[j][1] = 1.0;
        rotates[j] = interpolate(times_rotate[j][0], times[j][1] * max_rotate, 0.0);

        real orig_rad = LERP(time, radius_inner, radius_outer);
        radii[j][0] = orig_rad;
        radii[j][1] = LERP(time, orig_rad, orig_rad* 0.65);
        heights[j] = LERP(time, peak, base);

        real new_rad = radii[j][0] + interpolate(time, 0.0, 3.0) * time;
        real half_rad = radii[j][1] + interpolate(time, 0.0, 3.0) * time;

        scale[j] =  M4_Scaling(LERP(time, 0.5, 0.9) * V3(0.6, 0.05, 0.3));

        for (int i = 0; i < num; ++i) {
            orig[j * num_double + 2 * i] = V3(cos(angle * i) * new_rad, heights[j], sin(angle * i) * new_rad);
            orig[j * num_double + 2 * i + 1] = V3(cos(angle * (i + 0.5)) * half_rad, heights[j], sin(angle * (i + 0.5)) * half_rad);
        }
    }

    // for (int j = 0; j < layers / 2; ++j) {
    //     real time_rotate = (real) j / (layers / 2 - 1);
    //     // times_rotate[j][0] = 1 - time_rotate;
    //     times_rotate[j][1] = 1.0;
    //     // times_rotate[layers - 1 - j] = times_rotate[j];
    //     times_rotate[j + layers / 2][1] = 1.0;
    //     //rotates[layers - 1 - j] = interpolate(times_rotate[j][0], times[j][1] * max_rotate, 0.0);
    // }

    mat4 movement = globals.Identity;
    mat4 test_movement = globals.Identity;
    mat4 to_move = globals.Identity;
    real jelly_move_speed = 0.05;
    real ant_move_speed = 0.05;
    real real_move_speed = 0.0;

    real speed = 65.0 / 5000.0;
    mat4 shift_little = M4_RotationAboutYAxis(angle / 2.0);
    bool playing = true;
    IndexedTriangleMesh3D jelly_node = library.meshes.sphere;
    vec3 ivory = V3(247.0 / 255.0, 218.0 / 255.0, 201.0 / 255.0);
    vec3 gold = 1 / 255.0 * V3(240.0, 217.0, 57.0);
    vec3 blood = V3(194.0, 6.0, 6.0) / 255.0;
    IndexedTriangleMesh3D jelly_bit = library.meshes.cylinder;
    IndexedTriangleMesh3D jelly_head = library.meshes.sphere;
    mat4 M_head = M4_Scaling(V3(0.8, 1.2, 0.8));
    mat4 slow_twirl = globals.Identity;
    
    // jellyfish ---------------------------------------------------

    // ants --------------------------------------------------------
    // TODO: ants can stop to confer with one another, 
    // shader,
    // ants have notion of forward
    int num_ants = 5;
    int max_ants = 50;
    vec3 *ants_spawn = (vec3 *) calloc(max_ants, sizeof(vec3));
    vec3 *ants = (vec3 *) calloc(max_ants, sizeof(vec3));
    vec3 *ants_crawl = (vec3 *) calloc(max_ants, sizeof(vec3)); // forward directions
    real *ants_turn = (real *) calloc(max_ants, sizeof(real)); // skew, curve
    mat4 *ants_crawled = (mat4 *) calloc(max_ants, sizeof(mat4)); // place ants
    real *ants_countdown = (real *) calloc(max_ants, sizeof(real)); // countdown to change dir
    real *ants_countdown_turn = (real *) calloc(max_ants, sizeof(real)); // coundown to change skew
    real *ants_speed = (real *) calloc(max_ants, sizeof(real));
    real grab_range = 1.0;

    vec3 *ants_lost_spawn = (vec3 *) calloc(max_ants, sizeof(vec3));
    vec3 *ants_lost = (vec3 *) calloc(max_ants, sizeof(vec3));
    vec3 *ants_lost_crawl = (vec3 *) calloc(max_ants, sizeof(vec3)); 
    mat4 *ants_lost_crawled = (mat4 *) calloc(max_ants, sizeof(mat4));
    real *ants_lost_countdown = (real *) calloc(max_ants, sizeof(real));
    real *ants_lost_speed = (real *) calloc(max_ants, sizeof(real));
    IndexedTriangleMesh3D ant_bit = library.meshes.sphere;

    for (int i = 0; i < max_ants; ++i) { // spawn ants
        ants_spawn[i] = V3(random_real(-10.0, 10.0), ground, random_real(30.0, 50.0));
        ants[i] = ants_spawn[i];
        ants_crawl[i] = normalized(V3(random_real(-2.0, 2.0), 0.0, random_real(-1.0, -2.0)));
        ants_speed[i] = random_real(ant_move_speed * 0.45, ant_move_speed * 0.86);
        ants_turn[i] = random_real(RAD(-60), RAD(60));
        ants_crawled[i] =  M4_Translation(transformVector(M4_RotationAboutYAxis(ants_turn[i]), ants_crawl[i] * ants_speed[i]));
        ants_countdown[i] = random_real(1.0, 3.0);
        ants_countdown_turn[i] = random_real(0.5, 2.5);
    }

    real lim_far = -30.0;
    real lim_side = 25.0;
    vec3 lim[2] = {V3(-lim_side - 5.0, ground, lim_far), V3(lim_side + 5.0, ground, lim_far)};

    real lim_lost_height = 15.0;
    real lim_lost_side = 30.0;
    vec3 screen_bounds[4] = {V3(lim_lost_side, lim_lost_height, 0.0), 
                            V3(-lim_lost_side, lim_lost_height, 0.0), 
                            V3(-lim_lost_side, -lim_lost_height, 0.0),
                            V3(lim_lost_side, -lim_lost_height, 0.0)};
    // ants --------------------------------------------------------
    int points = 0;
    int lost = 0;
    bool game_over = false;

    // shader stuff ------------------------------------------------------------
    Shader shader; {
        char *vertex_shader_source = R""(
            #version 330 core
            layout (location = 0) in vec3 _p_model;
            layout (location = 1) in vec3 _n_model;
            uniform mat4 P, V, M;

            out vec3 p_world;
            out vec3 _n_world;

            void main() {
                p_world = (M * vec4(_p_model, 1.0)).xyz;
                _n_world = mat3(transpose(inverse(M))) * _n_model;
                gl_Position = P * V * vec4(p_world, 1.0);
            }
        )"";

        char *fragment_shader_source = R""(
            #version 330 core

            uniform vec3 o_camera_world; // camera position
            uniform int  num_lights;
            uniform vec3 light_positions_world[3];
            uniform vec3 light_colors[3];
            uniform float ambientStrength;
            uniform float diffuseStrength;
            uniform float specularStrength;
            uniform float shininess; 
            uniform bool atten;
            uniform float l_const;
            uniform float l_lin;
            uniform float l_quad;
            uniform vec3 orig_color;

            in vec3 p_world; // fragment position
            in vec3 _n_world;
            in vec3 v_color;

            out vec4 fragColor;

            float attenuate(float dist, float l_const, float l_lin, float l_quad) {
                return 1 / (l_const + l_lin * dist + l_quad * dist * dist);
            }

            void main() {
                vec3 n_world = normalize(_n_world); // fragment normal

                vec3 color = orig_color * ambientStrength;
    
                float attenuate = 1.0;

                for (int i = 0; i < num_lights; ++i) {
                    if (atten) {
                        attenuate = attenuate(length(light_positions_world[i] - p_world), l_const, l_lin, l_quad);
                    }

                    // TODO add diffuse  contribution
                    vec3 lightDir = normalize(light_positions_world[i] - p_world);
                    float diff = max(dot(n_world, lightDir), 0.0);
                    color += diff * light_colors[i] * diffuseStrength * attenuate;

                    // TODO add specular contribution
                    vec3 viewDir = normalize(o_camera_world - p_world);
                    vec3 reflectDir = reflect(-lightDir, n_world);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                    color += spec * light_colors[i] * specularStrength * attenuate;
                }

                fragColor = vec4(color, 1);
            }
        )"";

        shader = shader_create(vertex_shader_source, 2, fragment_shader_source);
    }
    real jelly_ambientStrength = 0.75;
    real jelly_diffuseStrength = 0.1;
    real jelly_specularStrength = 0.8;
    real jelly_shininess = 68.0;
    real l_const = 1.0;
    real l_lin = 0.09;
    real l_quad = 0.032;

    vec3 pale = V3(240.0, 237.0, 163.0) / 255.0;
    vec3 orange = V3(209.0, 118.0, 0.0) / 255.0;
    vec3 periwinkle = V3(81.0, 143.0, 224.0) / 255.0;
    vec3 pale_blue = V3(131.0, 179.0, 235.0) / 255.0;
    vec3 deep_blue = V3(5.0, 102.0, 255.0) / 255.0;
    vec3 dirt = V3(209, 137, 65) / 255.0;
    int num_lights = 3;
    vec3 pale_light_pos = V3(head[0] - 5.0, 0.5, 40.0);
    vec3 orange_light_pos = V3(head[0] + 5.0, 0.5, 40.0);
    vec3 blue_light_pos = V3(0.0, 0.0, 40.0);
    vec3 light_positions_world[3] = {pale_light_pos, orange_light_pos, blue_light_pos};
    vec3 light_colors[3] = { pale, orange, deep_blue };
    
    // shader stuff ------------------------------------------------------------

    // scene stuff --------------------------------------------------
    vec3 colors[4] = {pale_blue, deep_blue, pale, orange};
    // ground
    mat4 M_ground = M4_Translation(0.0, ground - 0.7, 0.0) * M4_Scaling(V3(100.0, 0.5, 100.0));
    IndexedTriangleMesh3D ground_mesh = library.meshes.sphere;
    // scene stuff ---------------------------------------------------

    while (cow_begin_frame()) {
        mat4 PV = camera_get_PV(&camera);
        mat4 P = camera_get_P(&camera);
        mat4 V = camera_get_V(&camera);
        mat4 M = globals.Identity;
        //camera_move(&camera);

        // gui_readout("time", &times[0][0]);
        //gui_checkbox("breathe" , &playing, 'b');
        gui_printf("Don't let the ants get away!");
        gui_printf("Press SPACE to keep them home!");
        gui_printf("Use wasd for movement");
        gui_readout("points", &points);
        // gui_readout("play_time", &play_time);
        // gui_readout("time_for_more", &time_for_more);
        // gui_readout("num_ants", &num_ants);
        gui_readout("lost", &lost);
        play_time += 0.0167;
        speed_time += 0.0167;

        if (lost == max_ants) {
            game_over = true;
            if (gui_button("restart", 'r')) { // reset
                lost = 0;
                num_ants = 5;
                movement = globals.Identity;
                test_movement = globals.Identity;
                to_move = globals.Identity;
                jelly_move_speed = 0.05;
                ant_move_speed = 0.05;
                real_move_speed = 0.0;

                for (int i = 0; i < max_ants; ++i) { // spawn ants
                    ants_spawn[i] = V3(random_real(-10.0, 10.0), ground, random_real(30.0, 50.0));
                    ants[i] = ants_spawn[i];
                    ants_crawl[i] = normalized(V3(random_real(-2.0, 2.0), 0.0, random_real(-1.0, -2.0)));
                    ants_speed[i] = random_real(ant_move_speed * 0.45, ant_move_speed * 0.86);
                    ants_turn[i] = random_real(RAD(-60), RAD(60));
                    ants_crawled[i] =  M4_Translation(transformVector(M4_RotationAboutYAxis(ants_turn[i]), ants_crawl[i] * ants_speed[i]));
                    ants_countdown[i] = random_real(1.0, 3.0);
                    ants_countdown_turn[i] = random_real(0.5, 2.5);
                }
            }
        } else {
            { // randomize time
                if (play_time > time_for_more) {
                    num_ants++;
                    time_for_more -= 1.0;
                    play_time = 0.0;
                }

                if (speed_time > time_to_speed_up) {
                    jelly_move_speed *= 1.45;
                    ant_move_speed *= 1.2;
                    speed_time = 0.0;
                }
            }

            if (playing || gui_button("step", '.')) { // update time
                for (int i = 0; i < layers; ++i) {
                    breathe(&times[i][0], &times[i][1], speed, 1.0);
                    breathe(&times_rotate[i][0], &times_rotate[i][1], speed * 2.0, 1.0);
                }
                //gui_printf("times_rotate %d: %d", i, times_rotate[i][0]);
                
            }

            //gui_readout("times_rotate[0]", &times_rotate[0]);
            //gui_readout("times[0]", &times[0]);
            { // update jellyfish
                for (int j = 0; j < layers; ++j) { // update vertex positions
                    real time = times[j][0];
                    real layer_d = (real) j / (layers - 1);
                    real new_rad = radii[j][0] + interpolate(time, 0.0, 1.0) * layer_d;
                    real half_rad = radii[j][1] + interpolate(time, 0.0, 1.0) * layer_d;
                    real new_height = heights[j] + interpolate(time, 0.0, 1.0);
                    rotates[j] = interpolate(times_rotate[j][0], times[j][1] * max_rotate, 0.0);

                    for (int i = 0; i < num; ++i) {
                        orig[j * num_double + 2 * i][0] = cos(angle * i) * new_rad;
                        orig[j * num_double + 2 * i][1] = new_height;
                        orig[j * num_double + 2 * i][2] = sin(angle * i) * new_rad;
                        orig[j * num_double + 2 * i + 1][0] = cos(angle * (i + 0.5)) * half_rad;
                        orig[j * num_double + 2 * i + 1][1] = new_height;
                        orig[j * num_double + 2 * i + 1][2] = sin(angle * (i + 0.5)) * half_rad;
                    }
                }

                orig_head = V3(0.0, heights[0] + interpolate(times[0][0], 0.0, 1.0) ,0.0);
            }

            { // move jellyfish
                slow_twirl = M4_RotationAboutYAxis(RAD(0.5)) * slow_twirl;

                test_movement = movement;
                if (globals.key_held['a'] || globals.key_held['d'] || globals.key_held['w'] || globals.key_held['s']) {
                    real_move_speed = interpolate(times[0][0], jelly_move_speed * 0.7, jelly_move_speed * 1.5);
                    if (globals.key_held['a']) {
                        to_move = M4_Translation(V3(-real_move_speed, 0.0, 0.0));
                        test_movement = to_move * test_movement;
                    } 
                    if (globals.key_held['d']) {
                        to_move = M4_Translation(V3(real_move_speed, 0.0, 0.0));
                        test_movement = to_move * test_movement;
                    } 
                    if (globals.key_held['w']) {
                        to_move = M4_Translation(V3(0.0, 0.0, -real_move_speed * 1.5));
                        test_movement = to_move * test_movement;
                    } 
                    if (globals.key_held['s']) {
                        to_move = M4_Translation(V3(0.0, 0.0, real_move_speed * 1.5));
                        test_movement = to_move * test_movement;
                    }
                } 

                test_head = transformPoint(test_movement, orig_head);
                //gui_readout("test_head_side", &test_head[0]);
                if (ABS(test_head[0]) < lim_side && ABS(test_head[2]) < -lim_far - 3.0) {
                    movement = test_movement;
                } 

                head = transformPoint(slow_twirl, orig_head);
                head = transformPoint(movement, head);
                grounded = transformPoint(movement, orig_base);

                for (int i = 0; i < all; ++i) {
                    drawn[i] = transformPoint(slow_twirl, orig[i]);
                    drawn[i] = transformPoint(movement, drawn[i]);
                }
                for (int i = 0; i < num; ++i) {
                    grounded_base[i] = transformPoint(slow_twirl, orig_grounded_base[i]);
                    grounded_base[i] = transformPoint(movement, grounded_base[i]);
                }
            }

            { // draw jellyfish 
                //soup_draw(PV, SOUP_POINTS, 1, &head, NULL, monokai.white, size);
                //soup_draw(PV, SOUP_POINTS, 1, &grounded, NULL, monokai.green, size);
                soup_draw(PV, SOUP_POINTS, num, grounded_base, NULL, deep_blue, size);
                //soup_draw(PV, SOUP_POINTS, all, drawn, NULL, monokai.red, size);

                // for (int j = 0; j < layers; ++j) {
                //     real scale = LERP((real) j / (layers - 1), 0.8, 0.5);
                //     for (int i = 0; i < num_double; ++i) {
                //         mat4 M_little = M4_Translation(drawn[j * num_double + i]) * M4_RotationAboutYAxis(- i * angle / 2) * M4_RotationAboutZAxis(-rotates[j]) * M4_RotationAboutZAxis(RAD(90)) * M4_Scaling(scale * V3(0.6, 0.05, 0.3));
                //         library.meshes.cylinder.draw(P, V, M_little, ivory);
                //         library.meshes.sphere.draw(P, V, M4_Translation(drawn[j * num_double + i]) * M4_Scaling(scale * 0.1), monokai.black);
                //     }
                // }
                // library.meshes.sphere.draw(P, V, M4_Translation(head) * M4_Scaling(0.6), gold);
            }

            { // jelly shader
                light_positions_world[0] = V3(head[0], 0.5, 40.0);
                light_positions_world[1] = V3(head[0] + 5.0, 0.5, 40.0);
                //gui_readout("light pos", &pale_light_pos);

                // gui_slider("ambientStrength", &jelly_ambientStrength, 0.0, 2.0);
                // gui_slider("diffuseStrength", &jelly_diffuseStrength, 0.0, 2.0);
                // gui_slider("specularStrength", &jelly_specularStrength, 0.0, 2.0);
                // gui_slider("shininess", &jelly_shininess, 0.0, 256.0);

                shader_set_uniform(&shader, "P", P);
                shader_set_uniform(&shader, "V", V);
                shader_set_uniform(&shader, "o_camera_world", camera_get_origin(&camera));
                shader_set_uniform(&shader, "num_lights", num_lights);
                shader_set_uniform(&shader, "light_positions_world", num_lights, light_positions_world);
                shader_set_uniform(&shader, "light_colors", num_lights, light_colors);
                shader_set_uniform(&shader, "ambientStrength", jelly_ambientStrength);
                shader_set_uniform(&shader, "diffuseStrength", jelly_diffuseStrength);
                shader_set_uniform(&shader, "specularStrength", jelly_specularStrength);
                shader_set_uniform(&shader, "shininess", jelly_shininess);
                shader_set_uniform(&shader, "l_const", l_const);
                shader_set_uniform(&shader, "l_lin", l_lin);
                shader_set_uniform(&shader, "l_quad", l_quad);
                shader_set_uniform(&shader, "atten", false);
                // draw bits
                shader_set_uniform(&shader, "orig_color", ivory);

                for (int j = 0; j < layers; ++j) {
                    for (int i = 0; i < num_double; ++i) {
                        mat4 M_bit = M4_Translation(drawn[j * num_double + i]) * slow_twirl * M4_RotationAboutYAxis(- i * angle / 2) * M4_RotationAboutZAxis(-rotates[j]) * M4_RotationAboutZAxis(RAD(90)) * scale[j];
                        shader_set_uniform(&shader, "M", M_bit);
                        shader_pass_vertex_attribute(&shader, jelly_bit.num_vertices, jelly_bit.vertex_positions);
                        shader_pass_vertex_attribute(&shader, jelly_bit.num_vertices, jelly_bit.vertex_normals);
                        shader_draw(&shader, jelly_bit.num_triangles, jelly_bit.triangle_indices);
                    }
                }

                shader_set_uniform(&shader, "orig_color", blood);
                for (int i = 0; i < all; ++i) {
                    shader_set_uniform(&shader, "M", M4_Translation(drawn[i]) * M4_Scaling(0.08));
                    shader_pass_vertex_attribute(&shader, ant_bit.num_vertices, ant_bit.vertex_positions);
                    shader_pass_vertex_attribute(&shader, ant_bit.num_vertices, ant_bit.vertex_normals);
                    shader_draw(&shader, ant_bit.num_triangles, ant_bit.triangle_indices);
                }

                // draw head
                shader_set_uniform(&shader, "orig_color", gold);
                shader_set_uniform(&shader, "M", M4_Translation(head) * M_head);
                shader_pass_vertex_attribute(&shader, jelly_head.num_vertices, jelly_head.vertex_positions);
                shader_pass_vertex_attribute(&shader, jelly_head.num_vertices, jelly_head.vertex_normals);
                shader_draw(&shader, jelly_head.num_triangles, jelly_head.triangle_indices);

            }

            { // grab ants
                if (globals.key_pressed[COW_KEY_SPACE]) { // if press space
                    int closest_ant = -1;
                    real min_dist_ant = 0.5;
                    real dist_ant = 0.0;
                    for (int i = 0; i < num_ants; ++i) {
                        dist_ant = norm(ants[i] - grounded);
                        if (dist_ant < grab_range + 1.0) { // if ant is within reach
                            if (dist_ant < min_dist_ant) { // search for closest ant
                                min_dist_ant = dist_ant;
                                closest_ant = i;
                            }
                        }
                    }

                    // respawn closest ant within reach
                    if (closest_ant != -1) {
                        ants_spawn[closest_ant] = V3(random_real(-10.0, 10.0), ground, random_real(30.0, 50.0));
                        ants[closest_ant] = ants_spawn[closest_ant];
                        ants_crawl[closest_ant] = normalized(V3(random_real(-2.0, 2.0), 0.0, random_real(-1.0, -2.0)));
                        ants_speed[closest_ant] = random_real(ant_move_speed * 0.45, ant_move_speed * 0.86);
                        ants_crawled[closest_ant] = M4_Translation(ants_crawl[closest_ant] * ants_speed[closest_ant]);
                        ants_countdown[closest_ant] = random_real(1.0, 3.0);
                        points++;
                    }
                }
            }

            { // draw_world
                draw_world(PV, ground - 0.2, lim_far);
                // shader_set_uniform(&shader, "orig_color", dirt);
                // shader_set_uniform(&shader, "M", M_ground);
                // shader_pass_vertex_attribute(&shader, ground_mesh.num_vertices, ground_mesh.vertex_positions);
                // shader_pass_vertex_attribute(&shader, ground_mesh.num_vertices, ground_mesh.vertex_normals);
                // shader_draw(&shader, ground_mesh.num_triangles, ground_mesh.triangle_indices);
            }
            
            { // move real ants, spawn lost ants
                for (int i = 0; i < num_ants; ++i) {
                    // check if arrive at finish line, respawn
                    //gui_readout("ants_far:", &ants[i][2]);
                    //gui_readout("ants_side:", &ants[i][0]);
                    if (ants[i][2] < lim_far) {
                        ants_spawn[i] = V3(random_real(-10.0, 10.0), ground, random_real(30.0, 50.0));
                        ants[i] = ants_spawn[i];
                        ants_crawl[i] = normalized(V3(random_real(-2.0, 2.0), 0.0, random_real(-1.0, -2.0)));
                        ants_speed[i] = random_real(ant_move_speed * 0.45, ant_move_speed * 0.86);
                        ants_crawled[i] = M4_Translation(ants_crawl[i] * ants_speed[i]);
                        ants_countdown[i] = random_real(1.0, 3.0);

                        // spawn lost ants onto camera "lens"
                        ants_lost_spawn[lost] = V3(random_sign() * random_real(lim_lost_side, lim_lost_side - 5.0), random_sign() * random_real(lim_lost_height, lim_lost_height - 5.0), 29.0);
                        ants_lost[lost] = ants_spawn[lost];
                        ants_lost_crawl[i] = normalized(V3(random_real(-1.0, 1.0), random_real(-1.0, 1.0), 0.0));
                        ants_lost_speed[i] = random_real(ant_move_speed * 0.45, ant_move_speed * 0.86);
                        ants_lost_crawled[i] = M4_Translation(ants_lost_crawl[i] * ants_lost_speed[i]);
                        ants_lost_countdown[i] = random_real(16.0, 30.0);
                        lost++;
                    }

                    ants_countdown[i] -= 0.0167; // countdown to change directions
                    ants_countdown_turn[i] -= 0.0167; // coundown to change skew
                    // gui_readout("ants_countdown_turn", &ants_countdown_turn[i]);
                    // gui_printf("ants_turn: %d", ants_turn[i]);
                    if (ABS(ants[i][0]) > lim_side || ants_countdown[i] < 0.0) { // if hit side or counted down, change dir
                        if (ABS(ants[i][0]) > lim_side) {
                            ants_crawl[i][0] *= -1.0;
                        } else {
                            ants_crawl[i] = normalized(V3(random_real(-2.0, 2.0), 0.0, random_real(-1.0, -2.0)));
                        }
                        ants_countdown[i] = random_real(1.0, 3.0);
                        ants_speed[i] = random_real(ant_move_speed * 0.45, ant_move_speed * 0.86);
                    } 

                    if (ants_countdown_turn[i] < 0.0) { // if counted down, change skew
                        ants_countdown_turn[i] = random_real(0.5, 2.5);
                        ants_turn[i] = random_real(RAD(-60), RAD(60));
                    }

                    ants_crawled[i] = M4_Translation(transformVector(M4_RotationAboutYAxis(ants_turn[i]), ants_crawl[i] * ants_speed[i])) * ants_crawled[i]; // update position
                    ants[i] = transformPoint(ants_crawled[i], ants_spawn[i]);
                }
                //soup_draw(PV, SOUP_POINTS, num_ants, ants, NULL, monokai.yellow);
                // draw ants
                for (int i = 0; i < num_ants; ++i) {
                    library.meshes.sphere.draw(P, V, M4_Translation(ants[i]) * M4_Scaling(0.2), monokai.black);
                }

                shader_set_uniform(&shader, "orig_color", monokai.black);
                for (int i = 0; i < num_ants; ++i) {
                    shader_set_uniform(&shader, "M", M4_Translation(ants[i]) * M4_Scaling(0.2));
                    shader_pass_vertex_attribute(&shader, ant_bit.num_vertices, ant_bit.vertex_positions);
                    shader_pass_vertex_attribute(&shader, ant_bit.num_vertices, ant_bit.vertex_normals);
                    shader_draw(&shader, ant_bit.num_triangles, ant_bit.triangle_indices);
                }
            }

            { // move lost ants
                if (lost > 0) {
                    for (int i = 0; i < lost; ++i) {
                        ants_lost_countdown[i] -= 0.0167; // countdown to change directions
                        // if hit side or counted down, change dir
                        if (ABS(ants_lost[i][0]) > lim_lost_side - 7.0 || ABS(ants_lost[i][1]) > lim_lost_height - 7.0 || ants_lost_countdown[i] < 0.0) { 
                            if (ants_lost_countdown[i] < 0.0) { // if counted down, change in either one direction
                                bool rand = random_bool();
                                if (rand) {
                                    ants_lost_crawl[i][0] *= -1.0;
                                } else {
                                    ants_lost_crawl[i][1] *= -1.0;
                                }
                            } else { // if hit edge, change in one direction
                                if (ABS(ants_lost[i][0]) > lim_lost_side) {
                                    ants_lost_crawl[i][0] *= -1.0;
                                } 
                                if (ABS(ants_lost[i][1]) > lim_lost_height) {
                                    ants_lost_crawl[i][1] *= -1.0;
                                }
                            }
                            ants_lost_countdown[i] = random_real(1.0, 3.0);
                            ants_lost_speed[i] = random_real(ant_move_speed * 0.45, ant_move_speed * 0.86);
                        } 

                        ants_lost_crawled[i] = M4_Translation(ants_lost_crawl[i] * ants_lost_speed[i]) * ants_lost_crawled[i]; // update position
                        ants_lost[i] = transformPoint(ants_lost_crawled[i], ants_lost_spawn[i]);
                    }
                }
                glDisable(GL_DEPTH_TEST);
                // shader_set_uniform(&shader, "orig_color", monokai.black);
                // shader_set_uniform(&shader, "P", M4_RotationAboutXAxis(RAD(90)) * P);
                // shader_set_uniform(&shader, "V", globals.Identity);
                // soup_draw(P, SOUP_POINTS, lost, ants_lost, NULL, monokai.purple);
                for (int i = 0; i < lost; ++i) {
                    library.meshes.sphere.draw(P, globals.Identity, M4_Translation(ants_lost[i]) * M4_Scaling(0.5), monokai.black);
                    // shader_set_uniform(&shader, "M", M4_Translation(ants[i]) * M4_Scaling(1.0));
                    // shader_pass_vertex_attribute(&shader, ant_bit.num_vertices, ant_bit.vertex_positions);
                    // shader_pass_vertex_attribute(&shader, ant_bit.num_vertices, ant_bit.vertex_normals);
                    // shader_draw(&shader, ant_bit.num_triangles, ant_bit.triangle_indices);
                }
                //soup_draw(P, SOUP_POINTS, lost, ants_lost, NULL, monokai.black, 2.0);
                glEnable(GL_DEPTH_TEST);
                // soup_draw(P, SOUP_LINE_LOOP, 4, screen_bounds, NULL, monokai.purple);
            }
            
            //soup_draw(PV, SOUP_LINES, 2, lim, NULL, monokai.black);  
        }
    }
}

IndexedTriangleMesh3D jelly_bit() {
    IndexedTriangleMesh3D sculpted = {};
    sculpted.num_vertices = library.meshes.cylinder.num_vertices;
    sculpted.num_triangles = library.meshes.cylinder.num_triangles;

    ASSERT(library.meshes.cylinder.vertex_positions != NULL);
    sculpted.vertex_positions = (vec3 *) calloc(sculpted.num_vertices, sizeof(vec3));
    memcpy(sculpted.vertex_positions, library.meshes.cylinder.vertex_positions, sculpted.num_vertices * sizeof(vec3));
    sculpted.vertex_normals = (vec3 *) calloc(sculpted.num_vertices, sizeof(vec3));
    memcpy(sculpted.vertex_normals, library.meshes.cylinder.vertex_normals, sculpted.num_vertices * sizeof(vec3));
    sculpted.triangle_indices = (int3 *) calloc(sculpted.num_triangles, sizeof(int3));
    memcpy(sculpted.triangle_indices, library.meshes.cylinder.triangle_indices, sculpted.num_triangles * sizeof(int3));

    sculpted.vertex_colors = (vec3 *) calloc(sculpted.num_vertices, sizeof(vec3));
 
    for (int i = 0; i < sculpted.num_vertices; ++i) {
        vec3 upper_rotate = V3(0.0, 0.3, 0.0);
        sculpted.vertex_colors[i] = V3(247.0 / 255.0, 218.0 / 255.0, 201.0 / 255.0);
        if (sculpted.vertex_positions[i][1] < 0.5) {
            sculpted.vertex_colors[i] = V3(18, 23, 179) / 255.0;
        }
        if (sculpted.vertex_positions[i][2] < 0.2) {
            sculpted.vertex_colors[i] = V3(209.0, 118.0, 0.0) / 255.0;
        }

        sculpted.vertex_positions[i] = transformPoint(M4_Scaling(V3(0.5, 0.2, 0.8)), sculpted.vertex_positions[i]);
        sculpted.vertex_normals[i] = transformVector(M4_Scaling(V3(0.5, 0.2, 0.8)), sculpted.vertex_normals[i]);

        // sculpted.vertex_positions[i] = transformPoint(M4_RotationAboutXAxis(RAD(90)), sculpted.vertex_positions[i]);
        // sculpted.vertex_normals[i] = transformVector(M4_RotationAboutXAxis(RAD(90)), sculpted.vertex_normals[i]);
    }
    return sculpted;
}

void what() {
    Camera3D camera = {20.0};
    int num = 4;
    real angle = 2 * PI / num;

    IndexedTriangleMesh3D jelly_bit = library.meshes.cylinder;

    mat4 rotate = M4_RotationAboutXAxis(RAD(90));
    IndexedTriangleMesh3D cylinder = library.meshes.cylinder; 
    // for (int i = 0; i < cylinder.num_vertices; ++i) {
    //     cylinder.vertex_positions[i] = V3(0.0);
    //     //cylinder.vertex_positions[i] = transformPoint(rotate, cylinder.vertex_positions[i]);
    //     //cylinder.vertex_normals[i] = transformVector(rotate, cylinder.vertex_normals[i]);
    // }

    while (cow_begin_frame()) {
        mat4 PV = camera_get_PV(&camera);
        mat4 P = camera_get_P(&camera);
        mat4 V = camera_get_V(&camera);
        camera_move(&camera);
        for (int i = 0; i < cylinder.num_vertices; ++i) {
            
            //cylinder.vertex_positions[i] = transformPoint(rotate, cylinder.vertex_positions[i]);
            //cylinder.vertex_normals[i] = transformVector(rotate, cylinder.vertex_normals[i]);
        }
        cylinder.vertex_positions[0] = V3(1.0);
        //gui_readout("first", &cylinder.vertex_positions[0]);
        cylinder.draw(P, V, globals.Identity, monokai.green);
    }
}

int main() {
    APPS {
        APP(ants);
        //APP(what);
    }
    return 0;
}

