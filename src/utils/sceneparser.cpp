#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

void dfsSceneGraph(SceneNode*, std::vector<RenderShapeData>&, std::vector<SceneLightData>&, glm::mat4);

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();
    renderData.lights.clear();
    glm::mat4 ctm = glm::mat4(1.0f);

    dfsSceneGraph(root, renderData.shapes, renderData.lights, ctm);

    return true;
}


void dfsSceneGraph(SceneNode* root, std::vector<RenderShapeData> &rs_data, std::vector<SceneLightData> &sl_data, glm::mat4 ctm){
    if (root==nullptr){
        return;
    }

    // process here
    glm::mat4 tmp(1.0f);
    for (auto tfm: root->transformations){
        if (tfm->type==TransformationType::TRANSFORMATION_TRANSLATE){
            // glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), tfm->translate);
            //ctm = ctm * translationMatrix;
            // tmp = tmp * translationMatrix;
            tmp = glm::translate(tmp, tfm->translate);
        }
        else if (tfm->type==TransformationType::TRANSFORMATION_SCALE){
            // glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), tfm->scale);
            //ctm = ctm * scaleMatrix;
            // tmp = tmp * scaleMatrix;
            tmp = glm::scale(tmp, tfm->scale);
        }
        else if (tfm->type==TransformationType::TRANSFORMATION_ROTATE){
            // glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), tfm->angle, tfm->rotate);
            //ctm = ctm * rotationMatrix;
            //tmp =  tmp * rotationMatrix;
            tmp = glm::rotate(tmp, tfm->angle, tfm->rotate);
        }
        else if (tfm->type==TransformationType::TRANSFORMATION_MATRIX){
            //ctm = ctm * tfm->matrix;
            tmp = tmp * tfm->matrix;
        }
    }
    ctm = ctm * tmp;

    for (auto p: root->primitives){
        RenderShapeData rsd;
        rsd.primitive.type = p->type;
        rsd.primitive.material = p->material;
        rsd.primitive.meshfile = p->meshfile;

        rsd.ctm = ctm;
        rs_data.push_back(rsd);
    }

    for (auto l: root->lights){
        SceneLightData sld;
        sld.id = l->id;
        sld.type = l->type;
        sld.color = l->color;
        sld.function = l->function;

        if (l->type!=LightType::LIGHT_DIRECTIONAL){
            // position at origin in its own obj space
            sld.pos = ctm * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
        if (l->type!=LightType::LIGHT_POINT){
            sld.dir = ctm * l->dir;
        }

        if (l->type==LightType::LIGHT_SPOT){
            sld.penumbra = l->penumbra;
            sld.angle = l->angle;
        }
        sl_data.push_back(sld);
    }


    for (auto child: root->children){
        dfsSceneGraph(child, rs_data, sl_data, ctm);
    }
}
