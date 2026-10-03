#include "SplashWindow.h"
#include "Logging.h"
#include <atomic>
#include <filesystem>
#include <random>
#include <gdiplus.h>
#include <memory>

constexpr UINT WM_SPLASH_CLOSE = WM_APP + 1;

namespace SplashWindow {

	HMODULE gModule;
	static std::atomic<bool> sSignalClose(false);
	static std::wstring sSplashPath;
	static std::unique_ptr<Gdiplus::Image> sImage;
	static std::unique_ptr<Gdiplus::Bitmap> sBitmap;
	static int sWidth = 600;
	static int sHeight = 600;
	static float sVerticalCoverage = 0.5f;
	static HCURSOR sLoadCursor = NULL;
	HWND gSplashWindow = NULL;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {

		case WM_CREATE:
			SetTimer(hWnd, 1, 10, nullptr);
			return 0;

		case WM_SETCURSOR:
			if (LOWORD(lParam) == HTCAPTION) {
				if (sLoadCursor != NULL)
					SetCursor(sLoadCursor);
				return TRUE;
			}
			break;

		case WM_TIMER:
			if (sSignalClose.load(std::memory_order_relaxed))
			{
				KillTimer(hWnd, 1);
				DestroyWindow(hWnd);
				return 0;
			}
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_NCHITTEST:
			return HTCAPTION;

			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);

				HDC memDC = CreateCompatibleDC(hdc);
				HBITMAP memBitmap = CreateCompatibleBitmap(hdc, sWidth, sHeight);
				HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

				Gdiplus::Graphics graphics(memDC);

				graphics.Clear(Gdiplus::Color::White);

				graphics.DrawImage(sBitmap.get(), 0, 0);

				BitBlt(hdc, 0, 0, sWidth, sHeight, memDC, 0, 0, SRCCOPY);

				SelectObject(memDC, oldBitmap);
				DeleteObject(memBitmap);
				DeleteDC(memDC);

				EndPaint(hWnd, &ps);
				return 0;
			}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	DWORD WINAPI ThreadedCreate(LPVOID param) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;

		Gdiplus::GdiplusStartup(
			&gdiplusToken,
			&gdiplusStartupInput,
			nullptr
		);

		sImage = std::make_unique<Gdiplus::Image>(sSplashPath.c_str());

		if (sImage->GetLastStatus() == Gdiplus::Ok) {

			int screenW = GetSystemMetrics(SM_CXSCREEN);
			int screenH = GetSystemMetrics(SM_CYSCREEN);

			float ratio = (float)sImage->GetWidth() / (float)sImage->GetHeight();

			sHeight = (float)screenH * Config::SplashVerticalCoverage;
			sWidth = (float)sHeight * ratio;

			int xPos = (float)(screenW - sWidth) / 2;
			int yPos = (float)(screenH - sHeight) / 2;

			sBitmap = std::make_unique<Gdiplus::Bitmap>(sWidth, sHeight, PixelFormat32bppRGB);
			Gdiplus::Graphics graphics(sBitmap.get());
			graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			graphics.DrawImage(sImage.get(), 0, 0, sWidth, sHeight);
			const char kSplashClassName[] = "TS2ExtenderSplashWin";

			WNDCLASS wc = {};
			wc.lpfnWndProc = WndProc;
			wc.hInstance = gModule;
			wc.lpszClassName = kSplashClassName;
			wc.hCursor = sLoadCursor;
			wc.hbrBackground = nullptr;

			if (!RegisterClass(&wc))
				return 0;

			HWND hWnd = CreateWindowEx(
				WS_EX_TOOLWINDOW,
				kSplashClassName,
				"The Sims 2",
				WS_POPUP,
				xPos, yPos,
				sWidth, sHeight,
				nullptr,
				nullptr,
				gModule,
				nullptr
			);

			if (hWnd == nullptr)
				return 0;

			gSplashWindow = hWnd;

			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);

			MSG msg = {};
			while (GetMessage(&msg, nullptr, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		sBitmap.reset();
		sImage.reset();
		Gdiplus::GdiplusShutdown(gdiplusToken);
		gSplashWindow = NULL;
		return 0;
	}

	void SignalClose() {
		sSignalClose.store(true, std::memory_order_relaxed);
	}

	void Create(const char* splashDirectory, const wchar_t* baseDirectory, float verticalCoverage) {
		sVerticalCoverage = verticalCoverage;
		bool foundSplash = false;
		std::vector<std::wstring> splashFiles;

		std::filesystem::path splashPath = std::filesystem::u8path(splashDirectory);

		std::filesystem::path cursorPath = std::filesystem::path(baseDirectory) / "TSData" / "Res" / "UI" / "Cursors" / "Hourglass_8.ani";

		sLoadCursor = (HCURSOR)LoadImageW(
			NULL,                       
			cursorPath.wstring().c_str(),
			IMAGE_CURSOR,                
			0, 0,                        
			LR_LOADFROMFILE | LR_DEFAULTSIZE
		);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(splashPath)) {
			if (entry.is_regular_file()) {
				std::string fext = entry.path().extension().u8string();
				std::transform(fext.begin(), fext.end(), fext.begin(), ::tolower);

				if (fext == ".png")
				{
					splashFiles.push_back(entry.path().wstring());
					foundSplash = true;
				}
			}
		}

		if (!foundSplash)
			return;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<std::size_t> dist(0, splashFiles.size() - 1);
		std::size_t random_index = dist(gen);

		sSplashPath = splashFiles[random_index];

		CreateThread(nullptr, 0, ThreadedCreate, nullptr, 0, nullptr);
	}
}