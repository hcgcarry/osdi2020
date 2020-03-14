#include "uart.h"
#define NULL ((void*)0)
/* command line */
char cmd[256];

/**
 * Redefine some control chars to handle CSI
 * \e[3~ = 1, delete
 * \e[D  = 2, cursor left 
 * \e[C  = 3, cursor right
 */
void getline()
{
    int i, cmdidx = 0, cmdlast = 0;
    char c;
    cmd[0] = 0;
    printf("\r(๑•̀ㅂ•́)و✧ ");
    while((c = uart_getc()) != '\n') {
        /* redefine CSI key sequence */
        if(c == 27) {
            c = uart_getc();
            if(c == '[') {
                c = uart_getc();
                if(c == 'C') c = 3;      // left
                else if(c == 'D') c = 2; // right
                else if(c == '3') {
                    c = uart_getc();
                    if(c == '~') c = 1;  // delete
                }
            }
        }
        /* handle backspace */
        if(c == 8 || c == 127) {
            if(cmdidx > 0) {
                cmdidx--;
                for(i = cmdidx; i < cmdlast ; i++) cmd[i] = cmd[i+1];
                cmdlast--;
            }
        }
        /* handle delete */
        else if(c == 1) {
            if(cmdidx < cmdlast) {
                for(i = cmdidx; i < cmdlast; i++) cmd[i] = cmd[i+1];
                cmdlast--;
            }
        }
        /* handle cursor left */
        else if(c == 2) {
            if(cmdidx > 0) cmdidx--;
        }
        /* handle cursor right */
        else if(c == 3) {
            if(cmdidx < cmdlast) cmdidx++;
        }
        else {
            /* handle invalid char & invalid cmd size*/
            if(c < ' ' || cmdlast >= sizeof(cmd) - 1) continue;
            /* insert char in the cmd string */
            if(cmdidx < cmdlast) {
                for(i = cmdlast; i > cmdidx; i--) cmd[i] = cmd[i-1];
            }
            cmdlast++;
            cmd[cmdidx++] = c;
        }
        cmd[cmdlast] = 0;
        /* show command lines, and move cursor after prefix string */
        printf("\r(๑•̀ㅂ•́)و✧ %s \r\e[%dC", cmd, cmdidx+9);
    }
    printf("\n");

}

void main()
{
    uart_init();
    printf("Enter mini-shell: \n");
    printf("  _   _        _  _          ____                     _  \n");
    printf(" | | | |  ___ | || |  ___   |  _ \\  __ _  ___  _ __  (_)\n");
    printf(" | |_| | / _ \\| || | / _ \\  | |_) |/ _` |/ __|| '_ \\ | |\n");
    printf(" |  _  ||  __/| || || (_) | |  _ <| (_| |\\__ \\| |_) || |\n");
    printf(" |_| |_| \\___||_||_| \\___/  |_| \\_\\\\__,_||___/| .__/ |_|\n");
    printf("                                              |_|       \n");
  

    printf("      .--..--..--..--..--..--.             \n");
    printf("    .' \\  (`._   (_)     _   \\             \n");
    printf("  .'    |  '._)         (_)  |             \n");
    printf("  \\ _.')\\      .----..---.   /             \n");
    printf("  |(_.'  |    /    .-\\-.  \\  |             \n");
    printf("  \\     0|    |   ( O| O) | o|             \n");
    printf("   |  _  |  .--.____.'._.-.  |             \n");
    printf("   \\ (_) | o         -` .-`  |             \n");
    printf("    |    \\   |`-._ _ _ _ _\\ /              \n");
    printf("    \\    |   |  `. |_||_|   |              \n");
    printf("    | o  |    \\_      \\     |     -.   .-. \n");
    printf("    |.-.  \\     `--..-'   O |     `.`-' .' \n");
    printf("  _.'  .' |     `-.-'      /-.__   ' .-'   \n");
    printf(".' `-.` '.|='=.='=.='=.='=|._/_ `-'.'      \n");
    printf("`-._  `.  |________/\\_____|    `-.'        \n");
    printf("   .'   ).| '=' '='\\/ '=' |                \n");
    printf("   `._.`  '---------------'                \n");
    printf("           //___\\   //___\\                 \n");
    printf("             ||       ||                   \n");
    printf("             ||_.-.   ||_.-.               \n");
    printf("            (_.--__) (_.--__)              \n");

    while(1) {
        getline();
    
    }
}
