#include "TurnTableCameraController.h"
#include <Qt3DRender/QCamera>

#include <iostream>

TurnTableCameraController::TurnTableCameraController(Qt3DCore::QNode *parent)
{}

TurnTableCameraController::~TurnTableCameraController()
{}

void TurnTableCameraController::moveCamera(
    const QAbstractCameraController::InputState &state,
    float dt)
{
    using namespace std;

    if ((state.leftMouseButtonActive || state.rightMouseButtonActive) == false) {
        return;
    }

    // #define PP(t) std::cout << #t << ": " << state.t << ", "
    // std::cout << "{";
    // PP(altKeyActive);
    // PP(leftMouseButtonActive);
    // PP(middleMouseButtonActive);
    // PP(rightMouseButtonActive);
    // PP(rxAxisValue);
    // PP(ryAxisValue);
    // PP(shiftKeyActive);
    // PP(txAxisValue);
    // PP(tyAxisValue);
    // PP(tzAxisValue);
    // std::cout << "dt: " << dt << ", ";
    // std::cout << "}\n";

    Qt3DRender::QCamera *theCamera = camera();
    if (theCamera == nullptr) {
        return;
    }

    const QVector3D upVector(0.0f, 1.0f, 0.0f);
    // auto clampInputs = [](float input1, float input2)
    // {
    //     float axisValue = input1 + input2;
    //     return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
    // };

    if (state.leftMouseButtonActive) {
        // Orbit
        // std::cout << dt << std::endl;
        theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, upVector);
        theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
        return;
    }

    if (state.rightMouseButtonActive) {
        std::cout << dt << std::endl;
        theCamera->translate(
            QVector3D(
                -1.0f * state.rxAxisValue * linearSpeed(),
                -1.0f * state.ryAxisValue * linearSpeed(),
                0.0f) * dt);
        return;
    }

    // const QVector3D upVector(0.0f, 1.0f, 0.0f);
    // // Mouse input
    // if (state.leftMouseButtonActive) {
    //     if (state.rightMouseButtonActive) {
    //         if (zoomDistance(camera()->position(), theCamera->viewCenter()) > d->m_zoomInLimit * d->m_zoomInLimit) {
    //             // Dolly up to limit
    //             theCamera->translate(QVector3D(0, 0, state.ryAxisValue), theCamera->DontTranslateViewCenter);
    //         } else {
    //             theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
    //         }
    //     } else {
    //         // Translate
    //         theCamera->translate(QVector3D(clampInputs(state.rxAxisValue, state.txAxisValue) * linearSpeed(),
    //         clampInputs(state.ryAxisValue, state.tyAxisValue) * linearSpeed(),
    //         0) * dt);
    //     }
    //     return;
    // }
    // else if (state.rightMouseButtonActive) {
    //     // Orbit
    //     theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, upVector);
    //     theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
    // }
    // // Keyboard Input
    // if (state.altKeyActive) {
    //     // Orbit
    //     theCamera->panAboutViewCenter((state.txAxisValue * lookSpeed()) * dt, upVector);
    //     theCamera->tiltAboutViewCenter((state.tyAxisValue * lookSpeed()) * dt);
    // } else if (state.shiftKeyActive) {
    //     if (zoomDistance(camera()->position(), theCamera->viewCenter()) > d->m_zoomInLimit * d->m_zoomInLimit) {
    //         // Dolly
    //         theCamera->translate(QVector3D(0, 0, state.tyAxisValue * linearSpeed() * dt), theCamera->DontTranslateViewCenter);
    //     } else {
    //         theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
    //     }
    // } else {
    //     // Translate
    //     theCamera->translate(QVector3D(clampInputs(state.leftMouseButtonActive ? state.rxAxisValue : 0, state.txAxisValue) * linearSpeed(),
    //     clampInputs(state.leftMouseButtonActive ? state.ryAxisValue : 0, state.tyAxisValue) * linearSpeed(),
    //     state.tzAxisValue * linearSpeed()) * dt);
    // }
}