#include "KitikToolbar.h"

//==============================================================================
KitikToolbarItemComponent::KitikToolbarItemComponent(const int itemId_,
    const String& labelText,
    const bool isBeingUsedAsAButton_)
    : Button(labelText),
    itemId(itemId_),
    mode(normalMode),
    toolbarStyle(Toolbar::iconsOnly),
    dragOffsetX(0),
    dragOffsetY(0),
    isActive(true),
    isBeingDragged(false),
    isBeingUsedAsAButton(isBeingUsedAsAButton_)
{
    // Your item ID can't be 0!
    jassert(itemId_ != 0);
}

KitikToolbarItemComponent::~KitikToolbarItemComponent()
{
    overlayComp.reset();
}

KitikToolbar* KitikToolbarItemComponent::getToolbar() const
{
    return dynamic_cast<KitikToolbar*> (getParentComponent());
}

bool KitikToolbarItemComponent::isToolbarVertical() const
{
    const KitikToolbar* const t = getToolbar();
    return t != nullptr && t->isVertical();
}
void KitikToolbarItemComponent::setStyle(const Toolbar::ToolbarItemStyle& newStyle)
{
    /*if (toolbarStyle != newStyle)
    {
        toolbarStyle = newStyle;
        repaint();
        resized();
    }*/
}

void KitikToolbarItemComponent::paintButton(Graphics& g, const bool over, const bool down)
{
    auto currentToolbar = this->getToolbar();
    auto& childComps = currentToolbar->getChildren();
    for (auto* child : childComps)
    {
        if (auto* tbComp = dynamic_cast<KitikToolbarItemComponent*>(child))
        {
            if (tbComp->getName() != this->getName() && tbComp->isClicked && down)
            {
                tbComp->isClicked = false;
                tbComp->repaint();
                break;
            }
        }
    }

    if (isBeingUsedAsAButton)
    {
        if (down)
        {
            g.fillAll(Colour(64u, 194u, 230u));
            isClicked = true;
        }
        else if (over)
            g.fillAll(Colour(64u, 194u, 230u).withAlpha(.4f));
    }

    if (toolbarStyle != Toolbar::iconsOnly)
    {
        auto indent = contentArea.getX();
        auto y = indent;
        auto h = getHeight() - indent * 2;

        if (toolbarStyle == Toolbar::iconsWithText)
        {
            y = contentArea.getBottom() + indent / 2;
            h -= contentArea.getHeight();
        }

        g.setColour(this->findColour(Toolbar::labelTextColourId, true)
            .withAlpha(this->isEnabled() ? 1.0f : 0.25f));

        auto fontHeight = jmin(14.0f, (float)h * 0.85f);
        g.setFont(fontHeight);

        g.drawFittedText(getButtonText(),
            indent, y, getWidth() - indent * 2, h,
            Justification::centred,
            jmax(1, h / (int)fontHeight));
    }

    if (isClicked)
    {
        g.fillAll(Colour(64u, 194u, 230u).withAlpha(.7f));
    }
    else
        g.fillAll(Colours::black);

    if (!contentArea.isEmpty())
    {
        Graphics::ScopedSaveState ss(g);

        g.reduceClipRegion(contentArea);
        g.setOrigin(contentArea.getPosition());

        paintButtonArea(g, contentArea.getWidth(), contentArea.getHeight(), over, down);

    }
}

void KitikToolbarItemComponent::resized()
{
    if (toolbarStyle != Toolbar::textOnly)
    {
        const int indent = jmin(proportionOfWidth(0.08f),
            proportionOfHeight(0.08f));

        contentArea = Rectangle<int>(indent, indent,
            getWidth() - indent * 2,
            toolbarStyle == Toolbar::iconsWithText ? proportionOfHeight(0.55f)
            : (getHeight() - indent * 2));
    }
    else
    {
        contentArea = {};
    }

    contentAreaChanged(contentArea);
}

void KitikToolbarItemComponent::setEditingMode(const TestToolbarEditingMode newMode)
{
    if (mode != newMode)
    {
        mode = newMode;
        repaint();
        resized();
    }
}

