/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/HardwareSupport/SpaceNavControl.hh>
#include <ACGL/HardwareSupport/SpaceNavPollInterface.hh>

using namespace ACGL;
using namespace ACGL::HardwareSupport;

SpaceNavControl::SpaceNavControl(Scene::GenericCamera *_cam, const glm::vec3 &_moveSensitivity, const glm::vec3 &_rotateSensitivity)
    : mCamera(_cam),
      mMoveSensitivity(_moveSensitivity),
      mRotateSensitivity(_rotateSensitivity)
{
    assert(mCamera);
}

void SpaceNavControl::update(float _elapsedSeconds)
{
    static bool firstRun = true;
    if (firstRun) {
        SpaceNavRegisterListener();
        firstRun = false;
    }

    SpaceNavEvent event;
    while ( SpaceNavPollEvent(event) > 0 )
    {
        if (event.type == movement)
        {
            //std::cout << "Motion event: " << std::endl;
            //std::cout << "  pos: " << event.x << " " << event.y << " " << event.z << std::endl;
            //std::cout << "  rot: " << event.rx << " " << event.ry << " " << event.rz << std::endl;

            glm::vec3 moveSpeed = (1.0f/256.0f) * _elapsedSeconds * glm::vec3(event.x, event.y, event.z) * mMoveSensitivity;
            mCamera->move(moveSpeed);

            float rotSpeed = (1.0f/1024.0f)* _elapsedSeconds;
            glm::mat3 ypr = glm::mat3(glm::yawPitchRoll(
                                           event.ry * rotSpeed * mRotateSensitivity.y,
                                          -event.rx * rotSpeed * mRotateSensitivity.x,
                                           event.rz * rotSpeed * mRotateSensitivity.z));
            mCamera->setRotationMatrix(ypr * mCamera->getRotationMatrix3());
        } else if (event.type == button) {
            //std::cout << "button ";
            //if (!event.pressed) std::cout << "un";
            //std::cout << "pressed: " << event.button << std::endl;
        } else {
            //std::cout << "unknown event " << std::endl;
        }
    }
}

SpaceNavControl::~SpaceNavControl()
{
    SpaceNavUnregisterListener();
}

