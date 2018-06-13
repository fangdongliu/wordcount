#include<windows.h>
#include"App.h"
#include"AnimUI\AnimUI.h"
#include"AnimUI\UIRelativeContainer.h"
#include"AnimUI\UILinearContainer.h"
#include"AnimUI\helper.h"
#include<wincodec.h>
#include<unordered_set>
#include"AnimUI\AsyncImageLoader.h"
#include"AnimUI\timer.h"
#include"resource.h"
using namespace AnimUI;
using namespace D2D1;
using namespace std;
namespace {
	static unordered_set<wstring>supports = {
		L"cpp",L"c",L"h",L"hpp",L"cxx",L"cs",L"jsp",L"java",L"js",L"py",L"php",L"html",L"htm"
	};
	App *app;
	UIRelativeContainer* CreateHead() {
		auto head = Page::CreateDraggableBackground();
		{
			auto &style = head->currentStyle;
			SizeCaculator size(0, 50.0f, 1.0f, 0, 0);
			style.SetSizeCaculator(size);
		}
		auto icon = new UIElement;
		{
			auto &style = icon->currentStyle;
			icon->painter = new App::IconPainter;
			SizeCaculator size(40.0f,40.0f, 0, 0, 0);
			style.SetSizeCaculator(size);
			style.SetLeft(55.0f);
			style.SetTop(10.0f);
		}
		auto back = new UIElement;
		{
			auto &style = back->currentStyle;
			SizeCaculator size(30.0f, 30.0f, 0, 0, 0);
			style.SetSizeCaculator(size);
			style.SetRight(10.0f);
			style.SetTop(0.0f);
			style.SetCursor(LoadCursor(0, IDC_HAND));
			auto painter = new LinesPainter();
			auto white = D2D1::ColorF(ColorF::Black, 0.5);
			painter->SetColor(white);
			painter->SetWidth(2);
			painter->SetLines(3, D2D1::Point2F(0.5, 0.2), D2D1::Point2F(0.5, 0.8), D2D1::Point2F(0.5, 0.2), D2D1::Point2F(0.2, 0.5), D2D1::Point2F(0.5, 0.2), D2D1::Point2F(0.8, 0.5));
			back->painter = painter;
			back->mouseEventListener = new NoParamMouseListener([](MouseEvent&e){
				if (e.GetEventHandledId() != 2) {
					e.EventHandled(2);
				}
				if (e.GetEventType() == MouseEventType::MouseEventLButtonUp) {
					if (e.GetDestElement() == e.GetDestWindow()->GetCaptureElement()) {
						app->Back();
					}
				}
			});
		}

		auto path = new UIInput;
		{
			app->path = path;
			auto &style = path->currentStyle;
			SizeCaculator size(-50.0f, 30.0f, 1.0f, 0, 0);
			style.SetSizeCaculator(size);
			style.SetCursor(LoadCursorW(0, IDC_IBEAM));
			path->trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
			path->textAlignVertical = TextAlignCenter;
			style.SetLeft(0.0f);
			style.SetFontSize(14.0f);
			auto lis = new App::EnterListener;
			path->keyEventListener = lis;
			RECTF padding = { 10,4,4,4 };
			style.SetPadding(padding);
			path->trimming.delimiter = '\\';
			path->trimming.delimiterCount = 2;
			style.SetTop(0.0f);
			ColorF color(0x444444);
			style.SetTextColor(color);
		}
		auto pathWrapper = new UIRelativeContainer;
		{
			auto &style = pathWrapper->currentStyle;
			SizeCaculator size(-300.0f, 30.0f, 1.0, 0, 0);
			style.SetSizeCaculator(size);
			style.SetBorderRadius(15.0f);
			style.SetLeft(150.0f);
			UIBorder border = { D2D1::ColorF(D2D1::ColorF::White),1.0f,1,1,1,1 };
			style.SetBorder(border);

			D2D1::ColorF white(D2D1::ColorF::White, 0.2);
			style.SetBackgroundColor(white);
			style.SetTop(20.0f);
		}
		{
			pathWrapper->AddChild(path);
			pathWrapper->AddChild(back);
		}

		auto close = new UIElement;
		{
			auto stateStyle = new UIStateStyle;

			auto style = new UIStyle;
			close->mouseEventListener = new NoParamMouseListener(
				[](MouseEvent&e)->void {
				if (e.GetEventHandledId() != 2) {
					e.EventHandled(2);
				}
				if (e.GetEventType() == MouseEventType::MouseEventLButtonUp) {
					if (e.GetDestElement() == e.GetDestWindow()->GetCaptureElement()) {
						e.GetDestWindow()->DoAction(WindowActionDestroy);
					}
				}
			});
			auto white = D2D1::ColorF(ColorF::White);
			auto painter = new LinesPainter();
			painter->SetColor(white);
			painter->SetWidth(2);
			painter->SetLines(2, D2D1::Point2F(0.35, 0.35), D2D1::Point2F(0.65, 0.65), D2D1::Point2F(0.65, 0.35), D2D1::Point2F(0.35, 0.65));
			close->painter = painter;
			SizeCaculator size(50.0f, 50.0f, 0, 0, 0);
			style->SetSizeCaculator(size);
			ColorF color(ColorF::Red);
			style->SetBackgroundColor(color);
			style->SetRight(0);
			style->SetTop(0);
			style->SetDuration(0.3f);
			stateStyle->SetState(UIStateHover, style);
			style = new UIStyle;
			style->SetSizeCaculator(size);
			ColorF color2(ColorF::Red,0.0f);
			style->SetBackgroundColor(color2);
			style->SetRight(0);
			style->SetTop(0);
			style->SetDuration(0.3f);
			stateStyle->SetState(UIStateNormal, style);
			close->SetStateStyle(stateStyle);
		}
		auto smallBtn = new UIElement;
		{
			auto stateStyle = new UIStateStyle;

			auto style = new UIStyle;
			SizeCaculator size(50.0f, 50.0f, 0, 0, 0);
			style->SetSizeCaculator(size);
			ColorF color(0xa0b099);
			style->SetBackgroundColor(color);
			style->SetRight(50.0f);
			style->SetTop(0);
			style->SetDuration(0.3f);
			stateStyle->SetState(UIStateHover, style);
			style = new UIStyle;
			style->SetSizeCaculator(size);
			ColorF color2(0xa0b099, 0.0f);
			style->SetBackgroundColor(color2);
			style->SetRight(50.0f);
			style->SetTop(0);
			style->SetDuration(0.3f);
			stateStyle->SetState(UIStateNormal, style);
			smallBtn->SetStateStyle(stateStyle);
			smallBtn->mouseEventListener=new NoParamMouseListener(
					[](MouseEvent&e)->void {
				if (e.GetEventHandledId() != 2) {
					e.EventHandled(2);
				}
				if (e.GetEventType() == MouseEventType::MouseEventLButtonUp) {
					if (e.GetDestElement() == e.GetDestWindow()->GetCaptureElement()) {
						e.GetDestWindow()->DoAction(WindowActionMinimize);
					}
				}
			});
			auto painter = new LinesPainter();
			auto white = D2D1::ColorF(ColorF::White);
			painter->SetColor(white);
			painter->SetLines(1, D2D1::Point2F(0.35, 0.6), D2D1::Point2F(0.65, 0.6));
			painter->SetWidth(2);
			smallBtn->painter = painter;
		}
		head->AddChild(smallBtn);
		head->AddChild(close);
	//	head->AddChild(icon);
		head->AddChild(pathWrapper);

		return head;
	}
	UIRelativeContainer* CreateLeftBar() {
		auto width = 150.0f;
		auto left = new UIRelativeContainer;
		{
			auto &style = left->currentStyle;
			SizeCaculator size(width, -50.0f, 0, 1.0f, 0);
			style.SetSizeCaculator(size);
			ColorF color(0xf0f0f0);
			style.SetTop(50.0f);
		}
		auto item = new UIRelativeContainer;
		{
			auto& style = item->currentStyle;
			SizeCaculator size(150.0f, 200.0f, 0, 0, 0);
			style.SetSizeCaculator(size);
		}
		auto fileIcon = new UIElement;
		{
			fileIcon->painter = new App::Painter;
			auto &style = fileIcon->currentStyle;
			SizeCaculator size(50.f, 50.0f, 0,0, 0);
			style.SetSizeCaculator(size);
			style.SetLeft(50.0f);
			style.SetTop(50.0f);
		}
		item->AddChild(fileIcon);

		auto fileName = new UIText;
		{
			fileName->contentText = L".";
			auto &style = fileName->currentStyle;
			fileName->trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
			SizeCaculator size(width-20.0f, 70.0f, 0, 0, 0);
			ColorF color(0x222222);
			fileName->isInline = false;
			style.SetTextColor(color);
			style.SetSizeCaculator(size);
			style.SetLeft(10.0f);
			style.SetTop(100.0f);
			fileName->textAlignHorizontal = TextAlignCenter;
			app->fileName = fileName;
		}
		item->AddChild(fileName);
		left->AddChild(item);
		WCHAR strs[6][10] = { L"字符数---",L"单词数---",L"总行数---",L"空行数---",L"代码行---",L"注释行---" };
		for (int i = 0; i < 6; i++) {
			auto wordCount = new UIText;
			{
				app->count[i] = wordCount;
				wordCount->contentText = strs[i];
				auto &style = wordCount->currentStyle;
				ColorF color(0x222222);
				style.SetTextColor(color);
				style.SetFontSize(12.0f);
				SizeCaculator size(200.f, 40.0f, 0, 0, 0);
				style.SetSizeCaculator(size);
				style.SetLeft(35.0f);
				style.SetTop(200+i*40.0f);
				left->AddChild(wordCount);
			}
		}
		return left;
	}
	UILinearContainer* CreateFileExplorer() {
		auto explorer = new UILinearContainer;
	
		explorer->Float(LinearFloat::LinearFloatLeft);
		{
			auto &style = explorer->currentStyle;
			SizeCaculator size(-150.f, -50.0f, 1.0f, 1.0f, 0);
			style.SetSizeCaculator(size);
			style.SetTop(50.0f);
			style.SetLeft(150.0f);
		}
		WIN32_FIND_DATA data;	
		app->explorer = explorer;
		app->LoadFolder();
		return explorer;
	}

}