//==============================================================================
std::unique_ptr<AccessibilityHandler> KitikToolbarItemComponent::createAccessibilityHandler()
{
    const auto shouldItemBeAccessible = (itemId != ToolbarItemFactory::separatorBarId
        && itemId != ToolbarItemFactory::spacerId
        && itemId != ToolbarItemFactory::flexibleSpacerId);

    return createIgnoredAccessibilityHandler(*this);

}

void KitikToolbarItemComponent::mouseDown(const MouseEvent& e)
{
    isDragging = false;
    auto name = this->getParentComponent()->getName();
    auto child = this->getName();
    //DBG("Name: " << child);

    if (KitikToolbarItemComponent* const tc = this)
    {
        tc->dragOffsetX = e.x;
        tc->dragOffsetY = e.y;
    }
}

void KitikToolbarItemComponent::mouseDrag(const MouseEvent& e)
{
    if (e.mouseWasDraggedSinceMouseDown() && !isDragging)
    {
        isDragging = true;

        auto mode = this->getEditingMode();

        if (!mode)
            return;

        if (DragAndDropContainer* const dnd = DragAndDropContainer::findParentDragContainerFor(this))
        {
            auto index = getIndexOfChildComponent(this);
            dnd->startDragging(KitikToolbar::toolbarDragDescriptor, this, ScaledImage(), true, nullptr, &e.source);

            if (KitikToolbarItemComponent* const tc = this)
            {
                tc->isBeingDragged = true;

                if (tc->getEditingMode() == KitikToolbarItemComponent::editableOnToolbar)
                    tc->setVisible(false);
            }
        }
    }
}

void KitikToolbarItemComponent::mouseUp(const MouseEvent&)
{
    isDragging = false;

    if (KitikToolbarItemComponent* const tc = this)
    {
        tc->isBeingDragged = false;

        if (KitikToolbar* const tb = tc->getToolbar())
            tb->updateAllItemPositions(true);
        else if (tc->getEditingMode() == ToolbarItemComponent::editableOnToolbar)
            delete tc;
    }


}


const char* const KitikToolbar::toolbarDragDescriptor = "_toolbarItem_";

//==============================================================================
class KitikToolbar::Spacer final : public ToolbarItemComponent
{
public:
    Spacer(int itemID, float sizeToUse, bool shouldDrawBar)
        : ToolbarItemComponent(itemID, {}, false),
        fixedSize(sizeToUse),
        drawBar(shouldDrawBar)
    {
        setWantsKeyboardFocus(false);
    }

    bool getToolbarItemSizes(int toolbarThickness, bool /*isToolbarVertical*/,
        int& preferredSize, int& minSize, int& maxSize) override
    {
        if (fixedSize <= 0)
        {
            preferredSize = toolbarThickness * 2;
            minSize = 4;
            maxSize = 32768;
        }
        else
        {
            maxSize = roundToInt((float)toolbarThickness * fixedSize);
            minSize = drawBar ? maxSize : jmin(4, maxSize);
            preferredSize = maxSize;

            if (getEditingMode() == editableOnPalette)
                preferredSize = maxSize = toolbarThickness / (drawBar ? 3 : 2);
        }

        return true;
    }

    void paintButtonArea(Graphics&, int, int, bool, bool) override
    {
    }

    void contentAreaChanged(const Rectangle<int>&) override
    {
    }

    int getResizeOrder() const noexcept
    {
        return fixedSize <= 0 ? 0 : 1;
    }

