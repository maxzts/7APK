#define _SCL_SECURE_NO_WARNINGS
#include <cstdio>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <iostream>
#include <string>
#include <cstdlib>


using namespace std;

//������������ ������ ������� ��� �������� ������/������
const int MAX_TIME_OUT = 1000;

void COM1(char* path);														// COM1 - ������� ����������
void COM2();																// COM2 - ��������� ����������

int main(int argc, char* argv[])
{
	switch (argc)
	{
	case 1:																	// ���� ������� ������� �� ������
		COM1(argv[0]);
		break;
	default:
		COM2();
		break;
	}
}

void COM1(char* path)
{
	string name = "COM1";
	STARTUPINFO si;															// ��������� ��� �������� �������� �������.
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	PROCESS_INFORMATION COM2_PROC_INFO;									// ��������� ��� �������� �������� COM2.
	ZeroMemory(&COM2_PROC_INFO, sizeof(COM2_PROC_INFO));

	HANDLE Receiver_COM1;
	HANDLE Semaphores[3];

	char buffer[20];			//������� ����� ����� �� ������, � ������� ���������� �� ����� �� 20 ��������
	int bufferSize = sizeof(buffer);

	string message;

	cout << "-----------------COM1-----------------\n\n";

	Semaphores[0] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE");				// ������� ����������.
	Semaphores[1] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_END");			// ������� ����������.
	Semaphores[2] = CreateSemaphore(NULL, 0, 1, "SEMAPHORE_EXIT");			// ������� ������.

	Receiver_COM1 = CreateFile(
		name.c_str(),														// ��� ������������ �����.
		GENERIC_READ | GENERIC_WRITE,										// ��� ������� � �����.
		0,																	// ��������� ����������� �������.
		NULL,																// �������� ������ �����.
		OPEN_EXISTING,														// ����� ������������.
		FILE_ATTRIBUTE_NORMAL,												// ����������� ����� ������.
		NULL																// ��������� ����� �������.
	);

	CreateProcess(
		path,																// ��� ������������ ������.
		(LPSTR)" COM2",													    // ��������� ��������� ������.
		NULL,																// ����������� ��������� ������ ��� ������ ����������.
		NULL,																// ����������� ��������� ������ ��� ������� ������ ���������� �����������.
		FALSE,																// ���� ������������ �� �������� ������������� ������.
		CREATE_NEW_CONSOLE,													// ����� ������� �������� ����� ������� ������ ����, ����� ������������ ������������. 
		NULL,																// ��������� �� ���� �����. ���� ����� ��� ������ ���������� ���=�������� � ���� ����� � ������� ����������.
		NULL,																// ��������� ������� ���� � �������.
		&si,																// ������������ ��� ��������� ������� ��������, �������� ������������ ���� � ���������.
		&COM2_PROC_INFO														// ��������� PROCESS_INFORMATION � ����������� � ��������. ����� ��������� Windows.
	);

	SetCommMask(Receiver_COM1, EV_RXCHAR);									// ������������� ����� �� ������� �����.
	SetupComm(Receiver_COM1, 1500, 1500);									// �������������� ���������������� ��������� ��� ��������� ���������� (����������, ����� �����-������)

	COMMTIMEOUTS CommTimeOuts;								// ���������, ��������������� ��������� ��������� ����������������� �����.
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;			// M����������� ����� ��� ��������� ����� ������������ ���� �������� �� ����� �����.
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;			// ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
	CommTimeOuts.ReadTotalTimeoutConstant = MAX_TIME_OUT;	// ���������, ������������, ����� ��������� ������ (������������) ������ ������� ������� ��� �������� ������.
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;			// ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.
	CommTimeOuts.WriteTotalTimeoutConstant = MAX_TIME_OUT;	// ���������, ������������, ����� ��������� ������ ������ ������� ������� ��� �������� ������.

	if (!SetCommTimeouts(Receiver_COM1, &CommTimeOuts))
	{
		CloseHandle(Receiver_COM1);
		Receiver_COM1 = INVALID_HANDLE_VALUE;
		return;
	}

	DCB COM_DCB;										// ���������, ��������������� �������� ��������� ����������������� �����. 
	memset(&COM_DCB, 0, sizeof(COM_DCB));				// ��������� ������ ��� ���������.
	COM_DCB.DCBlength = sizeof(DCB);					// ������ �����, � ������, ���������.
	GetCommState(Receiver_COM1, &COM_DCB);				// ��������� ������ � ������� ���������� ����������� �������� ��� ���������� ����������.
	COM_DCB.BaudRate = DWORD(9600);						// �������� �������� ������.
	COM_DCB.ByteSize = 8;								// ���������� ����� �������������� ��� � ������������ � ����������� ������.
	COM_DCB.Parity = NOPARITY;							// ���������� ����� ����� �������� �������� (��� ��������� �����������).
	COM_DCB.StopBits = ONESTOPBIT;						// ������ ���������� �������� ��� (���� ���). 
	COM_DCB.fAbortOnError = TRUE;						// ������ ������������� ���� �������� ������/������ ��� ������������� ������.
	COM_DCB.fDtrControl = DTR_CONTROL_DISABLE;			// ������ ����� ���������� ������� ��� ������� DTR.
	COM_DCB.fRtsControl = RTS_CONTROL_DISABLE;			// ������ ����� ���������� ������� ��� ������� RTS.
	COM_DCB.fBinary = TRUE;								// �������� �������� ����� ������.
	COM_DCB.fParity = FALSE;							// �������� ����� �������� ��������.
	COM_DCB.fInX = FALSE;								// ������ ������������� XON/XOFF ���������� ������� ��� ������.
	COM_DCB.fOutX = FALSE;								// ������ ������������� XON/XOFF ���������� ������� ��� ��������.
	COM_DCB.XonChar = 0;								// ������ ������ XON ������������ ��� ��� ������, ��� � ��� ��������.
	COM_DCB.XoffChar = (unsigned char)0xFF;				// ������ ������ XOFF ������������ ��� ��� ������, ��� � ��� ��������.
	COM_DCB.fErrorChar = FALSE;							//������ ������, �������������� ��� ������ �������� � ��������� ���������.
	COM_DCB.fNull = FALSE;								// ��������� �� ������������� ������ �������� � ������� �������� �� ������ ���������� ����� ErrorChar.
	COM_DCB.fOutxCtsFlow = FALSE;						// �������� ����� �������� �� �������� CTS.
	COM_DCB.fOutxDsrFlow = FALSE;						// �������� ����� �������� �� �������� DSR.
	COM_DCB.XonLim = 128;								// ������ ����������� ����� �������� � �������� ������ ����� �������� ������� XON.
	COM_DCB.XoffLim = 128;								// ���������� ������������ ���������� ���� � �������� ������ ����� �������� ������� XOFF.

	//��������� ������
	if (!SetCommState(Receiver_COM1, &COM_DCB))
	{
		CloseHandle(Receiver_COM1);
		Receiver_COM1 = INVALID_HANDLE_VALUE;
		return;
	}

	// ���� �������� ���������
	cout << "For exit press [`]" << endl;
	while (true)
	{
		DWORD AmountOfBytes;

		cout << ">>> ";
		cin.clear();
		getline(cin, message);

		if (message == "`")
		{																	// ������� ������ �� ���������.
			ReleaseSemaphore(Semaphores[2], 1, NULL);						// ���������� �������� ������ (������ � ���������� ���������).
			WaitForSingleObject(COM2_PROC_INFO.hProcess, INFINITE);		// �������� ������� �� ��������� ��������.
			break;
		}

		ReleaseSemaphore(Semaphores[0], 1, NULL);							// ���������� �������� ��������� (������ � ���������� ���������).

		int AmountOfBlocks = message.size() / bufferSize + 1;				//���������� ������ ������������ ����
		WriteFile(Receiver_COM1, &AmountOfBlocks, sizeof(AmountOfBlocks), &AmountOfBytes, NULL);

		int size = message.size();
		WriteFile(Receiver_COM1, &size, sizeof(size), &AmountOfBytes, NULL);

		for (int i = 0; i < AmountOfBlocks; i++)
		{
			message.copy(buffer, bufferSize, i * bufferSize);
			if (!WriteFile(Receiver_COM1, buffer, bufferSize, &AmountOfBytes, NULL))
				cout << "Error.";
		}
		WaitForSingleObject(Semaphores[1], INFINITE);						// �������� �������� � ������������ ���������.
	}

	CloseHandle(Receiver_COM1);
	CloseHandle(Semaphores[0]);
	CloseHandle(Semaphores[1]);
	cout << "\n";
	system("pause");
	return;
}

