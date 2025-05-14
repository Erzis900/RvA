//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// StyleAsCode exporter v2.0 - Style data exported as a values array            //
//                                                                              //
// USAGE: On init call: GuiLoadStyleRva();                                   //
//                                                                              //
// more info and bugs-report:  github.com/raysan5/raygui                        //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2020-2025 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define RVA_STYLE_PROPS_COUNT 22

// Custom style name: rva
static const GuiStyleProp rvaStyleProps[RVA_STYLE_PROPS_COUNT] = {
	{0, 0, (int)0x878787ff},  // DEFAULT_BORDER_COLOR_NORMAL
	{0, 1, (int)0x001531ff},  // DEFAULT_BASE_COLOR_NORMAL
	{0, 2, (int)0xc3c3c3ff},  // DEFAULT_TEXT_COLOR_NORMAL
	{0, 3, (int)0xe1e1e1ff},  // DEFAULT_BORDER_COLOR_FOCUSED
	{0, 4, (int)0x848484ff},  // DEFAULT_BASE_COLOR_FOCUSED
	{0, 5, (int)0x181818ff},  // DEFAULT_TEXT_COLOR_FOCUSED
	{0, 6, (int)0x000000ff},  // DEFAULT_BORDER_COLOR_PRESSED
	{0, 7, (int)0xefefefff},  // DEFAULT_BASE_COLOR_PRESSED
	{0, 8, (int)0x202020ff},  // DEFAULT_TEXT_COLOR_PRESSED
	{0, 9, (int)0x6a6a6aff},  // DEFAULT_BORDER_COLOR_DISABLED
	{0, 10, (int)0x818181ff}, // DEFAULT_BASE_COLOR_DISABLED
	{0, 11, (int)0x606060ff}, // DEFAULT_TEXT_COLOR_DISABLED
	{0, 16, (int)0x00000010}, // DEFAULT_TEXT_SIZE
	{0, 18, (int)0x9d9d9dff}, // DEFAULT_LINE_COLOR
	{0, 19, (int)0x1e2e49ff}, // DEFAULT_BACKGROUND_COLOR
	{0, 20, (int)0x00000018}, // DEFAULT_TEXT_LINE_SPACING
	{1, 5, (int)0xf7f7f7ff},  // LABEL_TEXT_COLOR_FOCUSED
	{1, 8, (int)0x898989ff},  // LABEL_TEXT_COLOR_PRESSED
	{4, 5, (int)0xb0b0b0ff},  // SLIDER_TEXT_COLOR_FOCUSED
	{5, 5, (int)0x848484ff},  // PROGRESSBAR_TEXT_COLOR_FOCUSED
	{9, 5, (int)0xf5f5f5ff},  // TEXTBOX_TEXT_COLOR_FOCUSED
	{10, 5, (int)0xf6f6f6ff}, // VALUEBOX_TEXT_COLOR_FOCUSED
};

// Style loading function: rva
static void GuiLoadStyleRva(void) {
	// Load style properties provided
	// NOTE: Default properties are propagated
	for (int i = 0; i < RVA_STYLE_PROPS_COUNT; i++) {
		GuiSetStyle(rvaStyleProps[i].controlId, rvaStyleProps[i].propertyId, rvaStyleProps[i].propertyValue);
	}

	//-----------------------------------------------------------------

	GuiSetStyle(0, BORDER_WIDTH, 1);
	// TODO: Custom user style setup: Set specific properties here (if required)
	// i.e. Controls specific BORDER_WIDTH, TEXT_PADDING, TEXT_ALIGNMENT
}