App::App() {
	WCHAR path[2000];
	app = this;
	GetCurrentDirectoryW(2000, path);
	pathStack.push(path);
	appIcon = 0;
	InitFDUI(LoadIconW(HINST_THISCOMPONET,MAKEINTRESOURCEW(IDI_ICON1)));
	window = UIWindow::CreateMainWindow(0, 0,800,500);
	window->createResourceListener = new ResourceIniter;
	window->destroyResourceListener = new ResourceDestroyer;
	window->Center();
	window->SetCurrentPage(CreatePage(),false);
	

	window->Show();
}

App::~App() {
	ReleaseFDUI();

}

void App::LoadFolder() {
	SetCurrentDirectoryW(pathStack.top().c_str());
	for (auto i : iconCache) {
		SafeRelease<ID2D1Bitmap>(&i.second);
	}
	iconCache.clear();
	WIN32_FIND_DATAW data;
	WCHAR strpath[1000];
	GetCurrentDirectoryW(1000, strpath);
	path->contentText = strpath;
	path->needMeasure = true;
	auto first = FindFirstFileW(L"*", &data);
	int cur = 0;
	for (auto i : explorer->childs) {
		delete i;
	}
	explorer->scrollY = 0;
	explorer->childs.clear();
	if (first) {
		do {
			bool isFolder = false;
			if (data.cFileName[0] == L'.') {
				if (data.cFileName[1] == 0)
					continue;
				isFolder = true;
			}
			else {
				isFolder = data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
			}
			if(isFolder)
				explorer->AddChild(new App::Item(data.cFileName));
			else {
				for (int start = lstrlenW(data.cFileName); start >= 0; start--) {
					if (data.cFileName[start] == L'.') {
						if (supports.count(data.cFileName + start+1)) {
							explorer->AddChild(new App::Item(data.cFileName));
						}
					}
				}
			}
		} while (FindNextFileW(first, &data));
		FindClose(first);
	}
	explorer->needMeasure = true;
}



