/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_SCREEN_WINDOW_HPP
#define XCSOAR_SCREEN_WINDOW_HPP

#include "Screen/Font.hpp"
#include "Thread/Debug.hpp"

#ifdef ENABLE_SDL
#include "Screen/BufferCanvas.hpp"
#include "Screen/Timer.hpp"
#endif /* ENABLE_SDL */

class ContainerWindow;

/**
 * A Window is a portion on the screen which displays something, and
 * which optionally interacts with the user.  To draw custom graphics
 * into a Window, derive your class from #PaintWindow.
 */
class Window {
public:
#ifdef ENABLE_SDL
  friend class SDLTimer;
  typedef SDLTimer *timer_t;
#else
  typedef UINT_PTR timer_t;
#endif

protected:
#ifdef ENABLE_SDL
  ContainerWindow *parent;
  int left, top;
  BufferCanvas canvas;

  bool focused;
#else
  HWND hWnd;
  WNDPROC prev_wndproc;
#endif

private:
  /* copy constructor not allowed */
  Window(const Window &window) {}
  Window &operator=(const Window &window) { return *this; }

public:
#ifdef ENABLE_SDL
  Window():parent(NULL), focused(false) {}
#else
  Window():hWnd(NULL), prev_wndproc(NULL) {}
#endif
  virtual ~Window();

#ifndef ENABLE_SDL
  operator HWND() const {
    return hWnd;
  };

  /**
   * Is it this window or one of its descendants?
   */
  bool identify_descendant(HWND h) const {
    return h == hWnd || ::IsChild(hWnd, h);
  }
#endif

public:
  bool defined() const {
#ifdef ENABLE_SDL
    return canvas.defined();
#else
    return hWnd != NULL;
#endif
  }

#ifdef ENABLE_SDL
  void clear_parent() {
    parent = NULL;
  }

  int get_top() const {
    return top;
  }

  int get_left() const {
    return left;
  }

  unsigned get_width() const {
    return canvas.get_width();
  }

  unsigned get_height() const {
    return canvas.get_height();
  }

  int get_right() const {
    return get_left() + get_width();
  }

  int get_bottom() const {
    return get_top() + get_height();
  }

  int get_hmiddle() const {
    return (get_left() + get_right()) / 2;
  }

  int get_vmiddle() const {
    return (get_top() + get_bottom()) / 2;
  }

  const Canvas &get_canvas() const {
    return canvas;
  }
#endif

  void set(ContainerWindow *parent, LPCTSTR cls, LPCTSTR text,
           int left, int top, unsigned width, unsigned height,
           DWORD style, DWORD ex_style=0);

  void set(ContainerWindow *parent, LPCTSTR cls, LPCTSTR text,
           int left, int top, unsigned width, unsigned height,
           bool center = false, bool notify = false, bool show = true,
           bool tabstop = false, bool border = false);

#ifndef ENABLE_SDL
  void created(HWND _hWnd);
#endif

  void reset();

  /**
   * Determines the root owner window of this Window.  This is
   * probably a pointer to the #MainWindow instance.
   */
  ContainerWindow *get_root_owner();

  void move(int left, int top) {
    assert_none_locked();

#ifdef ENABLE_SDL
    this->left = left;
    this->top = top;
#else
    ::SetWindowPos(hWnd, NULL, left, top, 0, 0,
                   SWP_NOSIZE | SWP_NOZORDER |
                   SWP_NOACTIVATE | SWP_NOOWNERZORDER);
#endif
  }

