/*
 * Copyright (C) 2004, 2005, 2006 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Collabora, Ltd.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "CompositionUnderline.h"
#include "KeypressCommand.h"
#include "PlatformEvent.h"
#include <wtf/WindowsExtras.h>
#include <wtf/text/WTFString.h>

#if PLATFORM(COCOA)
#include <wtf/RetainPtr.h>
OBJC_CLASS NSEvent;
#endif

#if PLATFORM(IOS_FAMILY)
OBJC_CLASS WebEvent;
#endif

#if PLATFORM(JAVA)
#include <jni.h>
#endif

namespace WebCore {

    class PlatformKeyboardEvent : public PlatformEvent {
        WTF_MAKE_FAST_ALLOCATED;
    public:
        PlatformKeyboardEvent()
            : PlatformEvent(PlatformEvent::KeyDown)
        {
        }

        PlatformKeyboardEvent(Type type, const String& text, const String& unmodifiedText, const String& key, const String& code,
        const String& keyIdentifier, int windowsVirtualKeyCode, bool isAutoRepeat, bool isKeypad, bool isSystemKey, OptionSet<Modifier> modifiers, WallTime timestamp)
            : PlatformEvent(type, modifiers, timestamp)
            , m_autoRepeat(isAutoRepeat)
            , m_isKeypad(isKeypad)
            , m_isSystemKey(isSystemKey)
            , m_text(text)
            , m_unmodifiedText(unmodifiedText)
            , m_key(key)
            , m_code(code)
            , m_keyIdentifier(keyIdentifier)
            , m_windowsVirtualKeyCode(windowsVirtualKeyCode)
        {
        }

        WEBCORE_EXPORT void disambiguateKeyDownEvent(Type, bool backwardCompatibilityMode = false); // Only used on platforms that need it, i.e. those that generate KeyDown events.

        // Text as as generated by processing a virtual key code with a keyboard layout
        // (in most cases, just a character code, but the layout can emit several
        // characters in a single keypress event on some platforms).
        // This may bear no resemblance to the ultimately inserted text if an input method
        // processes the input.
        // Will be null for KeyUp and RawKeyDown events.
        String text() const { return m_text; }

        // Text that would have been generated by the keyboard if no modifiers were pressed
        // (except for Shift); useful for shortcut (accelerator) key handling.
        // Otherwise, same as text().
        String unmodifiedText() const { return m_unmodifiedText; }

        String keyIdentifier() const { return m_keyIdentifier; }
        const String& key() const { return m_key; }
        const String& code() const { return m_code; }

        // Most compatible Windows virtual key code associated with the event.
        // Zero for Char events.
        int windowsVirtualKeyCode() const { return m_windowsVirtualKeyCode; }
        void setWindowsVirtualKeyCode(int code) { m_windowsVirtualKeyCode = code; }

#if USE(APPKIT) || PLATFORM(IOS_FAMILY) || PLATFORM(GTK) || USE(LIBWPE)
        bool handledByInputMethod() const { return m_handledByInputMethod; }
#endif
#if PLATFORM(GTK) || USE(LIBWPE)
        const std::optional<Vector<WebCore::CompositionUnderline>>& preeditUnderlines() const { return m_preeditUnderlines; }
        const std::optional<uint64_t>& preeditSelectionRangeStart() const { return m_preeditSelectionRangeStart; }
        const std::optional<uint64_t>& preeditSelectionRangeLength() const { return m_preeditSelectionRangeLength; }
#endif
#if USE(APPKIT)
        const Vector<KeypressCommand>& commands() const { return m_commands; }
#elif PLATFORM(GTK)
        const Vector<String>& commands() const { return m_commands; }
#endif

        bool isAutoRepeat() const { return m_autoRepeat; }
        bool isKeypad() const { return m_isKeypad; }
        bool isSystemKey() const { return m_isSystemKey; }

        bool isSyntheticEvent() const { return m_isSyntheticEvent; }
        void setIsSyntheticEvent() { m_isSyntheticEvent = true; }

        WEBCORE_EXPORT static bool currentCapsLockState();
        WEBCORE_EXPORT static void getCurrentModifierState(bool& shiftKey, bool& ctrlKey, bool& altKey, bool& metaKey);
        WEBCORE_EXPORT static void setCurrentModifierState(OptionSet<Modifier>);
        WEBCORE_EXPORT static OptionSet<Modifier> currentStateOfModifierKeys();

#if PLATFORM(COCOA)
#if !PLATFORM(IOS_FAMILY)
        NSEvent* macEvent() const { return m_macEvent.get(); }
#else
        ::WebEvent *event() const { return m_Event.get(); }
#endif
#endif

#if PLATFORM(WIN)
        PlatformKeyboardEvent(HWND, WPARAM, LPARAM, Type, bool);
#endif

#if PLATFORM(GTK)
        // Used by WebKit2
        static String keyValueForGdkKeyCode(unsigned);
        static String keyCodeForHardwareKeyCode(unsigned);
        static String keyIdentifierForGdkKeyCode(unsigned);
        static int windowsKeyCodeForGdkKeyCode(unsigned);
        static String singleCharacterString(unsigned);
        static bool modifiersContainCapsLock(unsigned);
#endif

#if PLATFORM(JAVA)
        PlatformKeyboardEvent(jint type, jstring text, jstring keyIdentifier,
                              jint windowsVirtualKeyCode, jboolean shift,
                              jboolean ctrl, jboolean alt, jboolean meta, jdouble timestamp);
#endif

#if USE(LIBWPE)
        static String keyValueForWPEKeyCode(unsigned);
        static String keyCodeForHardwareKeyCode(unsigned);
        static String keyIdentifierForWPEKeyCode(unsigned);
        static int windowsKeyCodeForWPEKeyCode(unsigned);
        static String singleCharacterString(unsigned);
#endif

    protected:
        bool m_autoRepeat { false };
        bool m_isKeypad { false };
        bool m_isSystemKey { false };

        String m_text;
        String m_unmodifiedText;
        String m_key;
        String m_code;
        String m_keyIdentifier;
        int m_windowsVirtualKeyCode { 0 };

        bool m_isSyntheticEvent { false };
#if USE(APPKIT) || PLATFORM(IOS_FAMILY) || PLATFORM(GTK) || USE(LIBWPE)
        bool m_handledByInputMethod { false };
#endif
#if PLATFORM(GTK) || USE(LIBWPE)
        std::optional<Vector<WebCore::CompositionUnderline>> m_preeditUnderlines;
        std::optional<uint64_t> m_preeditSelectionRangeStart;
        std::optional<uint64_t> m_preeditSelectionRangeLength;
#endif
#if USE(APPKIT)
        Vector<KeypressCommand> m_commands;
#elif PLATFORM(GTK)
        Vector<String> m_commands;
#endif

#if PLATFORM(COCOA)
#if !PLATFORM(IOS_FAMILY)
        RetainPtr<NSEvent> m_macEvent;
#else
        RetainPtr<::WebEvent> m_Event;
#endif
#endif
        // The modifier state is optional, since it is not needed in the UI process or in legacy WebKit.
        static std::optional<OptionSet<Modifier>> s_currentModifiers;
    };

} // namespace WebCore
