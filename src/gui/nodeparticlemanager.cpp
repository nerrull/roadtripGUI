#include "nodeparticlemanager.h"


NodeParticleManager::NodeParticleManager()
{
    compute.setupShaderFromFile(GL_COMPUTE_SHADER,"particle_node_compute.glsl");
    compute.linkProgram();

    maxSpeed = 300;
    activeNodeForce =0;
    nodeForce=2500;
    linearForce = 0;
    repulsionCoeff= 0.1;
    cohesionCoeff = 0;
    frictionCoeff =0.01;
    numNodes = 16;
    draw_nodes =false;
    num_particles = 1024*8;
    particles.resize(num_particles);
    structure_nodes.resize(4096);
    simple_nodes.resize(4096);

    int i=0;
    for(auto & p: particles){
        p.pos.x = ofRandom(-500,500);
        p.pos.y = ofRandom(-500,500);
        p.pos.z = ofRandom(-500,500);
        p.pos.w = 1;
        p.vel.set(0,0,0,0);
        p.mass_target_life.set(ofRandom(1,10),ofRandom(0,10),0.,0.);
        i++;
    }

    i =0;
    for(auto & n: structure_nodes){
        i++;

        n.pos.x =  i*ofGetWidth()/structure_nodes.size()*10 - ofGetWidth()/2;
        n.pos.y = -sin(float(i)*TWO_PI*2/structure_nodes.size())* ofGetHeight()/2;
        n.pos.z = -ofGetHeight()/2;
        n.direction = ofVec3f(1.);
        n.is_linear=0.;
        if (i%2 ==0) n.force= 1.;
        else n.force = 0.;
    }

    particlesBuffer.allocate(particles,GL_DYNAMIC_DRAW);
    particlesBuffer2.allocate(particles,GL_DYNAMIC_DRAW);
    nodesBuffer.allocate(structure_nodes,GL_DYNAMIC_DRAW);
    simpleNodesBuffer.allocate(simple_nodes, GL_DYNAMIC_DRAW);

    vbo.setVertexBuffer(particlesBuffer,4,sizeof(Particle));
    vbo.setColorBuffer(particlesBuffer,sizeof(Particle),sizeof(ofVec4f)*2);
    //vbo.disableColors();
    dirAsColor = false;

    particlesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    particlesBuffer2.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    nodesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER,2);
    simpleNodesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER,3);

}

void NodeParticleManager::update(){

    compute.begin();
    compute.setUniform1f("timeLastFrame",ofGetLastFrameTime());
    compute.setUniform1f("elapsedTime",ofGetElapsedTimef());
    compute.setUniform3f("active_node",activeNode.x,activeNode.y,activeNode.z);
    compute.setUniform1f("max_speed", maxSpeed);
    compute.setUniform1f("node_force", nodeForce);
    compute.setUniform1f("linear_force", linearForce);
    compute.setUniform1f("num_particles", num_particles);


    compute.setUniform1f("active_node_force", activeNodeForce);
    compute.setUniform1f("repulsion", repulsionCoeff);
    compute.setUniform1f("cohesion", cohesionCoeff);
    compute.setUniform1f("friction", frictionCoeff);
    compute.setUniform1i("num_nodes", numNodes);
    compute.setUniform1i("num_simple_nodes", numSimpleNodes);
    compute.setUniform1i("simple_aoe", simple_aoe);

    compute.dispatchCompute((particles.size() + 1024 -1 )/1024, 1, 1);
    compute.end();
    particlesBuffer.copyTo(particlesBuffer2);
}

void NodeParticleManager::draw(){
    ofSetColor(255,70);
    glPointSize(2);
    vbo.draw(GL_POINTS,0,particles.size());
    ofSetColor(255);
    glPointSize(1);
    vbo.draw(GL_POINTS,0,particles.size());

    if (draw_nodes){
        for(int i =0; i<numNodes; i++){
            if (i>numNodes )break;
            ofSetColor(structure_nodes[i].color);

            //ofSetColor(ofColor::green, 150);
            ofDrawSphere(structure_nodes[i].pos, 5);
        }

        for(int i =0; i<numSimpleNodes; i++){
            ofSetColor(0, 255,255);

            //ofSetColor(ofColor::green, 150);
            ofDrawSphere(ofVec3f(simple_nodes[i].pos.x, simple_nodes[i].pos.y, simple_nodes[i].pos.z), simple_aoe);
        }
        ofSetColor(ofColor::red);
        ofDrawSphere(activeNode,10);
    }
}

void NodeParticleManager::update_active(ofVec3f active){
    activeNode = active;
}

/*
struct graph_node{
    int point_index;
    vector<int> near_indexes;
    vector<float> near_weights;
};*/

void NodeParticleManager::updateNodesActivePoints(vector<ofVec3f> activePoints, vector<ofFloatColor> colors, ofVec3f activeNode){
    numNodes = activePoints.size()+1;
    structure_nodes.resize(numNodes);

    //add active node
    structure_nodes[0].color=ofFloatColor(colors[0]);
    structure_nodes[0].pos = ofVec4f(activeNode.x, activeNode.y, activeNode.z, 1.);
    structure_nodes[0].is_linear = 0.;
    structure_nodes[0].force = 1.;
    structure_nodes[0].neighbour_indexes = ofVec4f(0., -1., -1.,-1.);
    structure_nodes[0].neighbour_weights = ofVec4f(1., 1., 1., 1.);

    //add secondary nodes
    int i =0;
    for (auto p : activePoints){
        structure_nodes[i].color=ofFloatColor(colors[i+1]);
        structure_nodes[i].pos =  ofVec4f(p.x, p.y, p.z, 1.);
        structure_nodes[i].is_linear = 0.;
        structure_nodes[i].force = 1.;
        structure_nodes[i].neighbour_indexes = ofVec4f(0., -1., -1.,-1.);
        structure_nodes[i].neighbour_weights = ofVec4f(1., 1., 1., 1.);
        i++;
    }
    nodesBuffer.updateData(structure_nodes);
//    update_active(structure_nodes[0].pos);
}

