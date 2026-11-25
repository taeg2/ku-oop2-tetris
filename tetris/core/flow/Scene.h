#pragma once
#include <memory>
#include "flow/GameServices.h"
#include "ui/Command.h"
#include "ui/Console.h"

// 장면(게임 상태 머신의 한 상태)의 추상 클래스.
// 장면은 다음 장면을 직접 만들어 changeTo()로 SceneManager에 전환을 요청한다.
class Scene {
public:
    explicit Scene(GameServices& services) : m_services(services) {}
    virtual ~Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    virtual void onEnter();
    virtual void handle(Command command) = 0;
    virtual void update(int dtMs) = 0;
    virtual void render(Console& console) const = 0;

protected:
    void changeTo(std::unique_ptr<Scene> next);
    void quit();

    GameServices& m_services;
};
