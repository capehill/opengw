#pragma once

#include "entity.hpp"

class entityProton : public entity
{
  public:
    entityProton(void);

    virtual void run();
    virtual void spawnTransition();

  private:
    void drawRing();
};
