#include "pch.h"
#include "resource.h"
#include "Common/Canvas.h"
#include "Common/ResourcePack.h"

// HWND of the dialog
HWND hWnd = NULL;
// HINSTANCE of the process
HINSTANCE hInst;
// Logo image
pImageMatrix logo;

inline void disableButtons()
{
    EnableWindow(GetDlgItem(hWnd, IDC_PACK), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_UNPACK), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_IMGGLUING), FALSE);
    EnableWindow(GetDlgItem(hWnd, IDC_FONTIMG), FALSE);
}

inline void enableButtons()
{
    EnableWindow(GetDlgItem(hWnd, IDC_PACK), TRUE);
    EnableWindow(GetDlgItem(hWnd, IDC_UNPACK), TRUE);
    EnableWindow(GetDlgItem(hWnd, IDC_IMGGLUING), TRUE);
    EnableWindow(GetDlgItem(hWnd, IDC_FONTIMG), TRUE);
}

std::thread task;
bool userClosed = false;    // if user closed the window, do not update window controls when tasks finished

#include "Common/ResourcePack.h"
using namespace std::filesystem;
void deployPack(const std::wstring& destFile, const path& resDirectory)
{
    SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)L"Generating...");
    disableButtons();
    if (task.joinable()) task.join();
    task = std::thread([=]() {
        auto&& queue = walkFolder(resDirectory);
        HWND hwndPB = GetDlgItem(hWnd, IDC_PROGRESS);
        size_t cb = std::get<0>(queue.front()).dataSize;
        size_t count = 0;
        SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, cb / 2048)); // range(0, cb/2KB)
        SendMessage(hwndPB, PBM_SETSTEP, (WPARAM)1, 0); // step 1 => 2KB
        if (generatePack(path(destFile).u8string(), queue,
            [hwndPB, &count](size_t filesize) {
                count += filesize;
                while (count > 2048) {
                    count -= 2048;
                    SendMessage(hwndPB, PBM_STEPIT, 0, 0); // update progress bar
                }
            }))
            userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
            (LPARAM)L"The resource file was successfully generated.");
        else
            userClosed ? 0 : SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0,
            (LPARAM)L"An error occured when packing resource folder.");
        userClosed ? 0 : enableButtons();
    });
}

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

