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
#include <libm/IO/io.h>
#include <libm/msgPackets/keyPacket/keyPacket.h>

TempRenderer *render;

char *getInput(){
    char *input;
    GenericMessagePacket *msg = msgGetMessage();
    if(msg->Type == MessagePacketType::KEY_EVENT){
        KeyMessagePacket *keyMsg = (KeyMessagePacket*)msg;
        if(keyMsg->Type == KeyMessagePacketType::KEY_PRESSED){
            
            return input;
        }
    }
}

int main(){
    programSetPriority(2);

    ENV_DATA *env = getEnvData();
    render = new TempRenderer(env->globalFrameBuffer,env->globalFont);
    render->Clear(Colors.black);
    render->Println("Welcome to PxttpOS Termainal!");
    while(true){
        printf(">");
        char *input = getInput();
    }
}
