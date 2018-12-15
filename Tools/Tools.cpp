#include "pch.h"
#include "resource.h"
#include "Common/Canvas.h"

HWND hWnd;
HINSTANCE hInst;
ImageMatrix logo;

inline void disableButtons()
{
	EnableWindow(GetDlgItem(hWnd, IDC_PACK), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_UNPACK), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_IMGGLUING), FALSE);
}

inline void enableButtons()
{
	EnableWindow(GetDlgItem(hWnd, IDC_PACK), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_UNPACK), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_IMGGLUING), TRUE);
}

using namespace std::filesystem;
std::thread task;
bool userClosed = false;	// if user closed the window, do not update window controls when tasks finished
bool generatePack(const std::wstring& destFile, const path& resDirectory);
void deployPack(const std::wstring& destFile, const path& resDirectory)
{
	SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)L"Generating...");
	disableButtons();
	if (task.joinable()) task.join();
	task = std::thread([=]() {
		if (generatePack(destFile, resDirectory))
			userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
			(LPARAM)L"The resource file was successfully generated.");
		else
			userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
			(LPARAM)L"An error occured when packing resource folder.");
		userClosed ? 0 : enableButtons();
	});
}

bool extractPack(const path& resFile);
void deployUnpack(const path& resFile)
{
	SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)L"Extracting...");
	disableButtons();
	if (task.joinable()) task.join();
	task = std::thread([=]() {
		if (extractPack(resFile))
			userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
			(LPARAM)L"The resource file was successfully unpacked.");
		else
			userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
			(LPARAM)L"Unsupported resource package.");
		userClosed ? 0 : enableButtons();
	});
}

bool imageConcatenate(const std::wstring& destImage, const std::vector<std::wstring>& imgFiles);
void deployImageContatenate(const std::wstring& destImage, const std::vector<std::wstring>& imgFiles)
{
	SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)L"Concatenating...");
	disableButtons();
	if (task.joinable()) task.join();
	task = std::thread([](const std::wstring& destImage, const std::vector<std::wstring>& imgFiles) {
		if (imageConcatenate(destImage, imgFiles))
			userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
			(LPARAM)L"Selected images were successfully concatenated.");
		else
			userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
			(LPARAM)L"Failed to concatenate.");
		userClosed ? 0 : enableButtons();
	}, destImage, imgFiles);
}

void dialogInit()
{
	// X Y Center
	RECT rect; GetWindowRect(hWnd, &rect);
	int xPos = GetSystemMetrics(SM_CXSCREEN) - rect.right + rect.left;
	int yPos = GetSystemMetrics(SM_CYSCREEN) - rect.bottom + rect.top;
	SetWindowPos(hWnd, NULL, xPos / 2, yPos / 2, 0, 0, SWP_NOSIZE);

	// More details for using dialog controls, visit:
	// https://docs.microsoft.com/en-us/windows/desktop/Controls/individual-control-info

	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));
	PostMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	PostMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	DestroyIcon(hIcon);

	logo = ImageMatrixFactory::fromPngResource(IDB_LOGO, L"PNG", hInst);
	ImageMatrix temp = ImageMatrixFactory::createBufferImage(logo.getWidth() / 2, logo.getHeight() / 2, 0xFFF0F0F0);
	PiCanvas::blend(temp, logo, 0, 0, logo.getWidth() / 2, logo.getHeight() / 2, 0, 0, logo.getWidth(), logo.getHeight(), 0xFF);
	logo = std::move(temp);
}

inline void setOFN(OPENFILENAME& ofn, LPCTSTR lpstrFilter, LPCTSTR lpstrDefExt)
{
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = lpstrFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = lpstrDefExt;
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
}

#include <shlobj_core.h>
void onPack()
{
	TCHAR tempPath[MAX_PATH];
	*tempPath = L'\0';
	BROWSEINFO bi;
	bi.pidlRoot = NULL;
	bi.hwndOwner = hWnd;
	bi.pszDisplayName = tempPath;
	bi.lpszTitle = L"Select Resouce Folder";
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = NULL;
	LPITEMIDLIST pIDList = SHBrowseForFolderW(&bi);
	if (pIDList <= 0)
		return;

	SHGetPathFromIDListW(pIDList, tempPath);
	CoTaskMemFree(pIDList);

	path root(tempPath);
	if (!exists(root))
		return;

	*tempPath = L'\0';
	OPENFILENAME ofn;
	ofn.lpstrFile = tempPath;
	ofn.nMaxFile = sizeof(tempPath);
	setOFN(ofn, L"resource package (*.res)\0*.res\0\0", L"res");
	if (GetSaveFileName(&ofn) <= 0)
		return;

	deployPack(tempPath, root);
}

void onUnpack()
{
	TCHAR tempPath[MAX_PATH];
	*tempPath = L'\0';

	OPENFILENAME ofn;
	ofn.lpstrFile = tempPath;
	ofn.nMaxFile = sizeof(tempPath);
	setOFN(ofn, L"resource package (*.res)\0*.res\0\0", L"res");

	if (GetOpenFileName(&ofn) <= 0)
		return;

	deployUnpack(tempPath);
}

