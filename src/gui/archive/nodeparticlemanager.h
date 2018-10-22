#ifndef NODEPARTICLEMANAGER_H
#define NODEPARTICLEMANAGER_H
#include "ofBufferObject.h"
#include "ofMain.h"

class NodeParticleManager
{
public:
    NodeParticleManager();
    void update();
    void draw();

    void toggleForces();
    void setParameters(ofParameterGroup pg);
    void update_active(ofVec3f active);
    void update_nodes(vector<ofVec3f> , vector<ofVec3f>, vector<ofVec3f>);
    void updateParticles();

    void updateNodesFromLines(vector<ofPolyline>);
    void updateNodesActivePoints(vector<ofVec3f> activePoints, vector<ofFloatColor> colors, ofVec3f activeNode);

//    void update_nodes_from_nn(vector<ofVec3f> points, vector<graph_node> nodes );
//    void generate_intermediate_nodes(vector<ofVec3f> points, vector<graph_node> nodes);


    struct Particle{
        ofVec4f pos;
        ofVec4f vel;
        ofFloatColor color;
        ofVec4f mass_target_life;
    };

    struct Node{
        ofVec3f pos;
        float force;
        ofVec3f direction;
        float is_linear;
        ofVec4f neighbour_indexes;
        ofVec4f neighbour_weights;
        ofFloatColor color;

    };

    struct SimpleNode{
        ofVec4f pos;
    };

    vector<Particle> particles;
    vector<Node> structure_nodes;
    vector<SimpleNode> simple_nodes;
    ofVec3f activeNode;
    ofShader compute;
    ofBufferObject particlesBuffer, particlesBuffer2, nodesBuffer, simpleNodesBuffer;
    GLuint vaoID;
    ofVbo vbo;
    bool dirAsColor;

    bool draw_nodes;
    float maxSpeed;
    float nodeForce, activeNodeForce, linearForce;
    float repulsionCoeff, cohesionCoeff, frictionCoeff;
    int numNodes;
    int numSimpleNodes;
    int num_particles;
    int simple_aoe;


};

#endif // NODEPARTICLEMANAGER_H
