// #include "type.h"
#include "sysCall.h"

// bool ustrcmp(const char *s1, const char *s2)
// {
//     for (; *s1 != '\0' && *s2 != '\0'; s1++, s2++)
//     {
//         if ((*s1 - *s2) != 0)
//             return false;
//     }

//     if (*s1 == '\0' && *s2 == '\0')
//     {
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }


// static inline void unoneCmd(const char *input)
// {
//     uartPuts("Unknown command: ");
//     uartPuts(input);
//     uartPuts("\n");
// }

// void uprocessCmd(const char *input)
// {
//     if (input[0] == '\0')
//         return;

//     if (strcmp(input, "cmd1"))
//         testCmd1();
//     else if (strcmp(input, "cmd1"))
//         testCmd2();
//     else 
//         noneCmd();
// }

// static inline void uhelloMessage()
// {
//     uartPuts("\n|------------------------|\n");
//     uartPuts("| Welcome to OSDI shell! |\n");
//     uartPuts("|------------------------|\n\n");
//     uartPuts("> ");
// }

// void umain()
// {
//     char buf[512];
//     uint32_t buf_ptr = 0;

//     uhelloMessage();

//     // echo everything back
//     while (1)
//     {
//         char in_c = uartGetc();
//         uartSend(in_c);

//         if (in_c == '\n')
//         {
//             buf[buf_ptr] = '\0';

//             processCmd(buf);
//             uartPuts("> ");

//             buf_ptr = 0;
//             buf[buf_ptr] = '\0';
//         }
//         else
//         {
//             buf[buf_ptr++] = in_c;
//         }
//     }
// }

int main()
{
    userTest();

    return 0;
}