    void paint(Graphics& g) override
    {
        auto w = getWidth();
        auto h = getHeight();

        if (drawBar)
        {
            g.setColour(findColour(Toolbar::separatorColourId, true));

            auto thickness = 0.2f;

            if (isToolbarVertical())
                g.fillRect((float)w * 0.1f, (float)h * (0.5f - thickness * 0.5f), (float)w * 0.8f, (float)h * thickness);
            else
                g.fillRect((float)w * (0.5f - thickness * 0.5f), (float)h * 0.1f, (float)w * thickness, (float)h * 0.8f);
        }

        if (getEditingMode() != normalMode && !drawBar)
        {
            g.setColour(findColour(Toolbar::separatorColourId, true));

            auto indentX = jmin(2, (w - 3) / 2);
            auto indentY = jmin(2, (h - 3) / 2);
            g.drawRect(indentX, indentY, w - indentX * 2, h - indentY * 2, 1);

            if (fixedSize <= 0)
            {
                float x1, y1, x2, y2, x3, y3, x4, y4, hw, hl;

                if (isToolbarVertical())
                {
                    x1 = (float)w * 0.5f;
                    y1 = (float)h * 0.4f;
                    x2 = x1;
                    y2 = (float)indentX * 2.0f;

                    x3 = x1;
                    y3 = (float)h * 0.6f;
                    x4 = x1;
                    y4 = (float)h - y2;

                    hw = (float)w * 0.15f;
                    hl = (float)w * 0.2f;
                }
                else
                {
                    x1 = (float)w * 0.4f;
                    y1 = (float)h * 0.5f;
                    x2 = (float)indentX * 2.0f;
                    y2 = y1;

                    x3 = (float)w * 0.6f;
                    y3 = y1;
                    x4 = (float)w - x2;
                    y4 = y1;

                    hw = (float)h * 0.15f;
                    hl = (float)h * 0.2f;
                }

                Path p;
                p.addArrow({ x1, y1, x2, y2 }, 1.5f, hw, hl);
                p.addArrow({ x3, y3, x4, y4 }, 1.5f, hw, hl);
                g.fillPath(p);
            }
        }
    }

private:
    const float fixedSize;
    const bool drawBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spacer)
};

//==============================================================================
class KitikToolbar::MissingItemsComponent final : public PopupMenu::CustomComponent
{
public:
    MissingItemsComponent(KitikToolbar& bar, int h)
        : PopupMenu::CustomComponent(true),
        owner(&bar),
        height(h)
    {
        for (int i = bar.items.size(); --i >= 0;)
        {
            auto* tc = bar.items.getUnchecked(i);

            if (tc != nullptr && dynamic_cast<Spacer*> (tc) == nullptr && !tc->isVisible())
            {
                oldIndexes.insert(0, i);
                addAndMakeVisible(tc, 0);
            }
        }

        layout(400);
    }

    ~MissingItemsComponent() override
    {
        if (owner != nullptr)
        {
            for (int i = 0; i < getNumChildComponents(); ++i)
            {
                if (auto* tc = dynamic_cast<ToolbarItemComponent*> (getChildComponent(i)))
                {
                    tc->setVisible(false);
                    auto index = oldIndexes.removeAndReturn(i);
                    owner->addChildComponent(tc, index);
                    --i;
                }
            }

            owner->resized();
        }
    }

    void layout(const int preferredWidth)
    {
        const int indent = 8;
        auto x = indent;
        auto y = indent;
        int maxX = 0;

        for (auto* c : getChildren())
        {
            if (auto* tc = dynamic_cast<ToolbarItemComponent*> (c))
            {
                int preferredSize = 1, minSize = 1, maxSize = 1;

                if (tc->getToolbarItemSizes(height, false, preferredSize, minSize, maxSize))
                {
                    if (x + preferredSize > preferredWidth && x > indent)
                    {
                        x = indent;
                        y += height;
                    }

                    tc->setBounds(x, y, preferredSize, height);

                    x += preferredSize;
                    maxX = jmax(maxX, x);
                }
            }
        }

        setSize(maxX + 8, y + height + 8);
    }

    void getIdealSize(int& idealWidth, int& idealHeight) override
    {
        idealWidth = getWidth();
        idealHeight = getHeight();
    }

private:
    Component::SafePointer<KitikToolbar> owner;
    const int height;
    Array<int> oldIndexes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MissingItemsComponent)
};


//==============================================================================
KitikToolbar::KitikToolbar()
{
    lookAndFeelChanged();
    initMissingItemButton();
}

