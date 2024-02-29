#pragma once

#include <JuceHeader.h>

using namespace juce;

class KitikToolbar;
class KitikToolbarFactory;

class KitikToolbarItemComponent : public Button
{
public:

    KitikToolbarItemComponent(int itemId,
        const String& labelText,
        bool isBeingUsedAsAButton);

    /** Destructor. */
    ~KitikToolbarItemComponent() override;


    int getItemId() const noexcept { return itemId; }

    KitikToolbar* getToolbar() const;

    bool isToolbarVertical() const;

    Toolbar::ToolbarItemStyle getStyle() const noexcept { return toolbarStyle; }

    virtual void setStyle(const Toolbar::ToolbarItemStyle& newStyle);

    Rectangle<int> getContentArea() const noexcept { return contentArea; }

    virtual bool getToolbarItemSizes(int toolbarThickness,
        bool isToolbarVertical,
        int& preferredSize,
        int& minSize,
        int& maxSize) = 0;

    virtual void paintButtonArea(Graphics& g,
        int width, int height,
        bool isMouseOver, bool isMouseDown) = 0;

    virtual void contentAreaChanged(const Rectangle<int>& newBounds) = 0;

    enum TestToolbarEditingMode
    {
        normalMode = 0,     /**< Means that the component is active, inside a toolbar. */
        editableOnToolbar,  /**< Means that the component is on a toolbar, but the toolbar is in
                                 customisation mode, and the items can be dragged around. */
        editableOnPalette   /**< Means that the component is on an new-item palette, so it can be
                                 dragged onto a toolbar to add it to that bar.*/
    };

    void setEditingMode(TestToolbarEditingMode newMode);

    TestToolbarEditingMode getEditingMode() const noexcept { return mode; }


    //==============================================================================
    /** @internal */
    void paintButton(Graphics&, bool isMouseOver, bool isMouseDown) override;
    /** @internal */
    void resized() override;
    /** @internal */
    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override;

    bool isClicked{ false };

private:
    friend class KitikToolbar;
    friend class detail::ToolbarItemDragAndDropOverlayComponent;

    const int itemId;
    TestToolbarEditingMode mode;
    Toolbar::ToolbarItemStyle toolbarStyle;
    std::unique_ptr<Component> overlayComp;
    int dragOffsetX, dragOffsetY;
    bool isActive, isBeingDragged, isBeingUsedAsAButton;
    Rectangle<int> contentArea;
    bool isDragging = 0;

    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KitikToolbarItemComponent)
};


class TestToolbarItemFactory
{
public:
    //==============================================================================
    TestToolbarItemFactory();

    /** Destructor. */
    virtual ~TestToolbarItemFactory();

    //==============================================================================
    /** A set of reserved item ID values, used for the built-in item types.
    */
    enum SpecialItemIds
    {
        separatorBarId = -1,   /**< The item ID for a vertical (or horizontal) separator bar that
                                            can be placed between sets of items to break them into groups. */
        spacerId = -2,   /**< The item ID for a fixed-width space that can be placed between
                                            items.*/
        flexibleSpacerId = -3    /**< The item ID for a gap that pushes outwards against the things on
                                            either side of it, filling any available space. */
    };

    virtual void getAllToolbarItemIds(Array <int>& ids) = 0;

    virtual void getDefaultItemSet(Array <int>& ids) = 0;

    virtual KitikToolbarItemComponent* createItem(int itemId) = 0;
};