UIElement* App::CreatePage() {
	auto wrapper = Page::CreateDraggableBackground();
	wrapper->AddChild(CreateHead());
	wrapper->AddChild(CreateLeftBar());
	wrapper->AddChild(CreateFileExplorer());
	wrapper->painter = new AppBackground;
	return wrapper;
}



void App::Run() {
	MessageLoop();
}

void App::Back() {
	if (pathStack.size() > 1) {
		pathStack.pop();
		LoadFolder();
	}
}

App::Item::Item(const wchar_t*fileName) {
	isFocusable = true;
	{
		mouseEventListener = new App::MouseListener;
		SizeCaculator size(50.0f, 80.0f, 0, 0, 0);
		auto stateStyle = new UIStateStyle;
		auto style = new UIStyle;
		style->SetSizeCaculator(size);
		ColorF color(ColorF(0xa0b099));
		AnimUI::RECTF margin = { 10.f,10.f,10.f,10.f };
		style->SetMargin(margin);
		style->SetBackgroundColor(color);
		stateStyle->SetState(UIStateHover, style);
		style = new UIStyle;
		style->SetSizeCaculator(size);
		style->SetMargin(margin);
		stateStyle->SetState(UIStateNormal, style);
		style = new UIStyle;
		style->SetSizeCaculator(size);
		style->SetMargin(margin);
		
		style->SetBackgroundColor(color);
		stateStyle->SetState(UIStateFocus, style);
		SetStateStyle(stateStyle);
	}
	
	icon = new UIElement;
	{
		auto& style = icon->currentStyle;
		style.SetTop(10.0f);
		style.SetLeft(10.0f);
		SizeCaculator size(30.0f, 30.0f, 0, 0, 0);
		icon->painter = new App::Painter;
		style.SetSizeCaculator(size);
	}
	AddChild(icon);
	text = new UIText;
	{
		text->contentText = fileName;
		text->trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
		text->textAlignVertical = TextAlignCenter;
		text->textAlignHorizontal = TextAlignCenter;
		text->isInline = false;
		auto& style = text->currentStyle;
		
		style.SetFontSize(12.0f);
		SizeCaculator size(50.0f, 40.0f, 0, 0, 0);
		style.SetLeft(0);
		style.SetTop(40.0f);

		style.SetSizeCaculator(size);
	}
	AddChild(text);

}

