#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>
#include "global.h"


void initDisplay();
void screenRepaint();
void showWifi( bool = true );
void showMqtt( bool = true );
void showCount( bool = true );

#endif // DISPLAY_H