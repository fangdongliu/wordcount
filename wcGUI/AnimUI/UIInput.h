#pragma once
#include"UIText.h"

namespace AnimUI {
	class UIInput :public UIText {
	public:
		enum SetSelectionMode
		{
			SetSelectionModeLeft,               // cluster left
			SetSelectionModeRight,              // cluster right
			SetSelectionModeUp,                 // line up
			SetSelectionModeDown,               // line down
			SetSelectionModeLeftChar,           // single character left (backspace uses it)
			SetSelectionModeRightChar,          // single character right
			SetSelectionModeLeftWord,           // single word left
			SetSelectionModeRightWord,          // single word right
			SetSelectionModeHome,               // front of line
			SetSelectionModeEnd,                // back of line
			SetSelectionModeFirst,              // very first position
			SetSelectionModeLast,               // very last position
			SetSelectionModeAbsoluteLeading,    // explicit position (for mouse click)
			SetSelectionModeAbsoluteTrailing,   // explicit position, trailing edge
			SetSelectionModeAll                 // select all text
		}seletionMode;
		struct CaretFormat
		{
			wchar_t fontFamilyName[100];
			wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
			FLOAT fontSize;
			DWRITE_FONT_WEIGHT fontWeight;
			DWRITE_FONT_STRETCH fontStretch;
			DWRITE_FONT_STYLE fontStyle;
			UINT32 color;
			BOOL hasUnderline;
			BOOL hasStrikethrough;
		}caretFormat;
		UIInput();
		void ToDefaultState();
		void ToInputState();
		void OnKey(KeyEvent&e)override;
		void OnMouse(MouseEvent&e)override;
		void OnPaint(UIWindow*window)override;
		void PaintSelectRange(UIWindow*);
		DWRITE_TEXT_RANGE GetSelectionRange();
	//	bool SetSelectionFromPoint(float x, float y, bool extendSelection);
		void AlignCaretToNearestCluster(bool isTrailingHit = false, bool skipZeroWidth = false);
		void GetCaretRect(D2D1_RECT_F& rect);
		void UpdateSystemCaret(UIWindow*window, const D2D1_RECT_F& rect);

		void DeleteSelection();
		void CopyToClipboard(UIWindow*);
	
		void PasteFromClipboard(UIWindow*s);
		bool isInputMode;
		UINT32 caretAnchor;
		UINT32 caretPosition;
		int caretPositionOffset;

	public:
		 RuntimeId	GetRuntimeId()override { return RuntimeId::RuntimeIdUIInput; }
		class CharInput :public CharEventListener {
		public:
			void OnChar(CharEvent&e)override;
		};

		class StateListener :public StateChangeListener {
		public:
			void OnStateChange(StateChangeEvent&e)override;
		};
	};


}