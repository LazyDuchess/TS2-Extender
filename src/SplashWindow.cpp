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

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {

		case WM_CREATE:
			SetTimer(hWnd, 1, 50, nullptr);
			return 0;

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

				RECT rect;
				GetClientRect(hWnd, &rect);

				Gdiplus::Graphics graphics(hdc);
				graphics.DrawImage(sBitmap.get(), 0, 0);

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
			sBitmap = std::make_unique<Gdiplus::Bitmap>(600, 600, PixelFormat32bppARGB);
			Gdiplus::Graphics graphics(sBitmap.get());
			graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			graphics.DrawImage(sImage.get(), 0, 0, 600, 600);
			const char kSplashClassName[] = "TS2ExtenderSplashWin";

			WNDCLASS wc = {};
			wc.lpfnWndProc = WndProc;
			wc.hInstance = gModule;
			wc.lpszClassName = kSplashClassName;
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = nullptr;

			if (!RegisterClass(&wc))
				return 0;

			HWND hWnd = CreateWindowEx(
				WS_EX_TOOLWINDOW,
				kSplashClassName,
				"The Sims 2",
				WS_POPUP,
				CW_USEDEFAULT, CW_USEDEFAULT,
				600, 600,
				nullptr,
				nullptr,
				gModule,
				nullptr
			);

			if (hWnd == nullptr)
				return 0;

			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);

			MSG msg = {};
			while (GetMessage(&msg, nullptr, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		Gdiplus::GdiplusShutdown(gdiplusToken);
		sImage.reset();
		sBitmap.reset();
		return 0;
	}

	void SignalClose() {
		sSignalClose.store(true, std::memory_order_relaxed);
	}

	void Create(const char* splashDirectory) {

		bool foundSplash = false;
		std::vector<std::wstring> splashFiles;

		std::filesystem::path splashPath = std::filesystem::u8path(splashDirectory);

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