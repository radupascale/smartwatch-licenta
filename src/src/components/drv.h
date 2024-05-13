#pragma once
#include "Adafruit_DRV2605.h"
#include "components/component.h"
#include <Wire.h>

class DRV : Component
{
  private:
	Adafruit_DRV2605 *drv;

  public:
	DRV();
	~DRV();
	int init() override;
	void play(int effect);
};