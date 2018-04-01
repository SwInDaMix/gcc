# Custom firmware for toy oscilloscope DS203
This is one of my pretty ambitious large project using C++ on Cortex-M3 microcontroller. It's purpose to build a graphical, windows-like, with user friendly interface and more convenient control.

Currently more part of the planned UI features is ready:

## UI teaser on actual device
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaOscilloscope/UI_teaser1.jpg)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaOscilloscope/UI_teaser2.jpg)

## UI teaser on windows emulator
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaOscilloscope/UI_teaser_on_emulator1.png)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaOscilloscope/UI_teaser_on_emulator2.png)

## How this is created programmatically
All UI hierarchy is created programmatically:

```c++
CWindow *_window = new CWindow(SPoint(15, 15), SSize(_width >> 1, _height >> 1), nullptr, "Main window", CWindow::EStyleFlags::Border);
CStack *_stack = new CStack(*_window, nullptr, EOrientation::Vertical, EAlignmentEx::Stretch, CStack::EAlignmentStack::FillFirst, false);
new CPrimitiveSpace(*(new CBorder(*_stack, 2, CBorder::EType::Thin3DOutside, EColor::Cyan)), SSize(200, 28));
new CPrimitiveSpace(*(new CBorder(*_stack, 2, CBorder::EType::StaticOutside, EColor::_Invalid)), SSize(230, 21));
new CPrimitiveSpace(*(new CBorder(*_stack, 2, CBorder::EType::StaticInside, EColor::_Invalid)), SSize(180, 33));
new CPrimitiveSeparator(*_stack, EOrientation::Horizontal, true);
CStack *_stack2 = new CStack(*_stack, nullptr, EOrientation::Horizontal, EAlignmentEx::Stretch, CStack::EAlignmentStack::Spread, false);
new CButton(*_stack2);
new CButton(*_stack2, &_img16_cpma, EColor::Black, nullptr);
new CPrimitiveSeparator(*_stack2, EOrientation::Vertical, false);
CButton *_btn = new CButton(*_stack2, nullptr, nullptr, "If you wanna\nfly so hight -\nKiss my ass,\nand say good-byte"); _btn->ColorForeground_set(EColor::White); _btn->FontIsShadow_set(true);
new CButton(*_stack2, &_img16_cpma, EColor::Black, "If you wanna\nfly so hight -\nKiss my ass,\nand say good-byte");
CStack *_stack3 = new CStack(*_stack, nullptr, EOrientation::Horizontal, EAlignmentEx::Middle, CStack::EAlignmentStack::Middle, false);
new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, nullptr, nullptr);
new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, nullptr, EColor::Yellow);
new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, EColor::Red, nullptr);
new CPrimitiveImage(*_stack3, EAlignment::Middle, EAlignment::Middle, CFrame::s_img1_frame_close, EColor::Green, EColor::White);
CStack *_stack4 = new CStack(*_stack, nullptr, EOrientation::Horizontal, EAlignmentEx::Middle, CStack::EAlignmentStack::Middle, false);
new CCheckBox(*_stack4, true, nullptr, "Inactive");
new CCheckBox(*_stack4, false, true, "Checked");
new CCheckBox(*_stack4, false, false, "Unchecked");
_window->RepaintWithBackground();

// all can be deleted by single delete
delete _window;
```

Currently this project is frozen (not enough time to evolve it), but hope recently I'll find more time to continue.