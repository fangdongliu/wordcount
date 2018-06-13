#include"UIInput.h"
#include"uiwindow.h"
#include<algorithm>
using namespace AnimUI;

#undef min
#undef max

namespace {
	inline int RoundToInt(float x)
	{
		return static_cast<int>(floor(x + .5));
	}
	inline bool IsHighSurrogate(UINT32 ch) throw()
	{
		// 0xD800 <= ch <= 0xDBFF
		return (ch & 0xFC00) == 0xD800;
	}


	inline bool IsLowSurrogate(UINT32 ch) throw()
	{
		// 0xDC00 <= ch <= 0xDFFF
		return (ch & 0xFC00) == 0xDC00;
	}
}

UIInput::UIInput() {
	caretPos = 0;
	caretPositionOffset = 0;
	caretAnchor = 0;
	isFocusable = true;
	isInputMode = false;
	stateChangeListener = new StateListener;
	keyEventListener = new KeyInput;
	mouseEventListener = new MouseInput;
	charEventListener = new CharInput;
	currentStyle.fontFamily = L"ËÎÌו";
	ToDefaultState();
}

void UIInput::UpdateSystemCaret(UIWindow*window,const D2D1_RECT_F& rect)
{
	// Moves the system caret to a new position.

	// Although we don't actually use the system caret (drawing our own
	// instead), this is important for accessibility, so the magnifier
	// can follow text we type. The reason we draw our own directly
	// is because intermixing DirectX and GDI content (the caret) reduces
	// performance.

	// Gets the current caret position (in untransformed space).
	if (GetFocus() != window->GetHWnd()) // Only update if we have focus.
		return;


	float width = (rect.right - rect.left);
	float height = (rect.bottom - rect.top);

	// Update the caret's location, rounding to nearest integer so that
	// it lines up with the text selection.

	int intX = RoundToInt(rect.left+this->rect.X+currentStyle.padding.Left);
	int intY = RoundToInt(rect.top+this->rect.Y+currentStyle.padding.Top);
	int intWidth = RoundToInt(width);
	int intHeight = RoundToInt(rect.top + this->rect.Y + height +currentStyle.padding.Top) - intY;


	window->CreateCaret(intWidth, intHeight);
	SetCaretPos(intX, intY);
	window->ShowCaret();

//	window->solidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
//	window->currentRenderTarget->FillRectangle(D2D1::RectF(intX,intY,intX+intWidth,intY+intHeight), window->solidColorBrush);

	// Don't actually call ShowCaret. It's enough to just set its position.
}

DWRITE_TEXT_RANGE UIInput::GetSelectionRange() {
	UINT32 caretBegin = caretAnchor;
	UINT32 caretEnd = caretPosition + caretPositionOffset;
	if (caretBegin > caretEnd)
		std::swap(caretBegin, caretEnd);

	// Limit to actual text length.
	UINT32 textLength = static_cast<UINT32>(contentText.length());
	caretBegin = std::min(caretBegin, textLength);
	caretEnd = std::min(caretEnd, textLength);

	DWRITE_TEXT_RANGE textRange = { caretBegin, caretEnd - caretBegin };
	return textRange;
}

void UIInput::AlignCaretToNearestCluster(bool isTrailingHit, bool skipZeroWidth)
{
	// Uses hit-testing to align the current caret position to a whole cluster,
	// rather than residing in the middle of a base character + diacritic,
	// surrogate pair, or character + UVS.

	DWRITE_HIT_TEST_METRICS hitTestMetrics;
	float caretX, caretY;

	// Align the caret to the nearest whole cluster.
	textLayout->HitTestTextPosition(
		caretPosition,
		false,
		&caretX,
		&caretY,
		&hitTestMetrics
	);

	// The caret position itself is always the leading edge.
	// An additional offset indicates a trailing edge when non-zero.
	// This offset comes from the number of code-units in the
	// selected cluster or surrogate pair.
	caretPosition = hitTestMetrics.textPosition;
	caretPositionOffset = (isTrailingHit) ? hitTestMetrics.length : 0;

	// For invisible, zero-width characters (like line breaks
	// and formatting characters), force leading edge of the
	// next position.
	if (skipZeroWidth && hitTestMetrics.width == 0)
	{
		caretPosition += caretPositionOffset;
		caretPositionOffset = 0;
	}
}