#include "Common/Canvas.h"
bool imageConcatenate(const std::wstring& destImage, const std::vector<std::wstring>& imgFiles)
{
    std::vector<pImageMatrix> imgs;
    uint32_t width = 0, total_height = 0;
    try {
        for (auto& file : imgFiles) {
            std::wstring extName = file.substr(file.rfind(L'.') + 1);
            if (extName == L"png" || extName == L"PNG")
                imgs.emplace_back(ImageMatrixFactory::fromPngFile(file.c_str()));
            else
                imgs.emplace_back(ImageMatrixFactory::fromJpegFile(file.c_str()));
            if (width == 0)
                width = imgs.back()->getWidth();
            else if (imgs.back()->getWidth() != width)
                throw std::runtime_error("Images width don't match.\n"\
                    "Only images with the same width can be concatenated.");
            total_height += imgs.back()->getHeight();
            if (total_height > std::numeric_limits<uint16_t>::max())
                throw std::runtime_error("Image height is too large. \n"\
                    "Reduce the number of images and try again.");
        }
    }
    catch (std::runtime_error e) {
        MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        return false;
    }
    auto temp = ImageMatrixFactory::createBufferImage(width, total_height, Canvas::Constant::alpha);
    uint16_t y = 0;
    for (const pImageMatrix i : imgs) {
        PiCanvas::blend(temp, i, 0, y, 0xFF);
        y += i->getHeight();
    }
    try {
        std::wstring extName = destImage.substr(destImage.rfind(L'.') + 1);
        if (extName == L"png" || extName == L"PNG")
            ImageMatrixFactory::dumpPngFile(temp, destImage.c_str());
        else
            ImageMatrixFactory::dumpJpegFile(temp, destImage.c_str(), 100);
    }
    catch (std::runtime_error e) {
        MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

void deployImageContatenate(const std::wstring& destImage, std::vector<std::wstring>& imgFiles)
{
    SendDlgItemMessage(hWnd, IDC_STATUS, WM_SETTEXT, 0, (LPARAM)L"Concatenating...");
    // sort files by their names
    std::sort(imgFiles.begin(), imgFiles.end(), [](const std::wstring& a, const std::wstring& b) -> bool{
        size_t p1 = a.rfind(L'\\');
        std::wstring n1 = (p1 == std::wstring::npos ? a : a.substr(p1 + 1));
        size_t p2 = b.rfind(L'\\');
        std::wstring n2 = (p2 == std::wstring::npos ? b : b.substr(p2 + 1));
        return n1.compare(n2) < 0;
    });
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
    auto temp = ImageMatrixFactory::createBufferImage(logo->getWidth() / 2, logo->getHeight() / 2, 0xFFF0F0F0);
    PiCanvas::blend(temp, logo, 0, 0, logo->getWidth() / 2, logo->getHeight() / 2,
        0, 0, logo->getWidth(), logo->getHeight(), 0xFF);
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
    // max 100 files
    #define FILE_LIST_BUFFER_SIZE ((100 * (MAX_PATH + 1)) + 1)
    wchar_t* tempPath = new wchar_t[FILE_LIST_BUFFER_SIZE];
    *tempPath = L'\0';

    OPENFILENAME ofn;
    ofn.lpstrFile = tempPath;
    ofn.nMaxFile = FILE_LIST_BUFFER_SIZE;
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
    delete[] tempPath;
}

void dragFiles(HDROP hDrop)
{
    TCHAR tempPath[MAX_PATH];
    *tempPath = L'\0';

    UINT NumOfFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
    if (NumOfFiles == 1)
    {
        DragQueryFile(hDrop, 0, tempPath, MAX_PATH);
        DragFinish(hDrop);
        path file(tempPath);
        if (is_directory(file)) // pack resource
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
#include <set>
const int char_width = 32, char_height = 32;
TCHAR characters[4096], fontName[32];
bool GenerateFontImage(int chars_per_line)
{
    TCHAR tempPath[MAX_PATH];
    *tempPath = L'\0';
    OPENFILENAME ofn;
    ofn.lpstrFile = tempPath;
    ofn.nMaxFile = sizeof(tempPath);
    setOFN(ofn, L"PNG File (*.png)\0*.png\0\0", L"png");
    if (GetSaveFileName(&ofn) <= 0)
        return false;
    std::set<wchar_t, std::less<>> charset;
    for (TCHAR* i = characters; *i; i++)
    {
        charset.insert(*i);
    }
    int width = chars_per_line * char_width;
    int height = char_height + (int)charset.size() / chars_per_line * char_height;
    
    HDC tdc = GetDC(hWnd);
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP hbmp = CreateCompatibleBitmap(tdc, width, height);
    HBITMAP hobmp = (HBITMAP)SelectObject(hdc, hbmp);
    ReleaseDC(hWnd, tdc);

    SetBkColor(hdc, Canvas::toABGR(0x00FFFFFF));
    RECT rect{ 0, 0, width, height };
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

    HFONT fn = CreateFont(char_height, 0, 0, 0, 100, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS, fontName);
    HFONT oldfn = (HFONT)SelectObject(hdc, fn);
    SetBkColor(hdc, 0x00FFFFFF);
    SetTextColor(hdc, 0x00000000);
    int x = 0, y = 0;
    for (auto c : charset)
    {
        TextOut(hdc, x, y, &c, 1);
        x += char_width;
        if (x == width) {
            x = 0;
            y += char_height;
        }
    }
    SelectObject(hdc, oldfn);
    DeleteObject(fn);
    DeleteObject(oldfn);

    BITMAP bmpObj;
    GetObject(hbmp, sizeof(BITMAP), &bmpObj);
    BITMAPINFOHEADER bmpHead;
    bmpHead.biBitCount = (WORD)(GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES));
    bmpHead.biCompression = BI_RGB;
    bmpHead.biPlanes = 1;
    bmpHead.biHeight = -bmpObj.bmHeight;
    bmpHead.biWidth = bmpObj.bmWidth;
    bmpHead.biSize = sizeof BITMAPINFOHEADER;
    uint32_t* tmp = new uint32_t[(size_t)width*height];
    GetDIBits(hdc, hbmp, 0, bmpObj.bmHeight, tmp, (LPBITMAPINFO)&bmpHead, DIB_RGB_COLORS);

    SelectObject(hdc, hobmp);
    DeleteObject(hbmp);
    DeleteObject(hobmp);
    DeleteDC(hdc);

    pImageMatrix im = ImageMatrixFactory::fromPixelData(tmp, width, height, true);
    ImageMatrixFactory::dumpPngFile(im, tempPath);
    return true;
}

INT_PTR CALLBACK FontImg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SendDlgItemMessage(hDlg, IDC_ALLCHARS, WM_SETTEXT, 0, (LPARAM)L"abcdefghijklmnopqrstuvwxyz1234567890,.:!");
        SendDlgItemMessage(hDlg, IDC_CHARSPERLINE, WM_SETTEXT, 0, (LPARAM)L"12");
        SendDlgItemMessage(hDlg, IDC_FONTFACE, WM_SETTEXT, 0, (LPARAM)L"Microsoft Yahei UI");
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CHOOSEFONT:
        {
            GetWindowText(GetDlgItem(hDlg, IDC_FONTFACE), fontName, sizeof fontName / sizeof TCHAR);
            LOGFONT lf;
            memset(&lf, 0, sizeof(LOGFONT));
            (void)lstrcpyn(lf.lfFaceName, fontName, sizeof lf.lfFaceName / sizeof TCHAR);
            lf.lfHeight = char_height;
            CHOOSEFONT cf;
            memset(&cf, 0, sizeof(CHOOSEFONT));
            cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
            cf.lpLogFont = &lf;
            cf.lStructSize = sizeof CHOOSEFONT;
            if (ChooseFont(&cf) == TRUE)
            {
                SendDlgItemMessage(hDlg, IDC_FONTFACE, WM_SETTEXT, 0, (LPARAM)lf.lfFaceName);
            }
        }
            return (INT_PTR)TRUE;
            break;
        case IDOK:
        {
            GetWindowText(GetDlgItem(hDlg, IDC_CHARSPERLINE), characters, sizeof characters / sizeof TCHAR);
            int chars_per_line = _wtoi(characters);
            if (chars_per_line == 0)
            {
                MessageBox(hDlg, L"Invalid number of chars per line.", L"ERROR", MB_ICONERROR);
                break;
            }
            GetWindowText(GetDlgItem(hDlg, IDC_ALLCHARS), characters, sizeof characters / sizeof TCHAR);
            GetWindowText(GetDlgItem(hDlg, IDC_FONTFACE), fontName, sizeof fontName / sizeof TCHAR);
            if (GenerateFontImage(chars_per_line))
                EndDialog(hDlg, LOWORD(wParam));
        }
            return (INT_PTR)TRUE;
            break;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;
        default:
            break;
        }
    default:
        return (INT_PTR)FALSE;
    }
}

//---------------------------------------------
#include "Common/Window.h"
#include "Common/Timer.h"
#include "Snake/Snake.h"
SnakeView* easter = nullptr;

INT_PTR CALLBACK MainDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDBLCLK:
    {
        short x = (short)LOWORD(lParam);
        short y = (short)HIWORD(lParam);
        if (x > 0 && y > 0 && x < 350 && y < 100)
        {
            if (easter)
                delete easter;
            easter = new SnakeView(hInst, hWnd);
        }
    }
        break;
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
        case IDC_FONTIMG:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_FONTIMG), hWnd, FontImg);
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
