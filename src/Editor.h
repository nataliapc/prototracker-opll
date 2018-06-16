#pragma once

#include "SDL.h"
#include "Listener.h"
#include <functional>
#include <vector>

struct PlayerState;
struct Renderer;
struct EditorState;

/*

The Editor class is the base class for all GUI elements.

*/

class Editor: public Listener
{
public:
	static const int replacePreviousMessage = -1;

	enum MessageClass
	{
		MessageInfo,
		MessageError
	};

	typedef std::function<void()> Command;

	struct CommandDescriptor {
		char name[200];
		Command func;
		CommandDescriptor(const char *name, Command func);
	};

	struct EditorChild {
		Editor *editor;
		SDL_Rect area;
		EditorChild(Editor *editor, const SDL_Rect& area);
	};

private:
	Editor *mFocus;

	void drawModal(Renderer& renderer);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area) = 0;
	void drawChildren(Renderer& renderer, const SDL_Rect& area);
	void childAreaChanged(Editor *changedChild);

protected:
	Editor *mModal;
	EditorState& mEditorState;
	bool mIsDirty, mRedraw;
	Editor *mParent;
	std::vector<EditorChild> mChildren;
	SDL_Rect mThisArea;
	bool mWantsFocus;
	int mPopupMessageId;

	void removeFocus();
	void setModal(Editor *modal);

	void drawCoveredChildren(Renderer& renderer, const SDL_Rect& area, const SDL_Rect& childArea, int maxIndex);

	void invalidateAll();
	void invalidateParent();

	bool shouldRedrawBackground() const;
	virtual void onAreaChanged(const SDL_Rect& area);

	/* Actual rendering of the message */
	virtual int showMessageInner(MessageClass messageClass, int messageId, const char* message);

	// Register commands only here so that the Editor is added as a child and the registration
	// is propagated
	virtual void onRequestCommandRegistration();

	virtual bool registerCommand(const char *commandName, Command command);

public:
	Editor(EditorState& editorState, bool wantFocus = true);
	virtual ~Editor();

	void addChild(Editor *child, int x, int y, int w, int h);

	void setDirty(bool dirty);
	virtual void onFileSelectorEvent(const Editor& fileSelector, bool accept);
	virtual void onMessageBoxEvent(const Editor& messageBox, int code);
	virtual void onListenableChange(Listenable *listenable);
	virtual void onLoaded();
	virtual void onModalStatusChange(bool isNowModal);
	virtual bool isDirty() const;
	bool isFocusable() const;
	bool hasDirty() const;
	void setFocus(Editor *editor);
	Editor * getFocus();
	bool hasFocus();

	// Tell Editor its own top-left corner (absolute)
	void setArea(const SDL_Rect& area);

	const SDL_Rect& getArea() const;

	/**
	 * Messages
	 */

 	/**
	 * Tooltip - set in SDL_MOUSEMOTION
	 */
	virtual void showTooltip(const SDL_Rect& area, const char* message);
	void showTooltipV(const SDL_Rect& area, const char* message, ...) __attribute__((format(printf, 3, 4)));

	/**
	 * Status popup
	 */
	int showMessage(MessageClass messageClass, int messageId, const char* message);
	int showMessage(MessageClass messageClass, const char* message);
	int showMessageV(MessageClass messageClass, const char* message, ...) __attribute__((format(printf, 3, 4)));
	int showMessageV(MessageClass messageClass, int messageId, const char* message, ...) __attribute__((format(printf, 4, 5)));

	void draw(Renderer& renderer, const SDL_Rect& area);
	virtual void onUpdate(int ms);
	void update(int ms);

	/* onEvent() should return false if the event was not consumed
	 * so that the parent Editor knows to process it.
	 */
	virtual bool onEvent(SDL_Event& event);

	/**
	 * Helper members
	 */
	static bool pointInRect(const SDL_Point& point, const SDL_Rect& rect);
	static bool intersectRect(const SDL_Rect& a, const SDL_Rect& b, SDL_Rect& result);
};
