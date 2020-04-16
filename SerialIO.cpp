// SerialIO.cpp : Defines the entry point for the console application.
//
// Serial Port File I/O Demo Program
//
// FOR DEMO: Connect COM5: to PC with null modem cable
// 9600 Baud 8 data bits 1 stop bit 
// no parity and no flow control

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "winmm.lib")

BOOL ModifyCommSettings(HANDLE hComPort);
HANDLE hSerial;

const wchar_t* path = TEXT("C:/Users/harp.wav");

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    DWORD /*cBytes_out,*/ cBytes_in;
    DWORD dwMask;
    //char cBuffer_out[] = "\f\n    Hello Serial World!\n\rType something and watch it echo back\n\rCtrl C to exit\n\r";
    TCHAR cBuffer_in[80];
    // Display message on console
    printf("\nOpening COM5: Serial Port - Type ctrl C on other device to exit\n\r");
    // Open Serial Port COM5: for Read and Write
    hSerial = CreateFile(_T("COM5:"), GENERIC_READ /*| GENERIC_WRITE*/, 0, NULL, OPEN_EXISTING, 0, NULL);
    // Check for file open errors
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("file open errors\n");
        Sleep(4000);
        return 0;
    }
    // Modify Com Port settings (i.e. Baud Rate, #bits, parity etc)
    if (!ModifyCommSettings(hSerial)) {
        printf("com port settings errors\n");
        Sleep(4000);
        return 0;
    }

    // Set Communication event mask for WaitCommEvent for rxchar (recieve character) in buffer
    SetCommMask(hSerial, EV_RXCHAR | EV_ERR);

    
    cBuffer_in[0] = 0;
    // Read in characters, copy to console display and Echo
    // Loop until ctrl C is typed
    while (cBuffer_in[0] != 0x03) {
        // Wait for character in input buffer - events are more efficient than looping
        WaitCommEvent(hSerial, &dwMask, 0);
        cBytes_in = 1;

        // Play sound
        PlaySound(path, NULL, SND_FILENAME | SND_ASYNC);
        // Loop just in (rare) case more than one character is in UART input buffer
        while (cBytes_in != 0) {
            // Read back data any serial data and display
            if (ReadFile(hSerial, cBuffer_in, 79, &cBytes_in, NULL)) {
                if (cBytes_in == 0) break;
                // Display Data read back
                printf("%s", (char*)cBuffer_in);
                
                /*
                // Echo characters back to sender
                if (!WriteFile(hSerial, cBuffer_in, cBytes_in, &cBytes_out, NULL)) {
                    printf("\rfile write errors\n");
                    Sleep(4000);
                    return 0;
                }
                */
            }
        }
    }
    // Close File
    CloseHandle(hSerial);
    return 1;
}

BOOL   ModifyCommSettings(HANDLE hComPort)
{
    COMMTIMEOUTS ctos;
    DCB PortDCB;
    // Initialize the DCBlength member. 
    PortDCB.DCBlength = sizeof(DCB);
    // Get the default serial port settings DCB information.
    GetCommState(hSerial, &PortDCB);
    // Change the common DCB structure settings to modify serial port settings.
    PortDCB.BaudRate = 9600;              // Current baud 
    PortDCB.fBinary = TRUE;               // Binary mode; no EOF check 
    PortDCB.fParity = TRUE;               // Enable parity checking 
    PortDCB.fOutxCtsFlow = FALSE;         // No CTS output flow control 
    PortDCB.fOutxDsrFlow = FALSE;         // No DSR output flow control 
    PortDCB.fDtrControl = DTR_CONTROL_ENABLE; // DTR flow control type 
    PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity 
    PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx 
    PortDCB.fOutX = FALSE;                // No XON/XOFF out flow control 
    PortDCB.fInX = FALSE;                 // No XON/XOFF in flow control 
    PortDCB.fErrorChar = FALSE;           // Disable error replacement 
    PortDCB.fNull = FALSE;                // Disable null stripping 
    PortDCB.fRtsControl = RTS_CONTROL_ENABLE; // RTS flow control 
    PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on error
    PortDCB.ByteSize = 8;                 // Number of bits/byte, 4-8 
    PortDCB.Parity = NOPARITY;            // 0-4=no,odd,even,mark,space 
    PortDCB.StopBits = ONESTOPBIT;        // 0,1,2 = 1, 1.5, 2 
    // Configure the port settings according to the new specifications  
    // of the DCB structure.
    if (!SetCommState(hSerial, &PortDCB)) {
        printf("Unable to configure the serial port");
        Sleep(4000);
        return false;
    }
    // Set read time outs 
    ctos.ReadIntervalTimeout = MAXDWORD;
    ctos.ReadTotalTimeoutMultiplier = MAXDWORD;
    ctos.ReadTotalTimeoutConstant = 1;
    ctos.WriteTotalTimeoutMultiplier = 0;
    ctos.WriteTotalTimeoutConstant = 0;
    if (!SetCommTimeouts(hSerial, &ctos)) {
        printf("Unable to configure the serial port");
        Sleep(4000);
        return false;
    }
    return true;
}