KitikToolbar::~KitikToolbar()
{
    items.clear();
}

void KitikToolbar::setVertical(const bool shouldBeVertical)
{
    if (vertical != shouldBeVertical)
    {
        vertical = shouldBeVertical;
        resized();
    }
}

void KitikToolbar::clear()
{
    items.clear();
    resized();
}

KitikToolbarItemComponent* KitikToolbar::createItem(TestToolbarItemFactory& factory, const int itemId)
{
    /*if (itemId == TestToolbarItemFactory::separatorBarId)    return new TestToolbar::Spacer(itemId, 0.1f, true);
    if (itemId == TestToolbarItemFactory::spacerId)          return new Spacer(itemId, 0.5f, false);
    if (itemId == TestToolbarItemFactory::flexibleSpacerId)  return new Spacer(itemId, 0.0f, false);*/

    return factory.createItem(itemId);
}

void KitikToolbar::addItemInternal(TestToolbarItemFactory& factory,
    const int itemId,
    const int insertIndex)
{
    // An ID can't be zero - this might indicate a mistake somewhere?
    jassert(itemId != 0);

    if (auto* tc = createItem(factory, itemId))
    {
#if JUCE_DEBUG
        Array<int> allowedIds;
        factory.getAllToolbarItemIds(allowedIds);

        // If your factory can create an item for a given ID, it must also return
        // that ID from its getAllToolbarItemIds() method!
        jassert(allowedIds.contains(itemId));
#endif

        items.insert(insertIndex, tc);
        addAndMakeVisible(tc, insertIndex);
    }
}

void KitikToolbar::addItem(TestToolbarItemFactory& factory, int itemId, int insertIndex)
{
    addItemInternal(factory, itemId, insertIndex);
    resized();
}

void KitikToolbar::addDefaultItems(TestToolbarItemFactory& factoryToUse)
{
    Array<int> ids;
    factoryToUse.getDefaultItemSet(ids);

    clear();

    for (auto i : ids)
        addItemInternal(factoryToUse, i, -1);

    resized();
}

void KitikToolbar::removeToolbarItem(const int itemIndex)
{
    items.remove(itemIndex);
    resized();
}

KitikToolbarItemComponent* KitikToolbar::removeAndReturnItem(const int itemIndex)
{
    if (auto* tc = items.removeAndReturn(itemIndex))
    {
        removeChildComponent(tc);
        resized();
        return tc;
    }

    return nullptr;
}

int KitikToolbar::getNumItems() const noexcept
{
    return items.size();
}

int KitikToolbar::getItemId(const int itemIndex) const noexcept
{

    if (auto* tc = getItemComponent(itemIndex))
        return tc->getItemId();

    return 0;
}

std::vector<int> KitikToolbar::getAllItems()
{
    std::vector<int> allItems;

    for (int i = 0; i < items.size(); i++)
    {
        if (auto* tc = getItemComponent(i))
            allItems.push_back(tc->getItemId());
    }

    return allItems;
}

KitikToolbarItemComponent* KitikToolbar::getItemComponent(const int itemIndex) const noexcept
{
    return items[itemIndex];
}

KitikToolbarItemComponent* KitikToolbar::getNextActiveComponent(int index, const int delta) const
{
    for (;;)
    {
        index += delta;

        if (auto* tc = getItemComponent(index))
        {
            if (tc->isActive)
                return tc;
        }
        else
        {
            return nullptr;
        }
    }
}

void KitikToolbar::setStyle(const ToolbarItemStyle& newStyle)
{
    if (toolbarStyle != newStyle)
    {
        toolbarStyle = newStyle;
        updateAllItemPositions(false);
    }
}

String KitikToolbar::toString() const
{
    String s("TB:");

    for (int i = 0; i < getNumItems(); ++i)
        s << getItemId(i) << ' ';

    return s.trimEnd();
}