void UIInput::CopyToClipboard(UIWindow*window)
{
	// Copies selected text to clipboard.

	DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
	if (selectionRange.length <= 0)
		return;

	// Open and empty existing contents.
	if (OpenClipboard(window->GetHWnd()))
	{
		if (EmptyClipboard())
		{
			// Allocate room for the text
			size_t byteSize = sizeof(wchar_t) * (selectionRange.length + 1);
			HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, byteSize);

			if (hClipboardData != NULL)
			{
				void* memory = GlobalLock(hClipboardData);  // [byteSize] in bytes

				if (memory != NULL)
				{
					// Copy text to memory block.
					const wchar_t* text = contentText.c_str();
					memcpy(memory, &text[selectionRange.startPosition], byteSize);
					GlobalUnlock(hClipboardData);

					if (SetClipboardData(CF_UNICODETEXT, hClipboardData) != NULL)
					{
						hClipboardData = NULL; // system now owns the clipboard, so don't touch it.
					}
				}
				GlobalFree(hClipboardData); // free if failed
			}
		}
		CloseClipboard();
	}
}



void UIInput::GetCaretRect(D2D1_RECT_F& rect)
{
	// Gets the current caret position (in untransformed space).

	D2D1_RECT_F zeroRect = {};
	rect = zeroRect;

	if (textLayout == NULL)
		return;

	// Translate text character offset to point x,y.
	DWRITE_HIT_TEST_METRICS caretMetrics;
	float caretX, caretY;

	textLayout->HitTestTextPosition(
		caretPosition,
		caretPositionOffset > 0, // trailing if nonzero, else leading edge
		&caretX,
		&caretY,
		&caretMetrics
	);

	// If a selection exists, draw the caret using the
	// line size rather than the font size.
	
	// The default thickness of 1 pixel is almost _too_ thin on modern large monitors,
	// but we'll use it.
	DWORD caretIntThickness = 2;
	SystemParametersInfo(SPI_GETCARETWIDTH, 0, &caretIntThickness, FALSE);
	const float caretThickness = float(caretIntThickness);

	// Return the caret rect, untransformed.
	rect.left = caretX - caretThickness / 2.0f;
	rect.right = rect.left + caretThickness;
	rect.top = caretY;
	rect.bottom = caretY + caretMetrics.height;
}


void UIInput::ToDefaultState() {
	DWRITE_HIT_TEST_METRICS metrics;
}

void UIInput::ToInputState() {

}

void UIInput::PasteFromClipboard(UIWindow*window)
{
	// Pastes text from clipboard at current caret position.

	DeleteSelection();

	UINT32 characterCount = 0;

	// Copy Unicode text from clipboard.

	if (OpenClipboard(window->GetHWnd()))
	{
		HGLOBAL hClipboardData = GetClipboardData(CF_UNICODETEXT);

		if (hClipboardData != NULL)
		{
			// Get text and size of text.
			size_t byteSize = GlobalSize(hClipboardData);
			void* memory = GlobalLock(hClipboardData); // [byteSize] in bytes
			const wchar_t* text = reinterpret_cast<const wchar_t*>(memory);
			characterCount = static_cast<UINT32>(wcsnlen(text, byteSize / sizeof(wchar_t)));

			if (memory != NULL)
			{
				// Insert the text at the current position.
				contentText.insert(caretPosition + caretPositionOffset, text,characterCount);
				GlobalUnlock(hClipboardData);
			}
		}
		CloseClipboard();
	}

	caretPosition = caretPosition + caretPositionOffset + characterCount;
	caretAnchor = caretPosition;
	caretPositionOffset = 0;
	NeedUpdate();
}

