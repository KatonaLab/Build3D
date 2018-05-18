#include "TurnTableCameraController.h"
#include <QtMath>
#include <algorithm>

using namespace std;

TurnTableCameraController::TurnTableCameraController(Qt3DCore::QNode *parent)
    : m_mouseDevice(new Qt3DInput::QMouseDevice()),
    m_mouseHandler(new Qt3DInput::QMouseHandler()),
    m_camera(nullptr),
    m_linearSpeed(1.0),
    m_lookSpeed(180),
    m_viewPortSize(QSize(256, 256)), // for no particular reason
    m_rollBallRadius(90), // should be smaller than the viewport half
    m_zoomMin(0.1),
    m_zoomMax(3),
    m_zoomRate(0.2),
    m_upVector(QVector3D(0.0f, 1.0f, 0.0f))
{
    // calc m_coordMultiplier and m_coordCenter
    setViewPortSize(m_viewPortSize);

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

    QObject::connect(m_mouseHandler, &Qt3DInput::QMouseHandler::doubleClicked, this,
        [this] (Qt3DInput::QMouseEvent*)
        {
            if (m_camera) {
                m_camera->viewAll();
            }
        });
}

void TurnTableCameraController::handleMouseEvent(QPoint prev, QPoint current)
{
    if (!m_camera) {
        return;
    }

    // TODO: no need to recalculate the prev point details, store it
    // somehow and use that

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
    if (!m_camera) {
        return;
    }

    QVector3D v = m_camera->position() - m_camera->viewCenter();
    float factor = max(1.0 - angleDelta / 120. * m_zoomRate, 0.0);
    QVector3D offset = factor * v;

    if (offset.length() <= m_zoomMin) {
        offset = v.normalized() * m_zoomMin;
    }

    if (offset.length() > m_zoomMax) {
        offset = v.normalized() * m_zoomMax;
    }
    
    m_camera->setPosition(m_camera->viewCenter() + offset);
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

float TurnTableCameraController::zoomMin() const
{
    return m_zoomMin;
}

float TurnTableCameraController::zoomMax() const
{
    return m_zoomMax;
}

float TurnTableCameraController::zoomRate() const
{
    return m_zoomRate;
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

void TurnTableCameraController::setZoomMin(float zmin)
{
    if (m_zoomMin != zmin) {
        m_zoomMin = zmin;
        Q_EMIT zoomMinChanged();
    }
}

void TurnTableCameraController::setZoomMax(float zmax)
{
    if (m_zoomMax != zmax) {
        m_zoomMax = zmax;
        Q_EMIT zoomMaxChanged();
    }
}

void TurnTableCameraController::setZoomRate(float zrate)
{
    if (m_zoomRate != zrate) {
        m_zoomRate = zrate;
        Q_EMIT zoomRateChanged();
    }
}