bool KitikToolbar::restoreFromString(TestToolbarItemFactory& factoryToUse,
    const String& savedVersion)
{
    if (!savedVersion.startsWith("TB:"))
        return false;

    StringArray tokens;
    tokens.addTokens(savedVersion.substring(3), false);

    clear();

    for (auto& t : tokens)
        addItemInternal(factoryToUse, t.getIntValue(), -1);

    resized();
    return true;
}

void KitikToolbar::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.fillAll();
}

int KitikToolbar::getThickness() const noexcept
{
    return vertical ? getWidth() : getHeight();
}

int KitikToolbar::getLength() const noexcept
{
    return vertical ? getHeight() : getWidth();
}

void KitikToolbar::setEditingActive(const bool active)
{
    if (isEditingActive != active)
    {
        isEditingActive = active;
        updateAllItemPositions(false);
    }
}

//==============================================================================
void KitikToolbar::resized()
{
    updateAllItemPositions(false);
}

void KitikToolbar::mouseDown(const MouseEvent& me) {}

void KitikToolbar::updateAllItemPositions(bool animate)
{
    if (getWidth() > 0 && getHeight() > 0)
    {
        StretchableObjectResizer resizer;

        for (auto* tc : items)
        {
            tc->setEditingMode(isEditingActive ? KitikToolbarItemComponent::editableOnToolbar
                : KitikToolbarItemComponent::normalMode);

            tc->setStyle(static_cast<Toolbar::ToolbarItemStyle>(toolbarStyle));

            auto* spacer = dynamic_cast<Spacer*> (tc);

            int preferredSize = 1, minSize = 1, maxSize = 1;

            if (tc->getToolbarItemSizes(getThickness(), isVertical(),
                preferredSize, minSize, maxSize))
            {
                tc->isActive = true;
                resizer.addItem(preferredSize, minSize, maxSize,
                    spacer != nullptr ? spacer->getResizeOrder() : 2);
            }
            else
            {
                tc->isActive = false;
                tc->setVisible(false);
            }
        }

        resizer.resizeToFit(getLength());

        int totalLength = 0;

        for (int i = 0; i < resizer.getNumItems(); ++i)
            totalLength += (int)resizer.getItemSize(i);

        const bool itemsOffTheEnd = totalLength > getLength();

        auto extrasButtonSize = getThickness() / 2;
        missingItemsButton->setSize(extrasButtonSize, extrasButtonSize);
        missingItemsButton->setVisible(itemsOffTheEnd);
        missingItemsButton->setEnabled(!isEditingActive);

        if (vertical)
            missingItemsButton->setCentrePosition(getWidth() / 2,
                getHeight() - 4 - extrasButtonSize / 2);
        else
            missingItemsButton->setCentrePosition(getWidth() - 4 - extrasButtonSize / 2,
                getHeight() / 2);

        auto maxLength = itemsOffTheEnd ? (vertical ? missingItemsButton->getY()
            : missingItemsButton->getX()) - 4
            : getLength();

        int pos = 0, activeIndex = 0;

        for (auto* tc : items)
        {
            if (tc->isActive)
            {
                auto size = (int)resizer.getItemSize(activeIndex++);

                Rectangle<int> newBounds;

                if (vertical)
                    newBounds.setBounds(0, pos, getWidth(), size);
                else
                    newBounds.setBounds(pos, 0, size, getHeight());

                auto& animator = Desktop::getInstance().getAnimator();

                if (animate)
                {
                    animator.animateComponent(tc, newBounds, 1.0f, 200, false, 3.0, 0.0);
                }
                else
                {
                    animator.cancelAnimation(tc, false);
                    tc->setBounds(newBounds);
                }

                pos += size;
                tc->setVisible(pos <= maxLength
                    && ((!tc->isBeingDragged)
                        || tc->getEditingMode() == ToolbarItemComponent::editableOnPalette));
            }
        }
    }
}

//==============================================================================
void KitikToolbar::initMissingItemButton()
{
    if (missingItemsButton == nullptr)
        return;

    addChildComponent(*missingItemsButton);
    missingItemsButton->setAlwaysOnTop(true);
    missingItemsButton->onClick = [this] { showMissingItems(); };
}