void UIInput::OnKey(KeyEvent&e) {
	if (keyEventListener) {
		keyEventListener->OnKey(e);
	}
	auto absPosition = caretPosition + caretPositionOffset;
	switch (e.GetKeyCode()) {
	//	case VK_RETURN:
	//		// Insert CR/LF pair
	//		break;

		case VK_BACK:

			if (absPosition != caretAnchor)
			{
				// delete the selected text
				DeleteSelection();
			
			}
			else if (absPosition > 0)
			{
				UINT32 count = 1;
				// Need special case for surrogate pairs and CR/LF pair.
				if (absPosition >= 2
					&& absPosition <= contentText.size())
				{
					wchar_t charBackOne = contentText[absPosition - 1];
					wchar_t charBackTwo = contentText[absPosition - 2];
					if ((IsLowSurrogate(charBackOne) && IsHighSurrogate(charBackTwo))
						|| (charBackOne == '\n' && charBackTwo == '\r'))
					{
						count = 2;
					}
				}
				caretPosition = absPosition- count;
				caretAnchor = caretPosition;
				caretPositionOffset = 0;
				contentText.erase(caretPosition,count);
				NeedUpdate();
				e.EventHandled();
			}
			break;

		case VK_DELETE:
			// Delete following cluster.
		{
			caretPosition = caretAnchor=absPosition;
			AlignCaretToNearestCluster(true, true);

			// special check for CR/LF pair
			absPosition = caretPosition + caretPositionOffset;
			int prev = caretPosition;
			if (absPosition >= 1
				&& absPosition < contentText.size()
				&& contentText[absPosition - 1] == '\r'
				&&  contentText[absPosition] == '\n')
			{
				caretPosition = absPosition + 1;
				AlignCaretToNearestCluster(false, true);

			}
			contentText.erase(prev, caretPosition + caretPositionOffset - prev);
			caretPosition = prev;
			caretPositionOffset = 0;
			NeedUpdate();
			e.EventHandled();
		}
			break;


		case VK_LEFT: // seek left one cluster
			if (absPosition > 0)
			{
				UINT32 count = 1;
				// Need special case for surrogate pairs and CR/LF pair.
				if (absPosition >= 2
					&& absPosition <= contentText.size())
				{
					wchar_t charBackOne = contentText[absPosition - 1];
					wchar_t charBackTwo = contentText[absPosition - 2];
					if ((IsLowSurrogate(charBackOne) && IsHighSurrogate(charBackTwo))
						|| (charBackOne == '\n' && charBackTwo == '\r'))
					{
						count = 2;
					}
				}
				caretPosition = absPosition - count;
				caretAnchor = caretPosition;
				caretPositionOffset = 0;
				NeedUpdate();
				e.EventHandled();
			}
			break;

		case VK_RIGHT: // seek right one cluster
			caretPosition = absPosition;
			AlignCaretToNearestCluster(true, true);

			// special check for CR/LF pair
			absPosition = caretPosition + caretPositionOffset;
			if (absPosition >= 1
				&& absPosition < contentText.size()
				&& contentText[absPosition - 1] == '\r'
				&&  contentText[absPosition] == '\n')
			{
				caretPosition = absPosition + 1;
				AlignCaretToNearestCluster(false, true);
				
			}
			caretAnchor = caretPosition+caretPositionOffset;
			NeedUpdate();
			e.EventHandled();
			break;


		case 'C':
			if (e.IsHeldCtrl()) {
				CopyToClipboard(e.GetDestWindow());
			}
			break;


		case 'V':
			if (e.IsHeldCtrl())
				PasteFromClipboard(e.GetDestWindow());
			break;

		case 'X':
			if (e.IsHeldCtrl())
			{
				CopyToClipboard(e.GetDestWindow());
				DeleteSelection();
			}
			break;

		case 'A':
			if (e.IsHeldCtrl()) {
				caretAnchor = 0;
				caretPositionOffset = 0;
				caretPosition = contentText.length();
				e.EventHandled();
			}
			break;
	}
}

void UIInput::DeleteSelection() {
	DWRITE_TEXT_RANGE selectionRange = GetSelectionRange();
	if (selectionRange.length <= 0)
		return;

	contentText.erase(selectionRange.startPosition, selectionRange.length);
	caretPosition = caretAnchor = selectionRange.startPosition;
	caretPositionOffset = 0;
	NeedUpdate();
}

void UIInput::KeyInput::OnKey(KeyEvent&e) {
	auto i = dynamic_cast<UIInput*>(e.GetDestElement());
	i->OnKey(e);
	e.EventHandled();
}

void UIInput::CharInput::OnChar(CharEvent&e) {

	if (e.GetCharCode() >= 0x20 || e.GetCharCode() == 9)
	{
		auto i = dynamic_cast<UIInput*>(e.GetDestElement());
		// Replace any existing selection.
		i->DeleteSelection();

		// Convert the UTF32 character code from the Window message to UTF16,
		// yielding 1-2 code-units. Then advance the caret position by how
		// many code-units were inserted.

		UINT32 charsLength = 1;
		wchar_t chars[2] = { static_cast<wchar_t>(e.GetCharCode()), 0 };

		// If above the basic multi-lingual plane, split into
		// leading and trailing surrogates.
		if (e.GetCharCode() > 0xFFFF)
		{
			chars[0] = wchar_t(0xD800 + (e.GetCharCode() >> 10) - (0x10000 >> 10));
			chars[1] = wchar_t(0xDC00 + (e.GetCharCode() & 0x3FF));
			charsLength++;
		}
		i->contentText.insert(i->caretPosition + i->caretPositionOffset, chars,charsLength);
		i->caretPosition = i->caretPosition+i->caretPositionOffset + charsLength;
		i->caretAnchor = i->caretPosition;
		i->caretPositionOffset = 0;

		

		i->NeedUpdate();
	}
	e.EventHandled();
}

