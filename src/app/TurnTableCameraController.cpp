#include "TurnTableCameraController.h"
#include <QtMath>

TurnTableCameraController::TurnTableCameraController(Qt3DCore::QNode *parent)
    : m_mouseDevice(new Qt3DInput::QMouseDevice()),
    m_mouseHandler(new Qt3DInput::QMouseHandler()),
    m_camera(nullptr),
    m_upVector(QVector3D(0.0f, 1.0f, 0.0f))
{
    m_mouseHandler->setSourceDevice(m_mouseDevice);
    addComponent(m_mouseHandler);

    QObject::connect(m_mouseHandler, &Qt3DInput::QMouseHandler::pressed, this,
        [this] (Qt3DInput::QMouseEvent *mouse)
        {
            m_currentMousePos = QPoint(mouse->x(), mouse->y());
            m_currentButtonMode = mouse->button();
        });

    QObject::connect(m_mouseHandler, &Qt3DInput::QMouseHandler::released, this,
        [this] (Qt3DInput::QMouseEvent *mouse)
        {
            m_currentMousePos = QPoint(mouse->x(), mouse->y());
            m_currentButtonMode = Qt3DInput::QMouseEvent::Buttons::NoButton;
        });

    QObject::connect(m_mouseHandler, &Qt3DInput::QMouseHandler::positionChanged, this,
        [this] (Qt3DInput::QMouseEvent *mouse)
        {
            QPoint pos(mouse->x(), mouse->y());
            handleMouseEvent(m_currentMousePos, pos);
            m_currentMousePos = pos;
        });

    QObject::connect(m_mouseHandler, &Qt3DInput::QMouseHandler::wheel, this,
        [this] (Qt3DInput::QWheelEvent *wheel)
        {
            handleWheelEvent(wheel->angleDelta().y());
        });
}

void TurnTableCameraController::handleMouseEvent(QPoint prev, QPoint current)
{
    if (!m_camera) {
        return;
    }

    auto posDetails = [this](QVector2D p)
    {
        QVector2D r = p - m_coordCenter;
        bool inRollBall = r.length() < m_rollBallRadius;
        float rollAngle = qRadiansToDegrees(qAtan2(r.y(), r.x()));
        QVector2D rollBallEdge = m_coordCenter + r.normalized() * m_rollBallRadius;
        QVector2D v;
        if (inRollBall) {
            v = p * m_coordMultiplier * m_lookSpeed;
        } else {
            v = rollBallEdge * m_coordMultiplier * m_lookSpeed;
        }
        return QVector4D(v.x(), v.y(), rollAngle, inRollBall);
    };

    switch (m_currentButtonMode) {
        case Qt3DInput::QMouseEvent::LeftButton: {
            QVector4D prevDetails = posDetails(QVector2D(prev));
            QVector4D currDetails = posDetails(QVector2D(current));

            if (currDetails.w() > 0.5) {
                m_camera->panAboutViewCenter(- currDetails.x() + prevDetails.x());
                m_camera->tiltAboutViewCenter(currDetails.y() - prevDetails.y());
            } else {
                m_camera->rollAboutViewCenter(currDetails.z() - prevDetails.z());
            }
        } break;
        case Qt3DInput::QMouseEvent::RightButton:
            qDebug() << "right button";
            break;
        case Qt3DInput::QMouseEvent::MiddleButton:
            qDebug() << "middle button";
            break;
        case Qt3DInput::QMouseEvent::BackButton:
            qDebug() << "back button";
            break;
        case Qt3DInput::QMouseEvent::NoButton:
            qDebug() << "no button";
            break;
    }
}

void TurnTableCameraController::handleWheelEvent(float angleDelta)
{

}

Qt3DRender::QCamera* TurnTableCameraController::camera() const
{
    return m_camera;
}

float TurnTableCameraController::linearSpeed() const
{
    return m_linearSpeed;
}

float TurnTableCameraController::lookSpeed() const
{
    return m_lookSpeed;
}

QSize TurnTableCameraController::viewPortSize() const
{
    return m_viewPortSize;
}

float TurnTableCameraController::rollBallRadius() const
{
    return m_rollBallRadius;
}

void TurnTableCameraController::setCamera(Qt3DRender::QCamera *camera)
{
    if (m_camera != camera) {
        m_camera = camera;
        Q_EMIT cameraChanged();
    }
}

void TurnTableCameraController::setLinearSpeed(float linearSpeed)
{
    if (m_linearSpeed != linearSpeed) {
        m_linearSpeed = linearSpeed;
        Q_EMIT linearSpeedChanged();
    }
}

void TurnTableCameraController::setLookSpeed(float lookSpeed)
{
    if (m_lookSpeed != lookSpeed) {
        m_lookSpeed = lookSpeed;
        Q_EMIT lookSpeedChanged();
    }
}