void KitikToolbar::showMissingItems()
{
    jassert(missingItemsButton->isShowing());

    if (missingItemsButton->isShowing())
    {
        PopupMenu m;
        auto comp = std::make_unique<MissingItemsComponent>(*this, getThickness());
        m.addCustomItem(1, std::move(comp), nullptr, TRANS("Additional Items"));
        m.showMenuAsync(PopupMenu::Options().withTargetComponent(missingItemsButton.get()));
    }
}

//==============================================================================
bool KitikToolbar::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
    return dragSourceDetails.description == toolbarDragDescriptor && isEditingActive;
}

void KitikToolbar::itemDragMove(const SourceDetails& dragSourceDetails)
{
    if (auto* tc = dynamic_cast<KitikToolbarItemComponent*> (dragSourceDetails.sourceComponent.get()))
    {
        if (!items.contains(tc))
        {
            if (tc->getEditingMode() == KitikToolbarItemComponent::editableOnPalette)
            {
                if (auto* palette = tc->findParentComponentOfClass<TestToolbarItemPalette>())
                    palette->replaceComponent(*tc);
            }
            else
            {
                jassert(tc->getEditingMode() == KitikToolbarItemComponent::editableOnToolbar);
            }

            items.add(tc);
            addChildComponent(tc);
            updateAllItemPositions(true);
        }

        auto& animator = Desktop::getInstance().getAnimator();

        for (int i = getNumItems(); --i >= 0;)
        {
            auto currentIndex = items.indexOf(tc);
            auto newIndex = currentIndex;

            auto dragObjectLeft = vertical ? (dragSourceDetails.localPosition.getY() - tc->dragOffsetY)
                : (dragSourceDetails.localPosition.getX() - tc->dragOffsetX);
            auto dragObjectRight = dragObjectLeft + (vertical ? tc->getHeight() : tc->getWidth());

            auto current = animator.getComponentDestination(getChildComponent(newIndex));

            if (auto* prev = getNextActiveComponent(newIndex, -1))
            {
                auto previousPos = animator.getComponentDestination(prev);

                if (std::abs(dragObjectLeft - (vertical ? previousPos.getY() : previousPos.getX()))
                    < std::abs(dragObjectRight - (vertical ? current.getBottom() : current.getRight())))
                {
                    newIndex = getIndexOfChildComponent(prev);
                }
            }

            if (auto* next = getNextActiveComponent(newIndex, 1))
            {
                auto nextPos = animator.getComponentDestination(next);

                if (std::abs(dragObjectLeft - (vertical ? current.getY() : current.getX()))
                    > std::abs(dragObjectRight - (vertical ? nextPos.getBottom() : nextPos.getRight())))
                {
                    newIndex = getIndexOfChildComponent(next) + 1;
                }
            }

            if (newIndex == currentIndex)
                break;

            items.removeObject(tc, false);
            removeChildComponent(tc);
            addChildComponent(tc, newIndex);
            items.insert(newIndex, tc);
            updateAllItemPositions(true);
        }
    }
}

void KitikToolbar::itemDragExit(const SourceDetails& dragSourceDetails)
{
    if (auto* tc = dynamic_cast<KitikToolbarItemComponent*> (dragSourceDetails.sourceComponent.get()))
    {
        auto currentIndex = items.indexOf(tc);

        if (isParentOf(tc))
        {
            items.removeObject(tc, false);
            removeChildComponent(tc);
            addChildComponent(tc, currentIndex);
            items.insert(currentIndex, tc);
            updateAllItemPositions(true);
        }
    }
}

void KitikToolbar::itemDropped(const SourceDetails& dragSourceDetails)
{
    if (auto* tc = dynamic_cast<ToolbarItemComponent*> (dragSourceDetails.sourceComponent.get()))
        tc->setState(Button::buttonNormal);
}

void KitikToolbar::lookAndFeelChanged()
{
    missingItemsButton.reset(getLookAndFeel().createToolbarMissingItemsButton(juce::Toolbar{}));
    initMissingItemButton();
}

