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
#include <libm/msgPackets/keyPacket/keyPacket.h>
#include "keyscan.h"




void PutASCII(TempRenderer *renderer){
    int msgCount = min(msgGetCount(), 10);
    for (int i = 0; i < msgCount; i++)
    {
        GenericMessagePacket *msg = msgGetMessage();
        if (msg == NULL)
            break;
        if(msg->Type == MessagePacketType::KEY_EVENT){
            KeyMessagePacket *keyMsg = (KeyMessagePacket*)msg->Data;
            bool IsLeftShiftPressed;
            bool IsRightShiftPressed;
            if(keyMsg->Type == KeyMessagePacketType::KEY_RELEASE){
                switch (keyMsg->Scancode){
                    case LeftShift:
                        IsLeftShiftPressed = true;
                        return;
                    case LeftShift + 0x80:
                        IsLeftShiftPressed = false;
                        return;
                    case RightShift:
                        IsRightShiftPressed = true;
                        return;
                    case RightShift + 0x80:
                        IsRightShiftPressed = false;
                        return;
                    case Enter:
                        renderer->Println();
                        return;
                    case Spacebar:
                        renderer->Print(' ');
                        return;
                    case Backspace:
                        renderer->ClearChar();
                        return;
                }

                char ASCII = Translate(keyMsg->Scancode, IsLeftShiftPressed | IsRightShiftPressed);

                if (ASCII != 0){
                    renderer->Print(ASCII);
                }
                msg->Free();
                _Free(msg);
            }
        }
    }
}

TempRenderer *render;

int main(){
    programSetPriority(2);

    ENV_DATA *env = getEnvData();
    render = new TempRenderer(env->globalFrameBuffer,env->globalFont);
    render->Clear(Colors.black);
    render->Println("Welcome to PxttpOS Termainal!");
    while(true){
        PutASCII(render);
    }
}
