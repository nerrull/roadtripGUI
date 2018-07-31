#version 440

//mass_target_life contains multiple parameters
struct Particle{
        vec4 pos;
        vec4 vel;
        vec4 color;
        vec4 mass_target_life;
};

struct Node{
    vec3 pos;
    float force;
    vec3 direction;
    float is_linear;
    vec4 neighbour_indexes;
    vec4 neighbour_weights;
    vec4 color;
};

struct SimpleNode{
    vec4 pos;
};



layout(std140, binding=0) buffer particle{
    Particle p[];
};

layout(std140, binding=1) buffer particleBack{
    Particle p2[];
};

layout(std140, binding=2) buffer nodes{
    Node n[];
};

layout(std140, binding=3) buffer simplenodes{
    SimpleNode sn[];
};



uniform vec3 active_node;
uniform float timeLastFrame;
uniform float elapsedTime;
uniform float max_speed;
uniform float active_node_force;
uniform float linear_force;
uniform float node_force;

uniform float repulsion;
uniform float cohesion;
uniform float friction;
uniform int num_nodes;
uniform int num_simple_nodes;
uniform int simple_aoe;
uniform int num_particles;

const int SIMPLE_NODE_PARTICLE_LIMIT  = 1024*128;
const int PARTICLE_INTERACTION_PARTICLE_LIMIT  = 1024*128;


vec3 rule1(vec3 my_pos, vec3 their_pos){
        vec3 dir = my_pos-their_pos;
        float sqd = dot(dir,dir);
        if(sqd < 300.0*300.0){
            return dir;
        }
        return vec3(0.0);
}

vec3 rule2(vec3 my_pos, vec3 their_pos, vec3 my_vel, vec3 their_vel){
        vec3 d = their_pos - my_pos;
        vec3 dv = their_vel - my_vel;
        return dv / (dot(dv,dv) + 10.0);
}

vec3 rule3(vec3 my_pos, vec3 their_pos){
        vec3 dir = their_pos-my_pos;
        float sqd = dot(dir,dir);
        if(sqd < 50.0*50.0){
                float f = 1000000.0/(sqd+1);
                return normalize(dir)*f;
        }
        return vec3(0.0);
}

vec3 node_linear(vec3 my_pos, vec3 my_vel, vec3 n_pos, vec3 n_direction){

     vec3 dir = n_pos-my_pos;
     float sqd = dot(dir,dir);

     if(sqd > 50.0*50.0){
         return vec3(0.0);
     }

     vec3 vel = normalize(my_vel);
     float mag = (dot(n_direction, vel) +0.0001);
     return n_direction*mag;
}

vec3 node_attraction(vec3 my_pos, vec3 n_pos){
     vec3 dir = n_pos-my_pos;
     float sqd = dot(dir,dir);
     float f =  1000000./(sqd+0.001);
     return normalize(dir)*f;
     return vec3(0.0);
}

vec3 simple_node_repulsion(vec3 my_pos, vec3 n_pos){
    float dist = distance(n_pos, my_pos);

     vec3 dir = my_pos -n_pos;
     if (dist<float(simple_aoe)){
         vec3 dir =my_pos - n_pos;
         float sqd = dot(dir,dir);

         float f =  1000000./(sqd+0.001);
         return normalize(dir)*f;
     }

     return vec3(0.0);
}



float rnd(vec2 x)
{
    int n = int(x.x * 40.0 + x.y * 6400.0);
    n = (n << 13) ^ n;
    return 1.0 - float( (n * (n * n * 15731 + 789221) + \
             1376312589) & 0x7fffffff) / 1073741824.0;
}


float get_next_index(Particle part, uint node_index){
//    float rand_value =rnd(vec2(elapsedTime,gl_GlobalInvocationID.x ));
    //rand_value = float(rand()) / float(RAND_MAX);
//    if (rand_value > currentNode.neighbour_weights.z) return currentNode.neighbour_indexes.w;
//    else if (rand_value > currentNode.neighbour_weights.y) return currentNode.neighbour_indexes.z;
//    else if (rand_value > currentNode.neighbour_weights.x) return currentNode.neighbour_indexes.y;
//    if (rand_value > 0.75) return n[node_index].neighbour_indexes.w;
//    else if (rand_value >0.5) return n[node_index].neighbour_indexes.z;
//    else if (rand_value > 0.25) return n[node_index].neighbour_indexes.y;
//    return n[node_index].neighbour_indexes.x;
    return n[node_index].neighbour_indexes.x;
}

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;
void main(){
        Particle particle = p2[gl_GlobalInvocationID.x];
        vec3 pos = particle.pos.xyz;
        vec3 acc = vec3(0.0,0.0,0.0);

        uint start = 0;
        uint end = 512;
        uint step = num_nodes/512;

//         For interaction with all particles
//         uint start =0;
//         uint endstructure_nodes = 1024 *8;

            for(uint i=start;i<end;i++){
                uint idx = uint(i*step+elapsedTime)%num_nodes;

                if(i!=gl_GlobalInvocationID.x){
                    acc += rule1(pos,p2[i].pos.xyz) * repulsion;
                    acc += rule2(pos,p2[i].pos.xyz, p2[gl_GlobalInvocationID.x].vel.xyz, p2[i].vel.xyz) * cohesion;
//                    acc += rule3(pos,p2[i].pos.xyz) * repulsion;
                }
            }

//            for(uint i=0;i<num_simple_nodes;i++){
//                acc+= simple_node_repulsion(pos, sn[i].pos.xyz)*node_force/10.;
//            }

        highp int index = int(particle.mass_target_life.y);
        if (distance(pos, n[index].pos.xyz)<5.){
             p[gl_GlobalInvocationID.x].mass_target_life.y  = get_next_index(particle, index);
        }

        acc+= node_attraction(pos, n[index].pos.xyz)*node_force;
        p[gl_GlobalInvocationID.x].pos.xyz += p[gl_GlobalInvocationID.x].vel.xyz*timeLastFrame;

        acc += node_attraction(pos, active_node)*active_node_force;
        acc = acc/ p2[gl_GlobalInvocationID.x].mass_target_life.x;

        p[gl_GlobalInvocationID.x].vel.xyz += acc*timeLastFrame;
        p[gl_GlobalInvocationID.x].vel.xyz *= (1.-friction);

        vec3 vel_unit = normalize(p[gl_GlobalInvocationID.x].vel.xyz + vec3(0.000001));
        if(length(p[gl_GlobalInvocationID.x].vel.xyz)>max_speed){
                p[gl_GlobalInvocationID.x].vel.xyz = vel_unit * max_speed;
        }

        p[gl_GlobalInvocationID.x].mass_target_life.z = p2[gl_GlobalInvocationID.x].mass_target_life.z +1;

        vec3 dir = active_node - pos;
        float max_component = max(max(dir.x,dir.y),dir.z);
        p[gl_GlobalInvocationID.x].color.rgb = n[index].color.xyz;
//        p[gl_GlobalInvocationID.x].color.rgb = dir/max_component;
        p[gl_GlobalInvocationID.x].color.a = 0.4;
}