//==============================================================================
class KitikToolbar::CustomisationDialog final : public DialogWindow
{
public:
    CustomisationDialog(TestToolbarItemFactory& factory, KitikToolbar& bar, int optionFlags)
        : DialogWindow(TRANS("Add/remove items from toolbar"), Colours::white, true, true),
        toolbar(bar)
    {
        setContentOwned(new CustomiserPanel(factory, toolbar, optionFlags), true);
        setResizable(true, true);
        setResizeLimits(400, 300, 1500, 1000);
        positionNearBar();
    }

    ~CustomisationDialog() override
    {
        toolbar.setEditingActive(false);
    }

    void closeButtonPressed() override
    {
        setVisible(false);
    }

    bool canModalEventBeSentToComponent(const Component* comp) override
    {
        return true;
    }

    void positionNearBar()
    {
        auto screenSize = toolbar.getParentMonitorArea();
        auto pos = toolbar.getScreenPosition();
        const int gap = 8;

        if (toolbar.isVertical())
        {
            if (pos.x > screenSize.getCentreX())
                pos.x -= getWidth() - gap;
            else
                pos.x += toolbar.getWidth() + gap;
        }
        else
        {
            pos.x += (toolbar.getWidth() - getWidth()) / 2;

            if (pos.y > screenSize.getCentreY())
                pos.y -= getHeight() - gap;
            else
                pos.y += toolbar.getHeight() + gap;
        }

        setTopLeftPosition(pos);
    }

private:
    KitikToolbar& toolbar;

    class CustomiserPanel : public Component
    {
    public:
        CustomiserPanel(TestToolbarItemFactory& tbf, KitikToolbar& bar, int optionFlags)
            : factory(tbf), toolbar(bar), palette(tbf, bar),
            instructions({}, TRANS("You can drag the items above and drop them onto a toolbar to add them.")
                + "\n\n"
                + TRANS("Items on the toolbar can also be dragged around to change their order, or dragged off the edge to delete them.")),
            defaultButton(TRANS("Restore to default set of items"))
        {
            addAndMakeVisible(palette);

            if ((optionFlags & (Toolbar::allowIconsOnlyChoice
                | Toolbar::allowIconsWithTextChoice
                | Toolbar::allowTextOnlyChoice)) != 0)
            {
                addAndMakeVisible(styleBox);
                styleBox.setEditableText(false);

                if ((optionFlags & Toolbar::allowIconsOnlyChoice) != 0)     styleBox.addItem(TRANS("Show icons only"), 1);
                if ((optionFlags & Toolbar::allowIconsWithTextChoice) != 0) styleBox.addItem(TRANS("Show icons and descriptions"), 2);
                if ((optionFlags & Toolbar::allowTextOnlyChoice) != 0)      styleBox.addItem(TRANS("Show descriptions only"), 3);

                int selectedStyle = 0;
                switch (bar.getStyle())
                {
                case Toolbar::iconsOnly:      selectedStyle = 1; break;
                case Toolbar::iconsWithText:  selectedStyle = 2; break;
                case Toolbar::textOnly:       selectedStyle = 3; break;
                default:                      break;
                }

                styleBox.setSelectedId(selectedStyle);

                styleBox.onChange = [this] { updateStyle(); };
            }

            if ((optionFlags & Toolbar::showResetToDefaultsButton) != 0)
            {
                addAndMakeVisible(defaultButton);
                defaultButton.onClick = [this] { toolbar.addDefaultItems(factory); };
            }

            addAndMakeVisible(instructions);
            instructions.setFont(Font(13.0f));

            setSize(500, 300);
        }

        void updateStyle()
        {
            switch (styleBox.getSelectedId())
            {
            case 1:   toolbar.setStyle(KitikToolbar::iconsOnly); break;
            case 2:   toolbar.setStyle(KitikToolbar::iconsWithText); break;
            case 3:   toolbar.setStyle(KitikToolbar::textOnly); break;
            default:  break;
            }

            palette.resized(); // to make it update the styles
        }

