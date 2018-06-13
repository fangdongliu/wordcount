#pragma once
#include<stack>
#include<string>
#include<unordered_map>
#include"wordCount\WordCounter.h"
#include"AnimUI\uiwindow.h"
#include"AnimUI\UILinearContainer.h"
#include"AnimUI/UIInput.h"
struct CountInfo {
	int wordCount;
	int LineCount;
	int emptyLineCount;
	int noteLineCount;
	int codeLineCount;
};
class App {
public:
	std::stack<std::wstring>pathStack;
	std::unordered_map<std::wstring,CountInfo>info;
	std::unordered_map<std::wstring, ID2D1Bitmap*>iconCache;

	WordCounter counter;
	App();
	~App();
	AnimUI::UIWindow* window;
	AnimUI::UIElement* CreatePage();
public:
	//Actions
	void LoadFolder();
	void Run();
	void Back();

public:
	//infos
	AnimUI::UILinearContainer *explorer;
	AnimUI::UIText* fileName;
	AnimUI::UIInput* path;
	AnimUI::UIElement* element;
	AnimUI::UIText* count[6];
	ID2D1LinearGradientBrush *gradientBrush;


public:

	class EnterListener :public AnimUI::KeyEventListener {
	public:
		EnterListener() {
			last = 0;
		}
		AnimUI::KeyEventListener* last;
		void OnKey(AnimUI::KeyEvent&e)override;
	};

	class ResourceIniter :public AnimUI::DeviceResourceChangeListener {
	public:
		void OnDeviceResourceChange()override;
	};

	class ResourceDestroyer :public AnimUI::DeviceResourceChangeListener {
	public:
		void OnDeviceResourceChange()override;
	};

	class AppBackground :public AnimUI::CanvasPainter {
	public:
		void Paint(AnimUI::UIElement*, AnimUI::UIWindow*)override;
	};

	class Painter :public AnimUI::CanvasPainter {
	public:
		void Paint(AnimUI::UIElement*,AnimUI::UIWindow*)override;
	};
	class Item:public AnimUI::UIRelativeContainer {
	public:
		Item(const wchar_t* fileName);

		AnimUI::UIElement *icon;
		AnimUI::UIText *text;
	};
	class MouseListener :public AnimUI::MouseEventListener {
	public:
		void OnMouse(AnimUI::MouseEvent&e)override;
		void OnMouseOut(AnimUI::UIElement*e)override{}
	};
};