#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "triangle.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"
#include "pdf.h"
#include "rtw_stb_obj_loader.h"

#include <omp.h>
#include <iostream>

#define rep(i, a, b) for(int i = (a); i < (b); ++i)
#define brep(i, a, b) for(int i = (b)-1; i >= (a); --i)

color ray_color(
        const ray& r, 
        const shared_ptr<texture>& background, 
        const shared_ptr<pdf>& background_pdf, 
        const hittable& world, 
        const shared_ptr<hittable>& lights, 
        int depth) {
    hit_record rec;
    

    if (depth <= 0){
        return color(0, 0, 0);
    }
    if(!world.hit(r, 0.000001, infinity, rec)){
        auto unit_dir = unit_vector(r.direction());
        double u, v; get_spherical_uv(unit_dir, u, v);
        return background->value(u, v, unit_dir);
    }

    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, srec)){
        return emitted;
    }

    // no importance sampling
    if (srec.is_specular){
        return srec.attenuation * ray_color(srec.specular_ray, background, background_pdf, world, lights, depth-1);
    }

    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p_objs(light_ptr, srec.pdf_ptr, 0.5);
    mixture_pdf p(make_shared<mixture_pdf>(p_objs), background_pdf, 0.8);

    ray scattered = ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());

    return emitted + 
        srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
                         * ray_color(scattered, background, background_pdf, world, lights, depth-1) / pdf_val;
}

hittable_list rt_iow_final_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    const int extent = 11; // default 11
    for (int a = -extent; a < extent; a++) {
        for (int b = -extent; b < extent; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center+vec3(0, random_double(0, 0.5), 0);
                    world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list rt_iow_final_scene_lights(){
    // an empty hittable list gives importance sampling with cosine distribution over the normal-hemisphere
    hittable_list lights;
    return lights;
}

/*
hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);

    objects.add(make_shared<sphere>(point3(0,-1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("../src/textures/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

hittable_list cornell_smoke() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    objects.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));

    return objects;
}
*/

hittable_list rt_tnw_final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(123, 423, 147, 412, 554, light)));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("textures/earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = make_shared<marble_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
            vec3(-100,270,395)
        )
    );

    return objects;
}

hittable_list rt_tnw_final_scene_lights() {
    hittable_list lights;
    auto mat = make_shared<material>();

    lights.add(make_shared<xz_rect>(123, 423, 147, 412, 554, mat));

    return lights;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190,90,190), 90 , glass));

    return objects;
}

hittable_list cornell_box_lights(){
    hittable_list lights;
    lights.add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    lights.add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));    
    lights.add(make_shared<box>(point3(0,0,0), point3(165,330,165), shared_ptr<material>()));
    return lights;
}

hittable_list triangle_test() {
    hittable_list objects;

    auto light = make_shared<diffuse_light>(color(1, 1, 1));
    auto grey = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto blue = make_shared<lambertian>(color(0.1, 0.1, 0.7));

    objects.add(make_shared<xy_rect>(-10, 10, -10, 10, 0, grey));
    triangle light_tri = triangle(vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1), light);
    objects.add(make_shared<triangle>(light_tri));

    objects.add(make_shared<triangle>(triangle(vec3(1, 0, 0), vec3(1, 0, 2), vec3(1, 2, 2), blue)));

    return objects;
}

hittable_list triangle_test_lights() {
    hittable_list lights;

    triangle test_tri = triangle(vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1), shared_ptr<material>());
    lights.add(make_shared<triangle>(test_tri));

    return lights;
}

hittable_list obj_loader_test(){
    hittable_list objects;

    auto grey = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto light = make_shared<diffuse_light>(color(1, 1, 1)*10);
    objects.add(make_shared<xz_rect>(-10, 10, -10, 10, -1, grey));
    //objects.add(make_shared<flip_face>(make_shared<xz_rect>(-1, 1, 2, 3, 4, light)));
    
    objects.add(load_model_from_file("../models/suzanne.obj", grey, false));

    return objects;
}

