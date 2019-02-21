#include "EnemyManager.h"
#include "Game.h"
#include "MD2Model.h"
#include "Enemies.h"
#include "glm/glm.hpp"


EnemyManager::EnemyManager()
{}


EnemyManager::~EnemyManager()
{}


void
EnemyManager::CreateEnemies(Game* TheGame) {
    glm::vec3 Pos;

    int total      = sizeof(Enemies);   // whats the total size
    int Individual = sizeof(Enemies[0]);// whats the individual size of a struct
    int entries    = total / Individual;

    GLuint PO = TheGame->MyObjects[1]->programObject;

    for(int i=0; i<entries; i++) {
        switch (Enemies[i].Type) {
            case TURRET: {// Create a Turret
                Pos = glm::vec3(Enemies[i].Xpos, Enemies[i].Ypos, Enemies[i].Zpos);
                MD2Model* En= new MD2Model(&TheGame->Handler,
                                           "Resources/Models/TurretDroid.md2",
                                           &TheGame->MainModelManager);
                En->SetPosition(Pos);
                En->StoreGraphicClass(TheGame->MyGraphics);
                TheGame->MyGraphics->Init(En);
                En->TheGame =TheGame;
                En->programObject = PO;
                En->LoadSkin("Resources/Textures/TurretDroid.png", &TheGame->MainModelManager); // he does not auto load a skin
                En->Scales = glm::vec3(0.18f); // original models are quite large
                En->TheModelManager->GetBoundingBoxes(En);
                En->MyPhysObj = TheGame->
                        CreatePhysicsObj(En->CreateMyShape(ObjectModel::CYLINDER),
                                         250, // mass 250kg
                                         btVector3(Pos.x, Pos.y, Pos.z), // position
                                         btQuaternion(0, 1, 0, 0));
                En->MyPhysObj->
                        GetRigidBody()->
                        getCollisionShape()->
                        setLocalScaling(btVector3(0.18f, 0.18f, 0.18f));
                En->MyPhysObj->
                        GetRigidBody()->
                        setUserPointer(static_cast<ObjectModel*>(En));
                En->WhatAmI = ObjectModel::e_GUARD;
                En->pAnimation->Update(TheGame->DeltaTimePassed);
                TheGame->MyObjects.push_back(En); // place on the system for possible updates/draws
                break;
            }// case TURRET
            case KNIGHT: {// Create a Droid
                Pos = glm::vec3(Enemies[i].Xpos, Enemies[i].Ypos, Enemies[i].Zpos);
                MD2Model* En = new MD2Model(&TheGame->Handler,
                                            "Resources/Models/knight.md2",
                                            &TheGame->MainModelManager);
                En->SetPosition(Pos);
                En->StoreGraphicClass(TheGame->MyGraphics);
                TheGame->MyGraphics->Init(En);
                En->TheGame = TheGame;
                En->programObject = PO;
                En->LoadSkin("Resources/Textures/knight.png", &TheGame->MainModelManager); // he does not auto load a skin
                En->Scales = glm::vec3(0.18f); // original models are quite large
                En->TheModelManager->GetBoundingBoxes(En);
                En->MyPhysObj = TheGame->
                        CreatePhysicsObj(En->CreateMyShape(ObjectModel::CYLINDER),
                                         250, // mass 250kg
                                         btVector3(Pos.x, Pos.y, Pos.z), // position}
                                         btQuaternion(0, 1, 0, 0));
                En->MyPhysObj->
                        GetRigidBody()->
                        getCollisionShape()->
                        setLocalScaling(btVector3(0.18f, 0.18f, 0.18f));
                En->MyPhysObj->
                        GetRigidBody()->
                        setUserPointer(static_cast<ObjectModel*>(En));
                En->WhatAmI = ObjectModel::e_GUARD;
                En->pAnimation->Update(TheGame->DeltaTimePassed);
                TheGame->MyObjects.push_back(En); // place on the system for possible updates/draws
                break;
            }// case KNIGHT
        case BATTLE: {// Create a BattleDroid
            Pos = glm::vec3(Enemies[i].Xpos, Enemies[i].Ypos, Enemies[i].Zpos);
            MD2Model* En = new MD2Model(&TheGame->Handler,
                                        "Resources/Models/BattleDroid.md2",
                                        &TheGame->MainModelManager);
            En->SetPosition(Pos);
            En->StoreGraphicClass(TheGame->MyGraphics);
            TheGame->MyGraphics->Init(En);
            En->TheGame = TheGame;
            En->programObject = PO;
            En->LoadSkin("Resources/Textures/BattleDroid.png", &TheGame->MainModelManager); // he does not auto load a skin
            En->Scales = glm::vec3(0.18f); // original models are quite large
            En->TheModelManager->GetBoundingBoxes(En);
            En->MyPhysObj = TheGame->
                    CreatePhysicsObj(En->CreateMyShape(ObjectModel::CYLINDER),
                                     250, // mass 250kg
                                     btVector3(Pos.x, Pos.y, Pos.z), // position}
                                     btQuaternion(0, 1, 0, 0));
            En->MyPhysObj->
                    GetRigidBody()->
                    getCollisionShape()->
                    setLocalScaling(btVector3(0.18f, 0.18f, 0.18f));
            En->MyPhysObj->
                    GetRigidBody()->
                    setUserPointer(static_cast<ObjectModel*>(En));
            En->WhatAmI = ObjectModel::e_GUARD;
            En->pAnimation->Update(TheGame->DeltaTimePassed);
            TheGame->MyObjects.push_back(En); // place on the system for possible updates/draws
            break;
        }// case BATTLE
        } // switch (Enemies[i].Type)
    } // for
}