void App::IconPainter::Paint(AnimUI::UIElement*element, AnimUI::UIWindow*window) {
	if (app->appIcon) {
		auto &rect = element->rect;
		D2D1_SIZE_F size;
		window->bitmapBrush->SetBitmap(app->appIcon);
		size = app->appIcon->GetSize();
		size.height = rect.Height / size.height;
		size.width = rect.Width / size.width;
		window->bitmapBrush->SetTransform(D2D1::Matrix3x2F::Scale(size)*D2D1::Matrix3x2F::Translation(D2D1::SizeF(rect.X, rect.Y)));

		window->currentRenderTarget->FillRectangle(D2D1::RectF(rect.X, rect.Y, rect.Width + rect.X, rect.Height + rect.Y), window->bitmapBrush);
	}
}

void App::Painter::Paint(UIElement*element, UIWindow*window) {

	auto LoadD2DIcon = [window](const wchar_t* name) {
		SHFILEINFOW info;
		ID2D1Bitmap *icon = 0;
		SHGetFileInfoW(name, 0, &info, sizeof(info), SHGFI_ICON|SHGFI_LARGEICON);
		HRESULT hr = S_OK;
		IWICBitmap *bitmap = 0;
		IWICFormatConverter *converter = 0;

		hr = AsyncImageLoader::imageFactory->CreateBitmapFromHICON(info.hIcon, &bitmap);
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		hr = AsyncImageLoader::imageFactory->CreateFormatConverter(&converter);
		if (!SUCCEEDED(hr)) {
			goto end;
		}
		hr = converter->Initialize(
			bitmap,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			0,
			0.f,
			WICBitmapPaletteTypeCustom
		);
		if (!SUCCEEDED(hr)) {
			goto end;
		}
		hr = window->currentRenderTarget->CreateBitmapFromWicBitmap(converter, &icon);
		if (!SUCCEEDED(hr)) {
			goto end;
		}
		
	end:
		SafeRelease<IWICFormatConverter>(&converter);
		SafeRelease<IWICBitmap>(&bitmap);
		
		DestroyIcon(info.hIcon);
		return icon;
	};
	auto child=(UIText*)((UIContainer*)element->GetParent())->childs[1];
	SHFILEINFO info;
	int start;
	ID2D1Bitmap *icon=0;
	wstring prefix;
	if(child->contentText[0]!='.')
	for (start = child->contentText.length()-1; start >= 0; start--) {
		if (child->contentText[start] == '.') {
			prefix = child->contentText.substr(start+1);
			if (prefix == L"exe"||prefix==L"lnk") {
				prefix = child->contentText;
			}
			else if (prefix == L"") {
				icon = app->iconCache[child->contentText.c_str()];
				if (!icon) {
					icon = LoadD2DIcon(child->contentText.c_str());
					app->iconCache[child->contentText.c_str()] = icon;
				}
				goto draw;
			}
			icon = app->iconCache[prefix];
			if(!icon){
				icon = LoadD2DIcon(child->contentText.c_str());
				app->iconCache[prefix] = icon;
			}
			goto draw;
		}
	}
	icon = app->iconCache[child->contentText.c_str()];
	if (!icon) {
		icon = LoadD2DIcon(child->contentText.c_str());
		app->iconCache[child->contentText.c_str()] = icon;
	}
	draw:
	if (icon) {
		auto &rect = element->rect;
		D2D1_SIZE_F size;
		window->bitmapBrush->SetBitmap(icon);
		size = icon->GetSize();
		size.height = rect.Height / size.height;
		size.width = rect.Width / size.width;
		window->bitmapBrush->SetTransform(D2D1::Matrix3x2F::Scale(size)*D2D1::Matrix3x2F::Translation(D2D1::SizeF(rect.X, rect.Y)));

		window->currentRenderTarget->FillRectangle(D2D1::RectF(rect.X, rect.Y, rect.Width + rect.X, rect.Height + rect.Y), window->bitmapBrush);
	}
}
void App::MouseListener::OnMouse(MouseEvent&e) {
	static Timer timer;
	switch (e.GetEventType()) {
	case MouseEventType::MouseEventLButtonDown:
		
		float lastTime = timer.TotalTime();
		timer.Tick();
		app->fileName->contentText = ((UIText*)((UIContainer*)e.GetDestElement())->childs[1])->contentText;
		app->fileName->needMeasure = true;
		WIN32_FIND_DATAW data;
		bool isFolder = false;
		if (app->fileName->contentText[0] == L'.') {
			isFolder = true;
		}
		else {
			auto handle = FindFirstFileW(app->fileName->contentText.c_str(), &data);
			isFolder=data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		}
		if(isFolder){
			if (e.GetDestWindow()->lastClickElement == e.GetDestElement() && timer.TotalTime() - lastTime < 0.3f) {
				e.GetDestWindow()->lastClickElement = 0;
				e.EventHandled(2);
				WCHAR path[1000];
				e.GetDestWindow()->hoverElement = 0;
				e.GetDestWindow()->focusElement = 0;
				e.GetDestWindow()->captureElement = 0;
				GetCurrentDirectoryW(1000, path);
				lstrcatW(path, L"\\");
				lstrcatW(path, app->fileName->contentText.c_str());
				app->pathStack.push(path);
				
				app->LoadFolder();
				return;
			}
			else {
				app->count[0]->contentText = L"字符数---";
				app->count[1]->contentText = L"单词数---";
				app->count[2]->contentText = L"总行数---";
				app->count[3]->contentText = L"代码行---";
				app->count[4]->contentText = L"空行数---";
				app->count[5]->contentText = L"注释行---";
				for (int i = 0; i < 6; i++)
					app->count[i]->needMeasure = true;
			}
			e.GetDestWindow()->lastClickElement = e.GetDestElement();
			return;
		}
		int head;
		if (app->fileName->contentText[0] != '.')
		for (head = app->fileName->contentText.length() - 1; head >= 0; head--) {
			if (app->fileName->contentText[head] == '.') {
				break;
			}
		}
		if (app->fileName->contentText.substr(head+1) == L"exe") {
			if (e.GetDestWindow()->lastClickElement == e.GetDestElement() && timer.TotalTime() - lastTime < 0.3f) {
			/*	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, app->fileName->contentText.c_str(), -1, NULL, 0, NULL, FALSE);
				char *psText;
				psText = new char[dwNum];
				WideCharToMultiByte(CP_OEMCP, NULL, app->fileName->contentText.c_str(), -1, psText, dwNum, NULL, FALSE);
			*/	PROCESS_INFORMATION pro_info; //进程信息
				STARTUPINFOW sti; //启动信息
				ZeroMemory(&pro_info, sizeof(PROCESS_INFORMATION));
				ZeroMemory(&sti, sizeof(STARTUPINFO));
				CreateProcessW(app->fileName->contentText.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sti, &pro_info);
				
			}
			e.GetDestWindow()->lastClickElement = e.GetDestElement();
			return;
		}
		else if (app->fileName->contentText.substr(head + 1) == L"html"|| app->fileName->contentText.substr(head + 1) == L"htm") {
			if (e.GetDestWindow()->lastClickElement == e.GetDestElement() && timer.TotalTime() - lastTime < 0.3f) {
				WCHAR path[1000];
				GetCurrentDirectoryW(1000, path);

				ShellExecuteW(0, 0, (L"file:///" + (path + (L"/" + app->fileName->contentText))).c_str(), 0, 0, SW_SHOW);
			}
			e.GetDestWindow()->lastClickElement = e.GetDestElement();
			return;
		}
		e.GetDestWindow()->lastClickElement = e.GetDestElement();
		if (!app->counter.SetCurrentFile(app->fileName->contentText.c_str())) {
			app->count[0]->contentText = L"字符数---";
			app->count[1]->contentText = L"单词数---";
			app->count[2]->contentText = L"总行数---";
			app->count[3]->contentText = L"代码行---";
			app->count[4]->contentText = L"空行数---";
			app->count[5]->contentText = L"注释行---";
			for (int i = 0; i < 6; i++)
				app->count[i]->needMeasure = true;
			return;
		}

		WCHAR buf[200];
		auto cnt = app->counter.CountAll();
		swprintf_s(buf, L"字符数%d", cnt.cnt[0]);
		app->count[0]->contentText=buf;
		swprintf_s(buf, L"单词数%d", cnt.cnt[1]);
		app->count[1]->contentText = buf;
		swprintf_s(buf, L"总行数%d", cnt.cnt[2]);
		app->count[2]->contentText = buf;
		swprintf_s(buf, L"代码行%d", cnt.cnt[3]);
		app->count[3]->contentText = buf;
		swprintf_s(buf, L"空行数%d", cnt.cnt[4]);
		app->count[4]->contentText = buf;
		swprintf_s(buf, L"注释行%d", cnt.cnt[5]);
		app->count[5]->contentText = buf;
		for (int i = 0; i < 6; i++)
			app->count[i]->needMeasure = true;
		break;


	}
}