class KitikToolbar : public Component,
    public DragAndDropContainer,
    public DragAndDropTarget
{
public:

    KitikToolbar();

    ~KitikToolbar() override;

    void setVertical(bool shouldBeVertical);

    bool isVertical() const noexcept { return vertical; }

    int getThickness() const noexcept;

    int getLength() const noexcept;

    void clear();

    void addItem(TestToolbarItemFactory& factory,
        int itemId,
        int insertIndex = -1);

    /** Deletes one of the items from the bar. */
    void removeToolbarItem(int itemIndex);

    /** Removes an item from the bar and returns it. */
    KitikToolbarItemComponent* removeAndReturnItem(int itemIndex);

    int getNumItems() const noexcept;

    int getItemId(int itemIndex) const noexcept;

    std::vector<int> KitikToolbar::getAllItems();

    KitikToolbarItemComponent* getItemComponent(int itemIndex) const noexcept;

    void addDefaultItems(TestToolbarItemFactory& factoryToUse);

    enum ToolbarItemStyle
    {
        iconsOnly,       /**< Means that the toolbar should just contain icons. */
        iconsWithText,   /**< Means that the toolbar should have text labels under each icon. */
        textOnly         /**< Means that the toolbar only display text labels for each item. */
    };

    ToolbarItemStyle getStyle() const noexcept { return toolbarStyle; }

    void setStyle(const ToolbarItemStyle& newStyle);

    //==============================================================================
    /** Flags used by the showCustomisationDialog() method. */
    enum CustomisationFlags
    {
        allowIconsOnlyChoice = 1,    /**< If this flag is specified, the customisation dialog can
                                                        show the "icons only" option on its choice of toolbar styles. */
        allowIconsWithTextChoice = 2,    /**< If this flag is specified, the customisation dialog can
                                                        show the "icons with text" option on its choice of toolbar styles. */
        allowTextOnlyChoice = 4,    /**< If this flag is specified, the customisation dialog can
                                                        show the "text only" option on its choice of toolbar styles. */
        showResetToDefaultsButton = 8,    /**< If this flag is specified, the customisation dialog can
                                                        show a button to reset the toolbar to its default set of items. */

        allCustomisationOptionsEnabled = (allowIconsOnlyChoice | allowIconsWithTextChoice | allowTextOnlyChoice | showResetToDefaultsButton)
    };

    void showCustomisationDialog(TestToolbarItemFactory& factory,
        int optionFlags = allCustomisationOptionsEnabled);

    void setEditingActive(bool editingEnabled);

    enum ColourIds
    {
        backgroundColourId = 0x1003200,  /**< A colour to use to fill the toolbar's background. For
                                                        more control over this, override LookAndFeel::paintToolbarBackground(). */
        separatorColourId = 0x1003210,  /**< A colour to use to draw the separator lines. */

        buttonMouseOverBackgroundColourId = 0x1003220,  /**< A colour used to paint the background of buttons when the mouse is
                                                                over them. */
        buttonMouseDownBackgroundColourId = 0x1003230,  /**< A colour used to paint the background of buttons when the mouse is
                                                                held down on them. */

        labelTextColourId = 0x1003240,        /**< A colour to use for drawing the text under buttons
                                                                when the style is set to iconsWithText or textOnly. */

        editingModeOutlineColourId = 0x1003250   /**< A colour to use for an outline around buttons when
                                                        the customisation dialog is active and the mouse moves over them. */
    };

    String toString() const;

    bool restoreFromString(TestToolbarItemFactory& factoryToUse,
        const String& savedVersion);

    //==============================================================================
    /** This abstract base class is implemented by LookAndFeel classes. */
    struct JUCE_API  LookAndFeelMethods
    {
        virtual ~LookAndFeelMethods() = default;

        virtual void paintToolbarBackground(Graphics&, int width, int height, KitikToolbar&) = 0;

        virtual Button* createToolbarMissingItemsButton(KitikToolbar&) = 0;

        virtual void paintToolbarButtonBackground(Graphics&, int width, int height,
            bool isMouseOver, bool isMouseDown,
            KitikToolbarItemComponent&) = 0;

        virtual void paintToolbarButtonLabel(Graphics&, int x, int y, int width, int height,
            const String& text, KitikToolbarItemComponent&) = 0;
    };

    //==============================================================================
    /** @internal */
    void paint(Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void mouseDown(const MouseEvent& me) override;
    /** @internal */
    bool isInterestedInDragSource(const SourceDetails&) override;
    /** @internal */
    void itemDragMove(const SourceDetails&) override;
    /** @internal */
    void itemDragExit(const SourceDetails&) override;
    /** @internal */
    void itemDropped(const SourceDetails&) override;
    /** @internal */
    void lookAndFeelChanged() override;
    /** @internal */
    void updateAllItemPositions(bool animate);
    /** @internal */
    static KitikToolbarItemComponent* createItem(TestToolbarItemFactory&, int itemId);
    /** @internal */
    static const char* const toolbarDragDescriptor;
    /** @internal */
    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override;

private:
    //==============================================================================
    std::unique_ptr<Button> missingItemsButton;
    bool vertical = false, isEditingActive = false;
    ToolbarItemStyle toolbarStyle = iconsOnly;
    class MissingItemsComponent;
    friend class MissingItemsComponent;
    OwnedArray<KitikToolbarItemComponent> items;
    class Spacer;
    class CustomisationDialog;

    void initMissingItemButton();
    void showMissingItems();
    void addItemInternal(TestToolbarItemFactory& factory, int itemId, int insertIndex);

    KitikToolbarItemComponent* getNextActiveComponent(int index, int delta) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KitikToolbar)
};


class TestToolbarItemPalette : public Component,
    public DragAndDropContainer
{
public:
    TestToolbarItemPalette(TestToolbarItemFactory& factory,
        KitikToolbar& toolbar);

    /** Destructor. */
    ~TestToolbarItemPalette() override;

    //==============================================================================
    /** @internal */
    void resized() override;
    /** @internal */
    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override;

private:
    TestToolbarItemFactory& factory;
    KitikToolbar& toolbar;
    Viewport viewport;
    OwnedArray<KitikToolbarItemComponent> items;

    friend class KitikToolbar;
    void replaceComponent(KitikToolbarItemComponent&);
    void addComponent(int itemId, int index);

};