void UIInput::PaintSelectRange(UIWindow*destWindow) {

	if (destWindow->GetFocusElement() != this)
		return;

	D2D1_RECT_F caretRect;
	GetCaretRect(caretRect);
	UpdateSystemCaret(destWindow, caretRect);


	auto range = GetSelectionRange();
	UINT32 actualHitTestCount = 0;
	if (range.length >0) {
		textLayout->HitTestTextRange(range.startPosition, range.length, 0, 0, 0, 0, &actualHitTestCount);
		std::vector<DWRITE_HIT_TEST_METRICS>hitTestMetrics(actualHitTestCount);
		textLayout->HitTestTextRange(range.startPosition, range.length,rect.X+currentStyle.padding.Left,rect.Y+currentStyle.padding.Top, &hitTestMetrics[0], static_cast<UINT32>(hitTestMetrics.size()), &actualHitTestCount);
		destWindow->solidColorBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));
		for (int i = 0; i < actualHitTestCount; i++) {
			const DWRITE_HIT_TEST_METRICS& htm = hitTestMetrics[i];
			D2D1_RECT_F highlightRect = {
				htm.left,
				htm.top,
				(htm.left + htm.width),
				(htm.top + htm.height)
			};
			
			
			destWindow->currentRenderTarget->FillRectangle(highlightRect,destWindow->solidColorBrush);
		}
	}
	
}

void UIInput::OnPaint(UIWindow*destWindow) {

	PaintBackground(destWindow);

	PaintSelectRange(destWindow);


	auto brush = destWindow->solidColorBrush;
	brush->SetColor(currentStyle.textColor);

	if (textLayout)
		destWindow->currentRenderTarget->DrawTextLayout(D2D1::Point2F(rect.X + currentStyle.padding.Left, rect.Y + currentStyle.padding.Top),
			textLayout, brush);

	PaintBorder(destWindow);

}

void UIInput::MouseInput::OnMouse(MouseEvent&e) {

	bool heldShift = GetAsyncKeyState(VK_SHIFT & 0x80)!=0;
	auto i = dynamic_cast<UIInput*>(e.GetDestElement());

	switch (e.GetEventType()) {
	case MouseEventType::MouseEventLButtonDown:
	{
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		BOOL isTrailing;
		BOOL isInside;
		i->textLayout->HitTestPoint(e.GetPoint().X - i->rect.X, e.GetPoint().Y - i->rect.Y, &isTrailing, &isInside, &metrics);
		i->caretPosition = metrics.textPosition;
		if (isTrailing) {
			i->AlignCaretToNearestCluster(true);
		}
		else {
			i->caretPositionOffset = 0;
		}
		

		if (!heldShift) {
			i->caretAnchor = i->caretPosition+i->caretPositionOffset;
		}

		D2D1_RECT_F caretRect;
		i->GetCaretRect(caretRect);
		i->UpdateSystemCaret(e.GetDestWindow(), caretRect);

		
	}
		break;
	case  MouseEventType::MouseEventMouseMove:
	{
		
		auto i = dynamic_cast<UIInput*>(e.GetDestElement());
		if (e.GetDestWindow()->GetCaptureElement() != i) {
			return;
		}
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		BOOL isTrailing;
		BOOL isInside;
		i->textLayout->HitTestPoint(e.GetPoint().X - i->rect.X, e.GetPoint().Y - i->rect.Y, &isTrailing, &isInside, &metrics);
		i->caretPosition = metrics.textPosition;
		if (isTrailing) {
			i->AlignCaretToNearestCluster(true);
		}
		else {
			i->caretPositionOffset = 0;
		}
		D2D1_RECT_F caretRect;
		i->GetCaretRect(caretRect);
		i->UpdateSystemCaret(e.GetDestWindow(), caretRect);
		
	}
		break;
	case MouseEventLButtonUp:

		break;
	}

}
void UIInput::MouseInput::OnMouseOut(UIElement*e) {

}

void UIInput::StateListener::OnStateChange(StateChangeEvent&e){
	auto i = dynamic_cast<UIInput*>(e.GetDestElement());
	switch (e.GetDestState()) {
	case UIStateNormal:
		e.GetDestWindow()->HideCaret();
		break;
	case UIStateFocus:
		e.GetDestWindow()->ShowCaret();
		break;
	}
}