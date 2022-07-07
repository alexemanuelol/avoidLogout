/**
 * Copyright (C) 2022 Alexemanuelol - All Rights Reserved
 */

/**********************************
 *  Includes
 *********************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <limits.h>


/**********************************
 *  Defines
 *********************************/

#define OPTSTR                "k:i:rh"
#define DEFAULT_KEY_CODE      0x07 /* 0x07 is undefined key */
#define DEFAULT_INTERVAL      (60 * 8) /* Default 8 minutes */
#define DEFAULT_RESIZE_WINDOW TRUE
#define WINDOW_SIZE_W         280
#define WINDOW_SIZE_H         110


/**********************************
 *  External Declarations
 *********************************/

extern char *optarg;


/**********************************
 *  Typedefs
 *********************************/

typedef struct {
    WORD keyCode;
    uint16_t intervalSeconds;
    BOOL resizeWindow;
} options_t;


/**********************************
 *  Global Variable Declaration
 *********************************/


/**********************************
 *  Function Prototypes
 *********************************/

void usage(void);
uint8_t programExecute(options_t *options);
void clickKey(INPUT *input);
void getMonitorResolution(int *width, int *height);


/******************************************************************************
 *  Main
 *****************************************************************************/
int main(int argc, char *argv[])
{
    options_t options = {DEFAULT_KEY_CODE, DEFAULT_INTERVAL, DEFAULT_RESIZE_WINDOW};
    int opt;

    while ((opt = getopt(argc, argv, OPTSTR)) != EOF)
    {
        switch (opt)
        {
            case 'k': /* Key */
            {
                int lengthOfKeyCodeStr = (int) strlen(optarg);
                int keyCode;

                if (lengthOfKeyCodeStr > 2 && optarg[0] == '0' && (optarg[1] == 'x' || optarg[1] == 'X'))
                {
                    keyCode = (int) strtol(optarg, NULL, 0);
                }
                else
                {
                    keyCode = atoi(optarg);
                }

                if (keyCode < 0 || keyCode > USHRT_MAX)
                {
                    printf("Invalid Key-Code: %d\nValid Key-Codes in interval: 0x0 - 0xFFFF", keyCode);
                    exit(EXIT_FAILURE);
                }

                options.keyCode = (WORD) keyCode;
            }
            break;

            case 'i': /* Interval */
            {
                int lengthOfIntervalStr = (int) strlen(optarg);
                int interval;

                if (lengthOfIntervalStr > 2 && optarg[0] == '0' && (optarg[1] == 'x' || optarg[1] == 'X'))
                {
                    interval = (int) strtol(optarg, NULL, 0);
                }
                else
                {
                    interval = atoi(optarg);
                }

                if (interval <= 0 || interval > _UI16_MAX)
                {
                    printf("Invalid Key Press Interval: %d\nValid interval: 0x1 - 0xFFFF", interval);
                    exit(EXIT_FAILURE);
                }

                options.intervalSeconds = (uint16_t) interval;
            }
            break;

            case 'r': /* Ignore Resize of Terminal Window */
            {
                options.resizeWindow = FALSE;
            }
            break;

            case 'h': /* Help */
            {
                usage();
            }
            break;

            default:
            {
                usage();
            }
            break;
        }
    }

    if (programExecute(&options) != EXIT_SUCCESS)
    {
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

/******************************************************************************
 *  Usage Function
 *****************************************************************************/
void usage(void)
{
    printf("usage: avoidLogout.exe [-k key-code] [-i interval] [-r] [-h]\n\n"
    "Program is used to avoid getting logged out from your Windows machine\nby virtually clicking a keyboard key.\n\n"
    "Optional flags:\n"
    "  -k [KEY-CODE]   Key-Code for the key to be clicked (Default: 0x07).\n"
    "  -i [INTERVAL]   How often in seconds should the key be clicked (Default: 480s).\n"
    "  -r              Ignore resizing the window.\n"
    "  -h              Display this help message.\n");
    exit(EXIT_FAILURE);
}

/******************************************************************************
 *  Program Execute Function
 *****************************************************************************/
uint8_t programExecute(options_t *options)
{
    INPUT input = {
        .type = INPUT_KEYBOARD,
        .ki.wScan = 0,
        .ki.time = 0,
        .ki.dwExtraInfo = 0,
        .ki.wVk = options->keyCode
    };

    int screenWidth, screenHeight;
    getMonitorResolution(&screenWidth, &screenHeight);

    if (options->resizeWindow)
    {
        HWND window = GetConsoleWindow();
        MoveWindow(window, screenWidth - WINDOW_SIZE_W, 0, WINDOW_SIZE_W, WINDOW_SIZE_H, TRUE);
    }

    system("cls"); /* Clear Terminal */
    printf("Avoid Logout Program!\n  Key Press Interval: %ds\n  Key-Code: 0x%X\n",
        options->intervalSeconds, options->keyCode);

    uint16_t counter = 0;
    while (TRUE)
    {
        if (counter == 0)
        {
            clickKey(&input);
            printf("\33[2K\rCounter: CLICK!");
            counter = options->intervalSeconds;
        }
        else {
            printf("\33[2K\rCounter: %d", counter--);
        }

        fflush(stdout);
        sleep(1); /* Sleep for one second */
    }

    return EXIT_SUCCESS;
}

/******************************************************************************
 *  Click Keyboard Key
 *****************************************************************************/
void clickKey(INPUT *input)
{
    input->ki.dwFlags = 0; /* 0 for key press */
    SendInput(1, input, sizeof(INPUT));

    input->ki.dwFlags = KEYEVENTF_KEYUP; /* KEYEVENTF_KEYUP for key release */
    SendInput(1, input, sizeof(INPUT));
}

/******************************************************************************
 *  Get Monitor Resolution
 *****************************************************************************/
void getMonitorResolution(int *width, int *height)
{
    *width = GetSystemMetrics(SM_CXSCREEN);
    *height = GetSystemMetrics(SM_CYSCREEN);
}