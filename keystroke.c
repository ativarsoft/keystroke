#define WINVER 0x0500

#if !defined(_UNICODE)
#define _UNICODE
#endif
#if !defined(UNICODE)
#define UNICODE
#endif

#include <windows.h>
 
void PressKey(char VirtualKey, BOOL Shift)
{
	/* This structure will be used to create the keyboard
     * input event. */
    INPUT ip;

    /* Set up a generic keyboard event. */
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; /* hardware scan code for key */
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    if (Shift == TRUE) {
		ip.ki.wVk = VK_SHIFT;
		ip.ki.dwFlags = 0;
		SendInput(1, &ip, sizeof(INPUT));
	}
    /* Press the key */
    ip.ki.wVk = VirtualKey; /* virtual-key code for the key */
    ip.ki.dwFlags = 0; /* 0 for key press */
    SendInput(1, &ip, sizeof(INPUT));
    ip.ki.dwFlags = KEYEVENTF_KEYUP; /* KEYEVENTF_KEYUP for key release */
    SendInput(1, &ip, sizeof(INPUT));
    if (Shift) {
		ip.ki.wVk = VK_SHIFT;
		ip.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));
	}
}

void EnterText(LPTSTR Text, int Length)
{
	const TCHAR Offset = 'a' - 'A';
	int i;
	TCHAR c;

	for (i = 0; i < Length && (c = Text[i]); i++) {
		if (c >= '0' && c <= '9') {
			PressKey(c, FALSE);
		} else if (c >= 'A' && c <= 'Z') {
			PressKey(c, TRUE);
		} else if (c >= 'a' && c <= 'z') {
			PressKey(c - Offset, FALSE);
		} else if (c == ',') {
			PressKey(VK_OEM_COMMA, FALSE);
		} else if (c == '.') {
			PressKey(VK_OEM_PERIOD, FALSE);
		} else if (c == '\t') {
			PressKey(VK_TAB, FALSE);
		}
		/* Pause for 0.1 seconds. */
		Sleep(100);
	}
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	TCHAR Path[MAX_PATH] = {};
	OPENFILENAME ofn = {};
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD Size = 0;
	LPSTR Buffer = NULL;
	DWORD nBytesToRead = 0;
	DWORD nBytesRead = 0;
	BOOL bResult;
	int Length;
	LPTSTR Text;

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = TEXT("Arquivos de Texto\0*.txt\0\0");
	ofn.lpstrFile = Path;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = TEXT("txt");
	GetOpenFileName(&ofn);

	hFile = CreateFile
        (Path,
         GENERIC_READ,
         FILE_SHARE_READ,
         NULL,
         OPEN_EXISTING,
         FILE_ATTRIBUTE_NORMAL,
         NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("Não foi possível abrir o arquivo para leitura."), NULL, MB_OK | MB_ICONERROR);
		return 1;
	}
	Size = GetFileSize(hFile, NULL);
	if (Size <= 0) {
		MessageBox(NULL, TEXT("O arquivo está vazio."), NULL, MB_OK | MB_ICONERROR);
		CloseHandle(hFile);
		return 1;
	}
	Buffer = (LPSTR) GlobalAlloc(GMEM_ZEROINIT, Size);
	if (Buffer == NULL) {
		MessageBox(NULL, TEXT("Não foi possível reservar memória para o aquivo."), NULL, MB_OK | MB_ICONERROR);
		CloseHandle(hFile);
		return 1;
	}
	nBytesToRead = Size;
	bResult = ReadFile(hFile, Buffer, nBytesToRead, &nBytesRead, NULL);
	CloseHandle(hFile);

	if (bResult == FALSE || (nBytesToRead != nBytesRead)) {
		MessageBox(NULL, TEXT("Não foi possível ler o arquivo por completo."), NULL, MB_OK | MB_ICONERROR);
		return 1;
	}
	
#if defined UNICODE || define _UNICODE
	Length = MultiByteToWideChar(CP_UTF8, 0, Buffer, Size, 0, 0);
	Text = (LPTSTR) GlobalAlloc(GMEM_ZEROINIT, Length * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, Buffer, Size, Text, Length);
#else
	Text = Buffer;
	Length = Size;
#endif
	
	GlobalFree(Buffer);

    MessageBox(NULL,
        TEXT("O texto será digitado em 5 segundos. ")
        TEXT("Selecione a janela que receberá o texto."),
        TEXT("Prepare-se"),
        MB_OK | MB_ICONINFORMATION);
    Sleep(5000);
        
    EnterText(Text, Length);
    GlobalFree(Text);
    return 0;
}