void App::AppBackground::Paint(UIElement*e,UIWindow*w) {
	D2D1_RECT_F range = { e->rect.X,e->rect.Y,e->rect.X + e->rect.Width,e->rect.Y + e->rect.Height };
	app->gradientBrush->SetStartPoint(D2D1::Point2F(range.left, range.top));
	app->gradientBrush->SetEndPoint(D2D1::Point2F(range.right, range.bottom));
	w->currentRenderTarget->FillRectangle(range, app->gradientBrush);
}

void App::ResourceDestroyer::OnDeviceResourceChange() {
	for (auto i : app->iconCache) {
		SafeRelease<ID2D1Bitmap>(&i.second);
	}
	app->iconCache.clear();
	SafeRelease<ID2D1Bitmap>(&app->appIcon);
}

void App::ResourceIniter::OnDeviceResourceChange() {
	ID2D1GradientStopCollection *cll;
	D2D1_GRADIENT_STOP stop[] = {
		{0.0f,ColorF(0x92a8d1)},{0.7f,ColorF(ColorF::LightBlue)},{1.0f,ColorF(ColorF::LightSkyBlue)}
	};
	app->window->currentRenderTarget->CreateGradientStopCollection(stop, 2, &cll);
	app->window->currentRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0,0),D2D1::Point2F(800,500)), cll, &app->gradientBrush);
	auto LoadD2DIcon = [](HICON hIcon) {
		ID2D1Bitmap *icon = 0;
		HRESULT hr = S_OK;
		IWICBitmap *bitmap = 0;
		IWICFormatConverter *converter = 0;

		hr = AsyncImageLoader::imageFactory->CreateBitmapFromHICON(hIcon, &bitmap);
		if (!SUCCEEDED(hr)) {
			goto end;
		}

		hr = AsyncImageLoader::imageFactory->CreateFormatConverter(&converter);
		if (!SUCCEEDED(hr)) {
			goto end;
		}
		hr = converter->Initialize(
			bitmap,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			0,
			0.f,
			WICBitmapPaletteTypeCustom
		);
		if (!SUCCEEDED(hr)) {
			goto end;
		}
		hr = app->window->currentRenderTarget->CreateBitmapFromWicBitmap(converter, &icon);
		if (!SUCCEEDED(hr)) {
			goto end;
		}

	end:
		SafeRelease<IWICFormatConverter>(&converter);
		SafeRelease<IWICBitmap>(&bitmap);

		DestroyIcon(hIcon);
		return icon;
	};

	app->appIcon = LoadD2DIcon(LoadIconW(HINST_THISCOMPONET, MAKEINTRESOURCEW(IDI_ICON1)));
}

void App::EnterListener::OnKey(KeyEvent&e) {
	switch (e.GetKeyCode()) {
	case VK_RETURN:
		if (SetCurrentDirectoryW(app->path->contentText.c_str())) {
			app->pathStack.push(app->path->contentText.c_str());
			app->LoadFolder();
		}
		app->path->caretPosition = app->path->caretPositionOffset = app->path->caretAnchor = 0;
		e.GetDestWindow()->SetFocusElement(0);
		break;
	}
	e.EventHandled(2);

}