void onImageConcatenate()
{
	TCHAR tempPath[MAX_PATH * 100];		// max 100 files
	*tempPath = L'\0';

	OPENFILENAME ofn;
	ofn.lpstrFile = tempPath;
	ofn.nMaxFile = sizeof(tempPath);
	setOFN(ofn, L"Image File (*.jpg; *.png)\0*.jpg;*.png;*.jpeg\0\0", NULL);
	ofn.Flags |= OFN_ALLOWMULTISELECT;

	if (GetOpenFileName(&ofn) <= 0)
		return;

	std::vector<std::wstring> file_list;
	std::wstring baseDir(tempPath, ofn.nFileOffset - 1);
	for (TCHAR* p = tempPath + ofn.nFileOffset; *p; ++p) {
		std::wstring file_name = p;
		file_list.push_back(baseDir + L'\\' + file_name);
		p += file_name.size();
	}

	if (++file_list.begin() == file_list.end()) {
		MessageBox(hWnd, L"Concatenate at least two images", L"Error", MB_OK | MB_ICONWARNING);
		return;
	}

	*tempPath = L'\0';
	ofn.lpstrFile = tempPath;
	ofn.nMaxFile = MAX_PATH;
	setOFN(ofn, L"PNG File (*.png)\0*.png\0JPEG File (*.jpg)\0*.jpg\0\0", L"png\0jpg");
	ofn.Flags &= ~OFN_ALLOWMULTISELECT;
	if (GetSaveFileName(&ofn) <= 0)
		return;

	deployImageContatenate(tempPath, file_list);
}

#include "Common/Window.h"
void dragFiles(HDROP hDrop)
{
	TCHAR tempPath[MAX_PATH] = { '\0' };

	UINT NumOfFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
	if (NumOfFiles == 1)
	{
		DragQueryFile(hDrop, 0, tempPath, MAX_PATH);
		DragFinish(hDrop);
		path file(tempPath);
		if (is_directory(file))	// pack resource
		{
			*tempPath = L'\0';
			OPENFILENAME ofn;
			ofn.lpstrFile = tempPath;
			ofn.nMaxFile = sizeof(tempPath);
			setOFN(ofn, L"resource package (*.res)\0*.res\0\0", L"res");
			if (GetSaveFileName(&ofn) <= 0)
				return;
			deployPack(tempPath, file);
		}
		else // unpack resource
		{
			// check resource format
			std::size_t p = file.wstring().rfind(L'.');
			if (p == std::wstring::npos) {
				MessageBox(hWnd, L"Unknown resource format", L"Error", MB_OK | MB_ICONWARNING);
				return;
			}
			if (file.wstring().substr(p + 1) == L"res")
				deployUnpack(file);
			else {
				MessageBox(hWnd, L"Only Actinidia resource package is supported", L"Error", MB_OK | MB_ICONWARNING);
				return;
			}
		}
	}
	else 
	{
		// image concatenate
		std::vector<std::wstring> file_list;
		for (UINT i = 0; i < NumOfFiles; i++)
		{
			DragQueryFile(hDrop, i, tempPath, MAX_PATH);
			file_list.push_back(tempPath);

			// Check file format
			std::size_t p = file_list.back().rfind(L'.');
			if (p == std::wstring::npos) {
				MessageBox(hWnd, L"Unknown image format", L"Error", MB_OK | MB_ICONWARNING);
				DragFinish(hDrop);
				return;
			}
			std::wstring extName = file_list.back().substr(p + 1);
			
			if (extName == L"jpg" || extName == L"jpeg" || extName == L"png" ||
				extName == L"JPG" || extName == L"JPEG" || extName == L"PNG")
				continue;
			else {
				MessageBox(hWnd, L"Only image files are supported", L"Error", MB_OK | MB_ICONWARNING);
				DragFinish(hDrop);
				return;
			}
		}
		DragFinish(hDrop);

		*tempPath = L'\0';
		OPENFILENAME ofn;
		ofn.lpstrFile = tempPath;
		ofn.nMaxFile = sizeof(tempPath);
		setOFN(ofn, L"PNG File (*.png)\0*.png\0JPEG File (*.jpg)\0*.jpg\0\0", L"png\0jpg");
		ofn.Flags &= ~OFN_ALLOWMULTISELECT;
		if (GetSaveFileName(&ofn) <= 0)
			return;
		deployImageContatenate(tempPath, file_list);
	}
}

//---------------------------------------------

INT_PTR CALLBACK MainDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		hWnd = hDlg;
		dialogInit();
		return (INT_PTR)TRUE;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		GdiCanvas gdi = GdiCanvas(hWnd, hdc);
		gdi.paste(logo, 10, 20);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_PACK:
			onPack();
			break;
		case IDC_UNPACK:
			onUnpack();
			break;
		case IDC_IMGGLUING:
			onImageConcatenate();
			break;
		case IDOK:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		default:
			break;
		}
		break;
	case WM_DROPFILES:
		dragFiles((HDROP) wParam);
		break;
	case WM_DESTROY:
		userClosed = true;
		if (task.joinable())
			task.join();
		PostQuitMessage(0);
		break;
	}
	return (INT_PTR)FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	hInst = hInstance;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDialog);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