        void paint(Graphics& g) override
        {
            Colour background;

            if (auto* dw = findParentComponentOfClass<DialogWindow>())
                background = dw->getBackgroundColour();

            g.setColour(background.contrasting().withAlpha(0.3f));
            g.fillRect(palette.getX(), palette.getBottom() - 1, palette.getWidth(), 1);
        }

        void resized() override
        {
            palette.setBounds(0, 0, getWidth(), getHeight() - 120);
            styleBox.setBounds(10, getHeight() - 110, 200, 22);

            defaultButton.changeWidthToFitText(22);
            defaultButton.setTopLeftPosition(240, getHeight() - 110);

            instructions.setBounds(10, getHeight() - 80, getWidth() - 20, 80);
        }

    private:
        TestToolbarItemFactory& factory;
        KitikToolbar& toolbar;

        TestToolbarItemPalette palette;
        Label instructions;
        ComboBox styleBox;
        TextButton defaultButton;
    };
};

void KitikToolbar::showCustomisationDialog(TestToolbarItemFactory& factory, const int optionFlags)
{
    setEditingActive(true);

    (new CustomisationDialog(factory, *this, optionFlags))
        ->enterModalState(true, nullptr, true);
}

//==============================================================================
std::unique_ptr<AccessibilityHandler> KitikToolbar::createAccessibilityHandler()
{
    return std::make_unique<AccessibilityHandler>(*this, AccessibilityRole::group);
}


TestToolbarItemPalette::TestToolbarItemPalette(TestToolbarItemFactory& tbf, KitikToolbar& bar)
    : factory(tbf), toolbar(bar)
{
    auto* itemHolder = new Component();
    viewport.setViewedComponent(itemHolder);

    Array<int> allIds;
    factory.getAllToolbarItemIds(allIds);

    for (auto& i : allIds)
        addComponent(i, -1);

    addAndMakeVisible(viewport);
}

TestToolbarItemPalette::~TestToolbarItemPalette()
{
}

//==============================================================================
void TestToolbarItemPalette::addComponent(const int itemId, const int index)
{
    if (auto* tc = KitikToolbar::createItem(factory, itemId))
    {
        items.insert(index, tc);
        viewport.getViewedComponent()->addAndMakeVisible(tc, index);
        tc->setEditingMode(KitikToolbarItemComponent::editableOnPalette);
    }
    else
    {
        jassertfalse;
    }
}

void TestToolbarItemPalette::replaceComponent(KitikToolbarItemComponent& comp)
{
    auto index = items.indexOf(&comp);
    jassert(index >= 0);
    items.removeObject(&comp, false);

    addComponent(comp.getItemId(), index);
    resized();
}

void TestToolbarItemPalette::resized()
{
    viewport.setBoundsInset(BorderSize<int>(1));

    auto* itemHolder = viewport.getViewedComponent();

    const int indent = 8;
    const int preferredWidth = viewport.getWidth() - viewport.getScrollBarThickness() - indent;
    const int height = toolbar.getThickness();
    auto x = indent;
    auto y = indent;
    int maxX = 0;

    for (auto* tc : items)
    {
        tc->setStyle(static_cast<Toolbar::ToolbarItemStyle>(toolbar.getStyle()));

        int preferredSize = 1, minSize = 1, maxSize = 1;

        if (tc->getToolbarItemSizes(height, false, preferredSize, minSize, maxSize))
        {
            if (x + preferredSize > preferredWidth && x > indent)
            {
                x = indent;
                y += height;
            }

            tc->setBounds(x, y, preferredSize, height);

            x += preferredSize + 8;
            maxX = jmax(maxX, x);
        }
    }

    itemHolder->setSize(maxX, y + height + 8);
}

//==============================================================================
std::unique_ptr<AccessibilityHandler> TestToolbarItemPalette::createAccessibilityHandler()
{
    return std::make_unique<AccessibilityHandler>(*this, AccessibilityRole::group);
}

TestToolbarItemFactory::TestToolbarItemFactory() {}

TestToolbarItemFactory::~TestToolbarItemFactory() {}
