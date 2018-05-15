#ifndef _TurnTableCameraController_h_
#define _TurnTableCameraController_h_

#include <Qt3DExtras/qabstractcameracontroller.h>

class TurnTableCameraController : public Qt3DExtras::QAbstractCameraController {
    Q_OBJECT
public:
    explicit TurnTableCameraController(Qt3DCore::QNode *parent = nullptr);
    ~TurnTableCameraController();
private:
    void moveCamera(const QAbstractCameraController::InputState &state, float dt) override;
};

#endif