  void move(int left, int top, unsigned width, unsigned height) {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else /* !ENABLE_SDL */
    ::SetWindowPos(hWnd, NULL, left, top, width, height,
                   SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    // XXX store new size?
#endif
  }

  void resize(unsigned width, unsigned height) {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else /* !ENABLE_SDL */
    ::SetWindowPos(hWnd, NULL, 0, 0, width, height,
                   SWP_NOMOVE | SWP_NOZORDER |
                   SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    // XXX store new size?
#endif
  }

  void insert_after(HWND hWnd2, bool show=true) {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else
    ::SetWindowPos(hWnd, hWnd2, 0, 0, 0, 0,
                   SWP_NOMOVE|SWP_NOSIZE|(show?SWP_SHOWWINDOW:SWP_HIDEWINDOW));
#endif
  }

  void bring_to_top() {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else
    ::BringWindowToTop(hWnd);
#endif
  }

  void show_on_top() {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else
    ::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0,
                   SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
#endif
  }

  void set_font(const Font &font) {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else
    ::SendMessage(hWnd, WM_SETFONT,
                  (WPARAM)font.native(), MAKELPARAM(TRUE,0));
#endif
  }

  bool is_visible() const {
#ifdef ENABLE_SDL
    return true; // XXX
#else
    return ::IsWindowVisible(hWnd);
#endif
  }

  void show() {
#ifdef ENABLE_SDL
    // XXX
#else
    ::ShowWindow(hWnd, SW_SHOW);
#endif
  }

  void hide() {
#ifdef ENABLE_SDL
    // XXX
#else
    ::ShowWindow(hWnd, SW_HIDE);
#endif
  }

  void set_visible(bool visible) {
    if (visible)
      show();
    else
      hide();
  }

  /**
   * Can this window get user input?
   */
  bool is_enabled() const {
#ifdef ENABLE_SDL
    return true;
#else
    return ::IsWindowEnabled(hWnd);
#endif
  }

  /**
   * Specifies whether this window can get user input.
   */
  void set_enabled(bool enabled) {
#ifdef ENABLE_SDL
    // XXX
#else
    ::EnableWindow(hWnd, enabled);
#endif
  }

#ifdef ENABLE_SDL

  virtual Window *get_focused_window();
  void set_focus();

#else /* !ENABLE_SDL */

  void set_focus() {
    assert_none_locked();

    ::SetFocus(hWnd);
  }

#endif /* !ENABLE_SDL */

  bool has_focus() const {
#ifdef ENABLE_SDL
    return focused;
#else
    return hWnd == ::GetFocus();
#endif
  }

  bool has_capture() const {
#ifdef ENABLE_SDL
    return false; // XXX
#else
    return ::GetCapture() == hWnd;
#endif
  }

  void set_capture() {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else
    ::SetCapture(hWnd);
#endif
  }

  void release_capture() {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else
    ::ReleaseCapture();
#endif
  }

#ifndef ENABLE_SDL
  WNDPROC set_wndproc(WNDPROC wndproc)
  {
    return (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)wndproc);
  }

  void set_userdata(void *value)
  {
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)value);
  }
#endif /* !ENABLE_SDL */

  timer_t set_timer(unsigned id, unsigned ms)
  {
#ifdef ENABLE_SDL
    return new SDLTimer(*this, ms);
#else
    ::SetTimer(hWnd, id, ms, NULL);
    return id;
#endif
  }

  void kill_timer(timer_t id)
  {
#ifdef ENABLE_SDL
    delete id;
#else
    ::KillTimer(hWnd, id);
#endif
  }

#ifdef ENABLE_SDL
  void to_screen(RECT &rc) const;
#endif

  /**
   * Returns the position on the screen.
   */
  const RECT get_screen_position() const
  {
    RECT rc;
#ifdef ENABLE_SDL
    rc = get_position();
    to_screen(rc);
#else
    ::GetWindowRect(hWnd, &rc);
#endif
    return rc;
  }

  /**
   * Returns the position within the parent window.
   */
  const RECT get_position() const
  {
    RECT rc;
#ifdef ENABLE_SDL
    rc.left = get_left();
    rc.top = get_top();
    rc.right = get_width();
    rc.bottom = get_height();
#else
    rc = get_screen_position();

    HWND parent = ::GetParent(hWnd);
    if (parent != NULL) {
      POINT pt;

      pt.x = rc.left;
      pt.y = rc.top;
      ::ScreenToClient(parent, &pt);
      rc.left = pt.x;
      rc.top = pt.y;

      pt.x = rc.right;
      pt.y = rc.bottom;
      ::ScreenToClient(parent, &pt);
      rc.right = pt.x;
      rc.bottom = pt.y;
    }
#endif
    return rc;
  }

  const RECT get_client_rect() const
  {
    RECT rc;
#ifdef ENABLE_SDL
    rc.left = 0;
    rc.top = 0;
    rc.right = get_width();
    rc.bottom = get_height();
#else
    ::GetClientRect(hWnd, &rc);
#endif
    return rc;
  }

  bool in_client_rect(int x, int y) const {
    if (x < 0 || y < 0)
      return false;

    RECT rc = get_client_rect();
    return x < rc.right && y < rc.bottom;
  }

  const SIZE get_size() const
  {
    RECT rc = get_client_rect();
    SIZE s;
    s.cx = rc.right;
    s.cy = rc.bottom;
    return s;
  }

