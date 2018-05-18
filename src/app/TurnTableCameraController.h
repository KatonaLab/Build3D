#ifndef _TurnTableCameraController_h_
#define _TurnTableCameraController_h_

// TODO: forward declare
#include <Qt3DRender/QCamera>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QMouseEvent>
#include <Qt3DLogic/QFrameAction>

class TurnTableCameraController : public Qt3DCore::QEntity {
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(float linearSpeed READ linearSpeed WRITE setLinearSpeed NOTIFY linearSpeedChanged)
    Q_PROPERTY(float lookSpeed READ lookSpeed WRITE setLookSpeed NOTIFY lookSpeedChanged)
    Q_PROPERTY(QSize viewPortSize READ viewPortSize WRITE setViewPortSize NOTIFY viewPortSizeChanged)
    Q_PROPERTY(float rollBallRadius READ rollBallRadius WRITE setRollBallRadius NOTIFY rollBallRadiusChanged)
    Q_PROPERTY(float zoomMin READ zoomMin WRITE setZoomMin NOTIFY zoomMinChanged)
    Q_PROPERTY(float zoomMax READ zoomMax WRITE setZoomMax NOTIFY zoomMaxChanged)
    Q_PROPERTY(float zoomRate READ zoomRate WRITE setZoomRate NOTIFY zoomRateChanged)
public:
    explicit TurnTableCameraController(Qt3DCore::QNode *parent = nullptr);
    ~TurnTableCameraController() = default;
    Qt3DRender::QCamera *camera() const;
    float linearSpeed() const;
    float lookSpeed() const;
    QSize viewPortSize() const;
    float rollBallRadius() const;
    float zoomMin() const;
    float zoomMax() const;
    float zoomRate() const;
    void setCamera(Qt3DRender::QCamera *camera);
    void setLinearSpeed(float linearSpeed);
    void setLookSpeed(float lookSpeed);
    void setViewPortSize(QSize viewPortSize);
    void setRollBallRadius(float rollBallRadius);
    void setZoomMin(float zmin);
    void setZoomMax(float zmax);
    void setZoomRate(float zrate);
Q_SIGNALS:
    void cameraChanged();
    void linearSpeedChanged();
    void lookSpeedChanged();
    void viewPortSizeChanged();
    void rollBallRadiusChanged();
    void zoomMinChanged();
    void zoomMaxChanged();
    void zoomRateChanged();
protected:
    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QMouseHandler *m_mouseHandler;
    Qt3DRender::QCamera *m_camera;
    float m_linearSpeed;
    float m_lookSpeed;
    QSize m_viewPortSize;
    float m_rollBallRadius;
    float m_zoomMin;
    float m_zoomMax;
    float m_zoomRate;
    const QVector3D m_upVector;
    QVector2D m_coordMultiplier;
    QVector2D m_coordCenter;
    QPoint m_currentMousePos;
    Qt3DInput::QMouseEvent::Buttons m_currentButtonMode;
    void handleMouseEvent(QPoint prevPos, QPoint currentPos);
    void handleWheelEvent(float angleDelta);
};

#endif