hittable_list obj_loader_test_lights(){
    hittable_list lights;

    //lights.add(make_shared<flip_face>(make_shared<xz_rect>(-1, 1, 2, 3, 4, shared_ptr<material>())));

    return lights;
}

hittable_list boeing_test_world(){
    hittable_list objects;

    auto grey = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto light = make_shared<diffuse_light>(color(1, 1, 1)*100);
    int ground_size = 80;
    objects.add(make_shared<xy_rect>(-ground_size/2., ground_size/2., -ground_size/2., ground_size/2., -8, grey));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(-5, 5, -5, 5, 40, light)));
    
    objects.add(load_model_from_file("../models/boeing_737_900.obj", grey, true));

    return objects;
}

hittable_list boeing_test_world_lights(){
    hittable_list lights;

    lights.add(make_shared<flip_face>(make_shared<xy_rect>(-5, 5, -5, 5, 40, shared_ptr<material>())));

    return lights;
}

hittable_list cornell_klein_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    /*
    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190,90,190), 90 , glass));*/
    auto glass = make_shared<dielectric>(1.5);
    vec3 move_klein(300, 60, 200);
    objects.add(make_shared<translate>(load_model_from_file("../models/klein_bottle.obj", glass, true), move_klein));

    return objects;
}

hittable_list cornell_klein_box_lights(){
    hittable_list lights;
    lights.add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    return lights;
}

hittable_list theodor_test1_world(){
    hittable_list objects;

    auto grey = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto metal_mat = make_shared<metal>(color(0.6, 0.6, 0.6), 0.3);
    auto light = make_shared<diffuse_light>(color(1, 1, 1)*300);
    int ground_size = 140;
    objects.add(make_shared<xz_rect>(-ground_size/2., ground_size/2., -ground_size/2., ground_size/2., -8, grey));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(-5, 5, -5, 5, 150, light)));
    double camoffset0 = 60;
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(-5+camoffset0, 5+camoffset0, -5, 5, 150, light)));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(-5, 5, -5+camoffset0, 5+camoffset0, 150, light)));
    
    vec3 displacement(-25, 0, 10);
    shared_ptr<hittable> model = (make_shared<translate>(load_model_from_file("../models/from_theodor.obj", metal_mat, true), displacement));
    objects.add(model);

    return objects;
}