#ifdef ENABLE_SDL
  void paint() {
    on_paint(canvas);
  }

  /**
   * Ensures that the specified rectangle is updated on the physical
   * screen.
   */
  virtual void expose(const RECT &rect);

  /**
   * Ensures that the window is updated on the physical screen.
   */
  virtual void expose();
#else /* !ENABLE_SDL */
  void expose(const RECT &rect) {}
  void expose() {}
#endif /* !ENABLE_SDL */

#ifndef ENABLE_SDL
  static void *get_userdata(HWND hWnd) {
    return (void *)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
  }

  /**
   * Converts a #HWND into a #Window pointer, without checking if that
   * is legal.
   */
  static Window *get_unchecked(HWND hWnd) {
    return (Window *)get_userdata(hWnd);
  }

  /**
   * Converts a #HWND into a #Window pointer.  Returns NULL if the
   * HWND is not a Window peer.  This only works for windows which
   * have called install_wndproc().
   */
  static Window *get(HWND hWnd) {
    WNDPROC wndproc = (WNDPROC)::GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    return wndproc == WndProc
      ? get_unchecked(hWnd)
      : NULL;
  }
#endif

  void send_command(const Window &from) {
    assert_none_locked();

#ifdef ENABLE_SDL
    // XXX
#else /* !ENABLE_SDL */
    ::SendMessage(hWnd, WM_COMMAND, (WPARAM)0, (LPARAM)from.hWnd);
#endif /* !ENABLE_SDL */
  }

  void send_user(unsigned id) {
#ifdef ENABLE_SDL
    SDL_Event event;
    event.user.type = SDL_USEREVENT + id;
    event.user.code = 0;
    event.user.data1 = this;
    event.user.data2 = NULL;

    ::SDL_PushEvent(&event);
#else /* !ENABLE_SDL */
    ::PostMessage(hWnd, WM_USER + id, (WPARAM)0, (LPARAM)0);
#endif /* !ENABLE_SDL */
  }

protected:
#ifdef ENABLE_SDL
public:
#endif /* ENABLE_SDL */
  /**
   * @return true on success, false if the window should not be
   * created
   */
  virtual bool on_create();

  virtual bool on_destroy();
  virtual bool on_close();
  virtual bool on_resize(unsigned width, unsigned height);
  virtual bool on_mouse_move(int x, int y, unsigned keys);
  virtual bool on_mouse_down(int x, int y);
  virtual bool on_mouse_up(int x, int y);
  virtual bool on_mouse_double(int x, int y);
  virtual bool on_mouse_wheel(int delta);
  virtual bool on_key_down(unsigned key_code);
  virtual bool on_key_up(unsigned key_code);
  virtual bool on_command(unsigned id, unsigned code);
  virtual bool on_setfocus();
  virtual bool on_killfocus();
  virtual bool on_timer(timer_t id);
  virtual bool on_user(unsigned id);

#ifdef ENABLE_SDL
  virtual void on_paint(Canvas &canvas);

  virtual bool on_event(const SDL_Event &event);

#else /* !ENABLE_SDL */
  /**
   * Called by on_message() when the message was not handled by any
   * virtual method.  Calls the default handler.  This function is
   * virtual, because the Dialog class will have to override it -
   * dialogs have slightly different semantics.
   */
  virtual LRESULT on_unhandled_message(HWND hWnd, UINT message,
                                       WPARAM wParam, LPARAM lParam);

  virtual LRESULT on_message(HWND hWnd, UINT message,
                             WPARAM wParam, LPARAM lParam);
#endif /* !ENABLE_SDL */

public:
#ifdef ENABLE_SDL
  void install_wndproc() {
    // XXX
  }
#else /* !ENABLE_SDL */
  /**
   * This static method reads the Window* object from GWL_USERDATA and
   * calls on_message().
   */
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
                                  WPARAM wParam, LPARAM lParam);

  /**
   * Installs Window::WndProc() has the WNDPROC.  This enables the
   * methods on_*() methods, which may be implemented by sub classes.
   */
  void install_wndproc();
#endif /* !ENABLE_SDL */
};

#endif
