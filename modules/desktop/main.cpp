#include <libm/rendering/virtualRenderer.h>
#include <libm/cstrTools.h>
#include <libm/msgPackets/mousePacket/mousePacket.h>
#include <libm/msgPackets/keyPacket/keyPacket.h>
#include <libm/msgPackets/windowBufferUpdatePacket/windowBufferUpdatePacket.h>
#include <libm/queue/queue_windowBufferUpdate.h>
#include <libm/graphic/color.h>
#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>

int main(){
    ENV_DATA *env = getEnvData();
    TempRenderer *render = new TempRenderer(env->globalFrameBuffer,env->globalFont);
    render->Clear(Colors.black);
    render->Println("Welcome to PxttpOS Termainal!");
}