void TurnTableCameraController::setViewPortSize(QSize viewPortSize)
{
    if (m_viewPortSize != viewPortSize) {
        m_viewPortSize = viewPortSize;
        m_coordMultiplier = QVector2D(1.0 / viewPortSize.width(), 1.0 / viewPortSize.height());
        m_coordCenter = QVector2D(0.5 * viewPortSize.width(), 0.5 * viewPortSize.height());
        Q_EMIT viewPortSizeChanged();
    }
}

void TurnTableCameraController::setRollBallRadius(float rollBallRadius)
{
    if (m_rollBallRadius != rollBallRadius) {
        m_rollBallRadius = rollBallRadius;
        Q_EMIT rollBallRadiusChanged();
    }
}

// void TurnTableCameraController::moveCamera(
//     const QAbstractCameraController::InputState &state,
//     float dt)
// {
//     using namespace std;

//     if ((state.leftMouseButtonActive || state.rightMouseButtonActive) == false) {
//         return;
//     }

//     // #define PP(t) std::cout << #t << ": " << state.t << ", "
//     // std::cout << "{";
//     // PP(altKeyActive);
//     // PP(leftMouseButtonActive);
//     // PP(middleMouseButtonActive);
//     // PP(rightMouseButtonActive);
//     // PP(rxAxisValue);
//     // PP(ryAxisValue);
//     // PP(shiftKeyActive);
//     // PP(txAxisValue);
//     // PP(tyAxisValue);
//     // PP(tzAxisValue);
//     // std::cout << "dt: " << dt << ", ";
//     // std::cout << "}\n";

//     Qt3DRender::QCamera *theCamera = camera();
//     if (theCamera == nullptr) {
//         return;
//     }

//     const QVector3D upVector(0.0f, 1.0f, 0.0f);
//     // auto clampInputs = [](float input1, float input2)
//     // {
//     //     float axisValue = input1 + input2;
//     //     return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
//     // };

//     if (state.leftMouseButtonActive) {
//         // Orbit
//         // std::cout << dt << std::endl;
//         theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, upVector);
//         theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
//         return;
//     }

//     if (state.rightMouseButtonActive) {
//         // std::cout << dt << std::endl;
//         theCamera->translate(
//             QVector3D(
//                 -1.0f * state.rxAxisValue * linearSpeed(),
//                 -1.0f * state.ryAxisValue * linearSpeed(),
//                 0.0f) * dt);
//         return;
//     }

//     // const QVector3D upVector(0.0f, 1.0f, 0.0f);
//     // // Mouse input
//     // if (state.leftMouseButtonActive) {
//     //     if (state.rightMouseButtonActive) {
//     //         if (zoomDistance(camera()->position(), theCamera->viewCenter()) > d->m_zoomInLimit * d->m_zoomInLimit) {
//     //             // Dolly up to limit
//     //             theCamera->translate(QVector3D(0, 0, state.ryAxisValue), theCamera->DontTranslateViewCenter);
//     //         } else {
//     //             theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
//     //         }
//     //     } else {
//     //         // Translate
//     //         theCamera->translate(QVector3D(clampInputs(state.rxAxisValue, state.txAxisValue) * linearSpeed(),
//     //         clampInputs(state.ryAxisValue, state.tyAxisValue) * linearSpeed(),
//     //         0) * dt);
//     //     }
//     //     return;
//     // }
//     // else if (state.rightMouseButtonActive) {
//     //     // Orbit
//     //     theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, upVector);
//     //     theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
//     // }
//     // // Keyboard Input
//     // if (state.altKeyActive) {
//     //     // Orbit
//     //     theCamera->panAboutViewCenter((state.txAxisValue * lookSpeed()) * dt, upVector);
//     //     theCamera->tiltAboutViewCenter((state.tyAxisValue * lookSpeed()) * dt);
//     // } else if (state.shiftKeyActive) {
//     //     if (zoomDistance(camera()->position(), theCamera->viewCenter()) > d->m_zoomInLimit * d->m_zoomInLimit) {
//     //         // Dolly
//     //         theCamera->translate(QVector3D(0, 0, state.tyAxisValue * linearSpeed() * dt), theCamera->DontTranslateViewCenter);
//     //     } else {
//     //         theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
//     //     }
//     // } else {
//     //     // Translate
//     //     theCamera->translate(QVector3D(clampInputs(state.leftMouseButtonActive ? state.rxAxisValue : 0, state.txAxisValue) * linearSpeed(),
//     //     clampInputs(state.leftMouseButtonActive ? state.ryAxisValue : 0, state.tyAxisValue) * linearSpeed(),
//     //     state.tzAxisValue * linearSpeed()) * dt);
//     // }
// }