void COM2()
{
	HANDLE Receiver_COM2;
	HANDLE Semaphores[3];

	char buffer[20];
	int bufferSize = sizeof(buffer);

	string message;
	string name = "COM2";

	bool successFlag;
	Semaphores[0] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE");
	Semaphores[1] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_END");
	Semaphores[2] = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "SEMAPHORE_EXIT");

	cout << "-----------------COM2-----------------\n\n";

	//���������� ����������
	Receiver_COM2 = CreateFile(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		successFlag = TRUE;
		DWORD AmountOfBytes;
		message.clear();

		int SemaphoreIndex = WaitForMultipleObjects(3, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (SemaphoreIndex == 2)
		{
			break;
		}

		int AmountOfBlocks;
		if (!ReadFile(Receiver_COM2, &AmountOfBlocks, sizeof(AmountOfBlocks), &AmountOfBytes, NULL)) break;

		int size;
		if (!ReadFile(Receiver_COM2, &size, sizeof(size), &AmountOfBytes, NULL)) break;

		for (int i = 0; i < AmountOfBlocks; i++)
		{
			successFlag = ReadFile(Receiver_COM2, buffer, bufferSize, &AmountOfBytes, NULL);
			if (!successFlag)
			{
				break;
			}
			message.append(buffer, bufferSize);
		}

		if (!successFlag)
		{
			break;
		}

		message.resize(size);

		for (int i = 0; i < size; i++)
		{
			cout << message[i];
		}
		cout << endl;
		ReleaseSemaphore(Semaphores[1], 1, NULL);
	}
	CloseHandle(Receiver_COM2);
	CloseHandle(Semaphores[0]);
	CloseHandle(Semaphores[1]);
	return;
}