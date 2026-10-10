#include "util.h"
#include "../graphics.h"
#include "../menus.h"

void drawCopyProgress(float progress)
{
    graphicsDrawLoadingBar(50, 350, progress);
    graphicsDrawTextCentered(310, COLOR_MUTED, "Copying save...");
    graphicsRender();
}