hittable_list theodor_test1_lights(){
    hittable_list lights;

    lights.add(make_shared<flip_face>(make_shared<xz_rect>(-5, 5, -5, 5, 150, shared_ptr<material>())));
    double camoffset0 = 60;
    lights.add(make_shared<flip_face>(make_shared<xz_rect>(-5+camoffset0, 5+camoffset0, -5, 5, 150, shared_ptr<material>())));
    lights.add(make_shared<flip_face>(make_shared<xz_rect>(-5, 5, -5+camoffset0, 5+camoffset0, 150, shared_ptr<material>())));

    return lights;
}
int main() {
    // image  settings
    int samples_per_pixel = 1600;
    int max_depth = 16;
    double aspect_ratio = 16./9.;
    const int image_width = 1920;

    int scene_to_render = 1;

    const int N_THREADS = 10;
    const int CHUNKS_PER_THREAD = 4;

    // scene and camera
    hittable_list world;
    // dummy value, hittable is pure
    auto lights = make_shared<hittable_list>();
    point3 lookfrom, lookat;
    auto vfov = 40.0;
    vec3 vup(0, 1, 0);
    auto aperture = 0.0;

    auto background_skybox = make_shared<image_texture>("../models/christmas_studio_2k.hdr");
    shared_ptr<texture> background = background_skybox;
    shared_ptr<pdf> background_pdf = make_shared<image_pdf>(background_skybox);

    switch (scene_to_render) {
        case 1:
            world = rt_iow_final_scene();
            lights = make_shared<hittable_list>(rt_iow_final_scene_lights());
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            aperture = 0.1;
            break;
        /*
        case 2:
            world = two_spheres();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;
        case 3:
            world = two_perlin_spheres();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;
        case 4:
            world = earth();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;
        case 5:
            world = simple_light();
            background = color(0,0,0);
            lookfrom = point3(26,3,6);
            lookat = point3(0,2,0);
            vfov = 20.0;
            break;
        */
        case 6:
            world = cornell_box();
            lights = make_shared<hittable_list>(cornell_box_lights());
            background = make_shared<solid_color>(color(0, 0, 0));
            aspect_ratio = 1.0;
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        /*case 7:
            world = cornell_smoke();
            aspect_ratio = 1.0;
            background = color(0,0,0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break; */
        case 8:
            world = rt_tnw_final_scene();
            lights = make_shared<hittable_list>(rt_tnw_final_scene_lights());
            lookfrom = point3(478, 278, -600);
            lookat = point3(278, 278, 0);
            vfov = 45.0;
            break;
        case 9:
            world = triangle_test();
            lights = make_shared<hittable_list>(triangle_test_lights());
            lookfrom = point3(-4, 1, 2);
            lookat = point3(0, 0, 1);
            vup = vec3(0, 0, 1);
            vfov = 40.0;
            break;
        case 10:
            world = obj_loader_test();
            lights = make_shared<hittable_list>(obj_loader_test_lights());
            lookfrom = point3(0, 0.5, -3);
            lookat = point3(0, -0.1, 0);
            vfov = 45.0;
            break;
        case 11:
            vup = vec3(0, 0, 1);
            world = boeing_test_world();
            lights = make_shared<hittable_list>(boeing_test_world_lights());
            lookfrom = point3(0, -40, 20);
            lookat = point3(0, 0, 0);
            vfov = 40.0;
            break;
        case 12:
            world = cornell_klein_box();
            lights = make_shared<hittable_list>(cornell_klein_box_lights());
            aspect_ratio = 1.0;
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        case 13:
            world = theodor_test1_world();
            lights = make_shared<hittable_list>(theodor_test1_lights());
            lookfrom = point3(40, 55, 40);
            lookat = point3(-10, 5, 0);
            vfov = 45.0;
            break;
        case 14:
            auto grey = make_shared<lambertian>(color(0.5, 0.5, 0.5));
            auto central_sphere = make_shared<sphere>(vec3(), 1, grey);
            world.add(central_sphere);
            lookfrom = point3(40, 55, 40);
            lookat = point3(-10, 5, 0);
            vfov = 45.0;
            break;
    }

    const int image_height = static_cast<int>(image_width/aspect_ratio);
    auto image = new color[image_height][image_width];

    auto dist_to_focus = 10;
    double cam_time0 = 0.0;
    double cam_time1 = 1.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, cam_time0, cam_time1); 

    // render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    int global_done_scanlines=0;
    std::cerr << "Image dimensions: " << image_width << ' ' << image_height << ".\n";

    #pragma omp parallel num_threads(N_THREADS)
    {
    srand(int(time(NULL))^ omp_get_thread_num());
    #pragma omp for schedule(dynamic, image_height/(N_THREADS*CHUNKS_PER_THREAD))
    for (int j = image_height-1; j >= 0; --j) {
        #pragma omp critical
        {
            if ((image_height-global_done_scanlines) % 10 == 0){
                std::cerr << "\rScanlines remaining: " << image_height-global_done_scanlines << " " << std::flush;
            }
        }
        #pragma omp atomic
        ++global_done_scanlines;

        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for(int s = 0; s < samples_per_pixel; ++s){
                auto u = (i+random_double()) / (image_width-1);
                auto v = (j+random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                color ray_contribution = ray_color(r, background, background_pdf, world, lights, max_depth);
                zero_nan_vals(ray_contribution);
                pixel_color += ray_contribution;
            }
            
            image[j][i] = pixel_color;
        }
    }
    }
    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            write_color(std::cout, image[j][i], samples_per_pixel);
        }
    }

    delete[] image;

    std::cerr << "\nDone\n";
    std::cout << std::flush;
    return 0;
}