//void NodeParticleManager::update_nodes_from_nn(vector<ofVec3f> points, vector<graph_node> nodes ){
//    numNodes = points.size();
//    structure_nodes.resize(numNodes);
//    int i =0;
//    for (auto n : nodes){
//        structure_nodes[i].color=ofFloatColor(ofRandomuf(), ofRandomuf(), ofRandomuf(), 0.5);
//        structure_nodes[i].pos = points[n.point_index];
//        structure_nodes[i].is_linear = 0.;
//        structure_nodes[i].force = 1.;
//        structure_nodes[i].neighbour_indexes = ofVec4f(n.near_indexes[0],n.near_indexes[1], n.near_indexes[2],n.near_indexes[3]);
//        structure_nodes[i].neighbour_weights = ofVec4f(n.near_weights[0],n.near_weights[1], n.near_weights[2],n.near_weights[3]);
//        i++;
//    }
//    nodesBuffer.updateData(structure_nodes);
//    generate_intermediate_nodes(points, nodes);
//    update_active(structure_nodes[0].pos);
//}



//template <typename T>
//struct unorderLess
//{
//    bool operator () (const std::pair<T, T>& lhs, const std::pair<T, T>& rhs) const
//    {
//        const auto lhs_order = lhs.first < lhs.second ? lhs : std::tie(lhs.second, lhs.first);
//        const auto rhs_order = rhs.first < rhs.second ? rhs : std::tie(rhs.second, rhs.first);

//        return lhs_order < rhs_order;
//    }
//};

//void NodeParticleManager::generate_intermediate_nodes(vector<ofVec3f> points, vector<graph_node> nodes){
//    set<pair<int,int>, unorderLess<int>> unique_node_pairs;
//    for (auto n : nodes){
//        for (int n2_index: n.near_indexes){
//            unique_node_pairs.insert(pair<int,int>(n.point_index,n2_index));
//        }
//    }
//    int i = 0;
//    ofVec3f p1, p2;
//    for (pair<int,int> p : unique_node_pairs){
//        p1 =  points[p.first];
//        p2 =  points[p.second];
//        ofVec3f mid1 = p1 + (p2-p1).normalize()*20.;
//        ofVec3f mid2 = p2 + (p1-p2).normalize()*20.;
//        ofVec3f mid3 = p2 + (p1-p2)/2.;


//        simple_nodes[i].pos=ofVec4f(mid1.x, mid1.y, mid1.z, 1.);
//        simple_nodes[i+1].pos=ofVec4f(mid2.x, mid2.y, mid2.z, 1.);
//        simple_nodes[i+2].pos=ofVec4f(mid3.x, mid3.y, mid3.z, 1.);


//        i+=3;
//    }
//    numSimpleNodes=  i;
//    simpleNodesBuffer.updateData(simple_nodes);
//}


void NodeParticleManager::update_nodes(vector<ofVec3f> main_points, vector<ofVec3f> intermediate_points, vector<ofVec3f> directions){
    numNodes = main_points.size()+ intermediate_points.size();
    structure_nodes.resize(numNodes);
    int i =0;
    for (auto p : main_points){
        structure_nodes[i].pos=p;
        structure_nodes[i].is_linear = 0.;
        structure_nodes[i].force = 1.;
        i++;
    }

    int counter=0;
    for (auto p : intermediate_points){
        structure_nodes[i].pos=p;
        structure_nodes[i].direction = directions[counter];
        structure_nodes[i].is_linear = 1.;
        structure_nodes[i].force = 1.;
        i++;
        counter++;
    }
    nodesBuffer.updateData(structure_nodes);
    updateParticles();
}

void NodeParticleManager::updateParticles(){
    for(auto & p: particles){
        p.mass_target_life.set(ofRandom(1,10),ofRandom(0,numNodes),0.,0.);
    }
    particlesBuffer.updateData(particles);

}


void NodeParticleManager::toggleForces(){
    for(auto & n: structure_nodes){
        if (n.force >0.){
            n.force = -1.;
        }
        else {
            n.force = 1.;
        }
    }
    nodesBuffer.updateData(structure_nodes);
}


void NodeParticleManager::setParameters(ofParameterGroup pg){

    maxSpeed = pg.getFloat("max_speed");
    nodeForce= pg.getFloat("node_force");
    linearForce= pg.getFloat("linear_force");

    activeNodeForce = pg.getFloat("active_node_force");
    repulsionCoeff=pg.getFloat("repulsion");
    cohesionCoeff=pg.getFloat("cohesion");

    frictionCoeff=pg.getFloat("friction");
    numNodes= pg.getInt("num_nodes");
    draw_nodes= pg.getBool("show_nodes");
    simple_aoe = pg.getInt("simple_aoe");
}
